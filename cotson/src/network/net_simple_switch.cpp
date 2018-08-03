// -*- C++ -*-
// (C) Copyright 2006-2009 Hewlett-Packard Development Company, L.P.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//

// $Id: net_simple_switch.cpp 187 2011-04-11 16:10:57Z paolofrb $

#include "net_timing_model.h"
#include "liboptions.h"
#include "parameters.h"
#include <iostream>
#include <fstream>

using namespace std;

// A VERY simple switch timing nodel.

class SimpleSwitch : public TimingModel
{
public:
    SimpleSwitch(Parameters &);
    ~SimpleSwitch() {};

    void startquantum(uint64_t gt, uint64_t nextgt);
    void packet_event(const MacAddress& src, const MacAddress& dst, int size, bool bcast = false);
    uint64_t get_delay();
    uint32_t register_lat() { return 0; }

protected:

    uint64_t max_bw_;
    uint64_t latency_;
    uint64_t cfactor_;

protected:
    // Current quantum size in ns
    uint64_t quantum_;

    // Messages and bytes processed in the current quantum
    uint64_t messages_;
    uint64_t bytes_;
    uint64_t cmessages_; // congested messages
};

namespace simple_switch {
	registerClass<TimingModel,SimpleSwitch> mediator_simple_timer_c("simple");
};

SimpleSwitch::SimpleSwitch(Parameters &p) :
        max_bw_(p.get<int>("max_bw","80000",false)),
        latency_(p.get<int>("latency","0",false)),
        cfactor_(p.get<int>("cfactor","8",false)),
        quantum_(0),
        messages_(0),
        bytes_(0)
{
	// Don't use verbose, not yet set
    cout << "simple: latency=" << latency_ << endl;
    cout << "simple: max_bw=" << max_bw_ << endl;
    cout << "simple: cfactor=" << cfactor_ << endl;
	add("packets",messages_);
	add("congested_packets",cmessages_);
	add("bytes",bytes_);

	clear_metrics();
}

void 
SimpleSwitch::startquantum(uint64_t gt, uint64_t nextgt)
{
	if (verbose_ > 1)
	    cout << "simple: startquantum " << gt << " " << nextgt << endl;
	uint64_t q = nextgt - gt;
    if (q != 0)
	    quantum_ = q;
	clear_metrics();
}

void
SimpleSwitch::packet_event(const MacAddress& src, const MacAddress& dst, int size, bool bcast)
{
    messages_++;
    bytes_ += size;
}

uint64_t
SimpleSwitch::get_delay()
{
    // Models a simple switch with constant throuput below some value 
	// if max bandwith == 0 there is no limit

    if (max_bw_ == 0) 
	    return latency_;

	if (verbose_ > 2)
	    cout << "simple: bytes " << bytes_ 
		     << " quantum " << quantum_ 
			 << " maxbw " << max_bw_ << endl;

    if (bytes_* 8 < quantum_* max_bw_ * 1000000) {
	    return latency_;
	}
    else {
		cmessages_++;
		if (verbose_ > 2)
		    cout << "simple: congestion" << endl;
	    return latency_ * cfactor_; // N times more latency
    }
}

/* vi: set sw=4 ts=4: */
