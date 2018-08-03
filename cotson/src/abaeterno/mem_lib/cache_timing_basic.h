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

// $Id: cache_timing_basic.h 721 2015-06-03 02:29:33Z paolofrb $
#ifndef TIMING_BASIC_H
#define TIMING_BASIC_H

#include "libmetric.h"
#include "liboptions.h"
#include "memory_interface.h"

namespace Memory {
namespace Timing {

class Basic : public metric
{
public:
	Basic(const Parameters &p):
	    rlatency(p.get<uint32_t>("latency")),
	    wlatency(p.get<uint32_t>("write_latency","0", false)), // no track
		slots(p.get<uint32_t>("slots","0",false)), // no track
		sdelay_(0)
	{
		if (slots > 0) {
		    add ("slot_delay",sdelay_);
		    add ("slot_write_delay",swdelay_);
		    for(size_t i=0;i<slots;++i)
		        stime.push(0);
	    }
	}
	INLINE void reset()
	{ 
		for(size_t i=0;i<slots;++i) {
		    stime.pop();
		    stime.push(0);
	    }
	}
	INLINE uint32_t latency(uint64_t tstamp,const Trace& mt, const Access& ma)
	{
		uint32_t lat = rlatency;
		bool is_write = false;
		if (wlatency && mt.getType() == Memory::WRITE) {
		    lat = wlatency; // hold the slot for the write latency
			is_write = true;
		}

		if (!slots)
		    return lat; 

		// top slot is first to become free
		uint64_t tfree = stime.top();
		stime.pop();
		if (tfree <= tstamp) { // slot is free
		    stime.push(tstamp+lat);
			return lat;
		}
		else { // latency = next slots free time
		    stime.push(tfree+lat);
			uint64_t delay = (tfree-tstamp);
			sdelay_ += delay;
			if (is_write) 
			    swdelay_ += delay;
		    return lat+delay;
	    }
	}
private:
    const uint32_t rlatency;
    const uint32_t wlatency;
    const uint32_t slots;
	uint64_t sdelay_;
	uint64_t swdelay_;
	std::priority_queue< uint64_t,std::vector<uint64_t>,std::greater<uint64_t> > stime;
};

}
}
#endif

