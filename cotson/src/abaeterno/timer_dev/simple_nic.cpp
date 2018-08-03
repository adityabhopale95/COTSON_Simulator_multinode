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

// $Id: simple_nic.cpp 11 2010-01-08 13:20:58Z paolofrb $
//
// #define _DEBUG_THIS_

#include "abaeterno_config.h"
#include "logger.h"
#include "nic_timer.h"
#include "liboptions.h"

using namespace std;

class SimpleNic : public NicTimer
{
public: 
	SimpleNic(Parameters&);
	~SimpleNic();
	uint32_t accept(bool,bool,uint64_t,const void*);

private: 
	uint32_t update_delay(double&,double);
	const double setup;
	const double thruput;
	const int min_delay;

	double rdelay, wdelay;
	uint64_t num_reads;
	uint64_t num_writes;
	uint64_t tot_rdelay;
	uint64_t tot_wdelay;
	uint64_t bytes_read;
	uint64_t bytes_written;
};

registerClass<NicTimer,SimpleNic> simple_nic_c("simple_nic");

SimpleNic::SimpleNic(Parameters& p) :
	setup(p.get<double>("setup","0.0")*1e-3), // from ms to seconds
	thruput(p.get<double>("thruput","1000M")), 
	min_delay(p.get<int>("min_delay","0")),
	rdelay(0.0), wdelay(0.0)
{
	LOG("Nic init thruput:",thruput);

	add("bytes_read",bytes_read);
	add("bytes_written",bytes_written);
	add("tot_rdelay",tot_rdelay);
	add("tot_wdelay",tot_wdelay);
	add("pack_read",num_reads);
	add("pack_written",num_writes);
	clear_metrics();
}

SimpleNic::~SimpleNic() {}

uint32_t SimpleNic::update_delay(double& xd, double d)
{
    xd += d;
    uint32_t us = (xd > 1000.0) ? lround(xd/1000.0) : min_delay;
	xd -= static_cast<double>(us) * 1000.0;
	return us;
}

uint32_t SimpleNic::accept(
    bool dma, 
    bool read,
    uint64_t bytes, 
    const void* data
) {
	double delay = (setup + static_cast<double>(bytes) * 8.0 / thruput ) * 1e9 ; // in ns
	if (read) {
		uint32_t rd = update_delay(rdelay,delay);
        num_reads++;
        tot_rdelay += rd;
        bytes_read += bytes;
	    LOG("read: bytes",bytes,"delay",rd);
		return rd;
	}
	else { // write
		uint32_t wd = update_delay(wdelay,delay);
        num_writes++;
        tot_wdelay += wd;
        bytes_written += bytes;
	    LOG("write: bytes",bytes,"delay",wd);
		return wd;
	}
}
/* vi: set sw=4 ts=4: */
