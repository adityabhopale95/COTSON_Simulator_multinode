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

// $Id: interleaver.h 725 2015-07-03 16:14:08Z paolofrb $
#ifndef INTERLEAVER_H
#define INTERLEAVER_H

#include "instructions.h"
#include "trace_needs.h"
#include "state_observer.h"

class Interleaver : public SimpleStateObserver
{
	Interleaver():align_timers(true),order_by(UNDEF) {}
	~Interleaver() {}

public:

	inline static Interleaver& get()
	{
		static Interleaver singleton;
		return singleton;
	}

	void config(uint64_t,Instructions&,const TraceNeeds*);
	void end_quantum();
	void break_sample();
	void initialize();

protected:
	enum Order { UNDEF=0, CYCLE, ROUNDROBIN, UNIFORM };
	void update_cpus();
	bool align_timers;
	Order order_by;
    struct CpuData
    {
        Instructions *ins;
        const TraceNeeds *tn;
        uint32_t dev;
	    Order order_by;
        EmitFunction emit;
        const uint64_t* pcycles;
        uint64_t initial_cycles;
        uint64_t order;
        uint64_t order_rate; 
        inline void update(uint64_t,uint64_t);
        inline void set_order();
        inline uint emit_ins(uint64_t);
        CpuData(Instructions*,const TraceNeeds *,uint32_t,Order);
    };
    struct CpuCmp;
    std::vector<CpuData> cpus;
};

#endif
