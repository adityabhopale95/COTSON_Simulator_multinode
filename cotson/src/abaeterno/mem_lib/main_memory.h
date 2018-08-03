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

// $Id: main_memory.h 721 2015-06-03 02:29:33Z paolofrb $
#ifndef MAIN_MEMORY_H
#define MAIN_MEMORY_H

#include "memory_interface.h"
#include "liboptions.h"
#include "error.h"

namespace Memory {

template<typename Timer>
class Main : public Interface
{
public:
    Main(const Parameters&);

	MemState read(const Access&,uint64_t,Trace&,MOESI_state);
	MemState readx(const Access&,uint64_t,Trace&,MOESI_state);
	MemState write(const Access&,uint64_t,Trace&,MOESI_state);
    MOESI_state state(const Access&,uint64_t);
    void invalidate(const Access&,uint64_t,const Trace&,MOESI_state);

    void updatePending() { };
	uint32_t item_size() const { return linesize_; }
    
protected:
    void beginSimulation() {
      timer_.reset();
      updatePending();
    }

private:
    uint64_t read_;
    uint64_t write_;
    uint64_t access_;  // read + write + coherence traffic
	uint32_t linesize_;

    Timer timer_;
};

template <typename Timer>
Main<Timer>::Main(const Parameters& p) :
	linesize_(p.get<uint32_t>("linesize","64",false)), // do not track
    timer_(p)
{
	name=p.get<std::string>("name");

	add("read", read_);
	add("write", write_);
    add("access", access_);
	add("",timer_);

	clear_metrics();
}

template <typename Timer>
MemState Main<Timer>::read(const Access& m,uint64_t tstamp,Trace& mt,MOESI_state ms)
{
	LOG(name,"read",m,mt);
    access_++;
	read_++;
    uint32_t new_latency = timer_.latency(tstamp,mt,m);
    return MemState(new_latency,EXCLUSIVE); 
}

template <typename Timer>
MemState Main<Timer>::readx(const Access& m,uint64_t tstamp,Trace& mt,MOESI_state ms)
{
	LOG(name,"readx",m,mt);
	if (ms != NOT_FOUND) {
        access_++;
		return MemState(0,MODIFIED); 
	}
	mt.add(this,READ);
	return read(m,tstamp,mt,ms);
}

template <typename Timer>
MemState Main<Timer>::write(const Access& m,uint64_t tstamp,Trace& mt,MOESI_state ms)
{
	LOG(name,"write",m,mt);
    access_++;
	write_++;
    uint32_t new_latency = timer_.latency(tstamp,mt,m);
 	return MemState(new_latency, MODIFIED);
}

template <typename Timer>
MOESI_state Main<Timer>::state(const Access&,uint64_t)
{
	throw std::runtime_error("Main should not answer to state");
	return INVALID;
}

template <typename Timer>
void Main<Timer>::invalidate(const Access&,uint64_t tstamp,const Trace&,MOESI_state)
{
	throw std::runtime_error("Main should not answer to invalidate");
}

}

#endif
