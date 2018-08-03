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

#ifndef MEM_BINTRACER_H
#define MEM_BINTRACER_H

#include "memory_interface.h"
#include "liboptions.h"
//#include "dump_gzip.h"

namespace Memory {

class TraceMem
{
	public:
	TraceMem()
	{
		phys =((uint64_t)1)<<63;
		tstamp = length = nanos = 0;
	}
	TraceMem(bool r, uint64_t p, uint64_t t, uint32_t l, uint64_t n)
	{
		phys = p | ((r?(uint64_t)1:(uint64_t)0)<<63);
		tstamp = t;
		length = l;
		nanos = n;
	};
	bool isRead(void)
	{
		return (phys>>63?true:false);
	};
	uint64_t getPhys(void)
	{
		return(phys & 0x7fffffffffffffff);
	};
	uint64_t getTstamp(void)
	{
		return(tstamp);
	}
	uint32_t getLength(void)
	{
		return length;
	}
	uint64_t getNanos(void)
	{
		return nanos;
	}

	uint64_t phys;
	uint64_t tstamp;
	uint32_t length;
	uint64_t nanos;
};


class MemBinTracer : public Interface
{
	public:
	MemBinTracer(const Parameters&);
	~MemBinTracer(){std::cout << "Tracing ended." << std::endl;	/*tracefile.close();*/}

	MemState read(const Access&,uint64_t,Trace&,MOESI_state);
	MemState readx(const Access&,uint64_t,Trace&,MOESI_state);
	MemState write(const Access&,uint64_t,Trace&,MOESI_state);

	MOESI_state state(const Access&,uint64_t);
	void invalidate(const Access&,uint64_t,const Trace&,MOESI_state);

	uint32_t item_size() const { return linesize_; };

	private:

	//std::string filename;
	//std::ofstream tracefile;
	DumpGzip gz;

	uint32_t latency;

	bool trace_on;

	uint64_t read_;
	uint64_t write_;
	uint32_t linesize_;
};
}

std::ostream& operator<<(std::ostream&,const Memory::TraceMem&);
DumpGzip& operator<<(DumpGzip&,const Memory::TraceMem&);
ReadGzip& operator>>(ReadGzip&,Memory::TraceMem&);

#endif
