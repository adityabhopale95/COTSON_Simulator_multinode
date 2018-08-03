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

// $Id$

#ifndef TIMINGMODEL_H_
#define TIMINGMODEL_H_

#include "net_typedefs.h"

#include <string>
#include <map>
#include <boost/program_options.hpp>
#include <boost/pool/detail/mutex.hpp>
#include <boost/ref.hpp>
#include <boost/version.hpp> // brings BOOST_VERSION

#include "metric.h"

// Abstract class to derive the network timing models.
//

class TimingModel : public metric
{
public:
    TimingModel();
    virtual ~TimingModel();

	void startquantum_mt(uint64_t gt, uint64_t nextgt)
	{
		mux.lock();
		startquantum(gt,nextgt);
		mux.unlock();
	}
    
    // Notify that a new simulation quantum is starting
    virtual void startquantum(uint64_t gt, uint64_t nextgt) = 0;

	virtual void packet_event_mt(const MacAddress& src, const MacAddress& dst, int size, bool bcast = false)
	{
		mux.lock();
		packet_event(src,dst,size,bcast);
		mux.unlock();
	}
	
    // Notify about a packet event
    virtual void packet_event(const MacAddress& src, const MacAddress& dst, int size, bool bcast = false) = 0;

    // Return the delay of the last packet
    virtual uint64_t get_delay() = 0;

    // Return the latency of the timing model
    virtual uint32_t register_lat() = 0;

	// Set verbosity
	virtual void verbose(int);

protected:
    int verbose_;
    
#if BOOST_VERSION < 104800
    boost::details::pool::pthread_mutex mux;
#else
    // pthread_mutex is not available in BOOST >= 1.48 ..
    // --> SHOULD BE TESTED (RG120601)
    boost::details::pool::default_mutex mux;
#endif
};

#endif /*TIMINGMODEL_H_*/
