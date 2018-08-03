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
// #define _DEBUG_THIS_

// $Id: interleaver.cpp 457 2013-01-16 15:45:32Z paolofrb $
#include "abaeterno_config.h"
#include "interleaver.h"
#include "error.h"
#include "logger.h"
#include "cpuid_call.h"
#include "cotson.h"

using namespace std;

namespace {
    option o1("align_timers", "align timers to max time every MP quantum");
    option o2("interleaver_order", "cycle", "order CPUs by either cycle | round_robin | uniform");
}

// ==========================================================================================
// Private internals to the interleaver module
// ==========================================================================================

Interleaver::CpuData::CpuData(Instructions* i,const TraceNeeds *t, uint32_t d, Order ob)
    : ins(i),tn(t),dev(d),order_by(ob) 
{
    update(0,0); 
}

inline void Interleaver::CpuData::update(uint64_t max_cycle, uint64_t max_ins)
{
    order = 0;
    order_rate = 1000;

    emit=tn->st[sim_state()].emit;

    pcycles=tn->cycles;
    if (max_cycle > *pcycles)
        tn->idle(max_cycle-*pcycles);
    initial_cycles=*pcycles;

    uint64_t exe_ins = ins->elems();
    if (max_ins > exe_ins)
        order_rate = static_cast<uint64_t>(1000.0 * static_cast<double>(max_ins) / exe_ins);
        
    LOG("CPU",dec,dev,"cycle",*pcycles,"exe_ins",exe_ins,"insrate",order_rate);
}

inline void Interleaver::CpuData::set_order()
{
    switch(order_by) 
    {
        case CYCLE:
            order = *pcycles - initial_cycles;
            break;
        case ROUNDROBIN:
            order += 1000;
            break;
        case UNIFORM:
            order += order_rate;
            break;
        case UNDEF:
            ERROR("Undefined interleaving order!");
			break;
    }
}

inline uint Interleaver::CpuData::emit_ins(uint64_t next_order)
{
    LOG("emit cpu", dev, "ins", ins->elems(),"order",order,"next",next_order);
    while(order <= next_order && ins->elems())
    {
        const Instruction* nn=ins->next();
        if (CpuidCall::simulation(const_cast<Instruction*>(nn),dev)!=DISCARD)
            emit(nn);
		set_order();
    }
	return ins->elems();
}

struct Interleaver::CpuCmp 
{
    inline bool operator()(const CpuData* p1, const CpuData* p2) 
    {
	    // if order is the same, disambiguate with device id
        uint64_t o1 = p1->order;
        uint64_t o2 = p2->order;
        uint32_t d1 = p1->dev;
        uint32_t d2 = p2->dev;
        return (o1 != o2) ? o1 > o2 : d1 > d2;
    }
};

// ==========================================================================================
// Interleaver interface
// ==========================================================================================

void Interleaver::initialize() 
{
    align_timers = Option::get<bool>("align_timers",true);
	order_by = CYCLE;
    if (Option::has("interleaver_order"))
    {
        const string &s=Option::get<string>("interleaver_order");
        if (s=="cycle")
            order_by=CYCLE;
        else if (s=="round_robin")
            order_by=ROUNDROBIN;
        else if (s=="uniform")
            order_by=UNIFORM;
        else
            throw runtime_error("Unkown interleaver_order option '" + s + "'\n" +
			   "allowed options: cycle | round_robin | uniform");
    }
    LOG("initialize",order_by);
}

void Interleaver::config(uint64_t dev,Instructions& insns,const TraceNeeds* tn)
{
    LOG("config device",dec,dev);
    ERROR_IF(order_by==UNDEF,"Interleaver config with undefined order!");
    ERROR_IF(dev!=cpus.size(),"Device '", dev, "' is not the device I was hoping to get");
    insns.create(100,tn->history+1);
    cpus.push_back(CpuData(&insns,tn,dev,order_by));
}

void Interleaver::update_cpus() 
{
    // At every mpquantum, CPUs must be re-aligned to the
    // max cycle of all CPUs, to account for idle time of CPUs that
    // may have executed fewer instructions
    uint64_t max_cycle = 0;
    uint64_t max_ins = 0;
    for(uint i=0;i<cpus.size();i++)
    {
        if (align_timers) 
        {
            uint64_t cycle = *(cpus[i].pcycles);
            max_cycle = cycle > max_cycle ? cycle : max_cycle;
        }
        uint64_t nins = cpus[i].ins->elems();
        max_ins = nins > max_ins ? nins : max_ins;
    }
    for(uint i=0;i<cpus.size();i++)  
        cpus[i].update(max_cycle,max_ins);
}

void Interleaver::end_quantum() 
{
    LOG("end_quantum");
    update_cpus();

	// Shortcut for 1 CPU
    if (cpus.size()==1)
    {
		cpus[0].emit_ins(-1);
	    CpuidCall::reset();
        return;
    }

	// Prepare a sorted list of CPUs by order value (eg, cycles)
    priority_queue<CpuData*,deque<CpuData*>,CpuCmp> cpu_queue;
    for(uint i=0;i<cpus.size();i++)
    {
        const Instructions* ins = cpus[i].ins;
        if(ins && ins->elems())
            cpu_queue.push(&cpus[i]);
    }

	// Interleave instruction emit to timers
    while (!cpu_queue.empty())
    {
        CpuData *fcpu = cpu_queue.top(); 
        cpu_queue.pop(); // remove top CPU from queue
        if (cpu_queue.empty())
            fcpu->emit_ins(-1); // Only 1 CPU left, emit all
		else 
		    if (fcpu->emit_ins(cpu_queue.top()->order)) // emit up to next
                cpu_queue.push(fcpu); // reinsert CPU if not done
    }
	CpuidCall::reset();
}

void Interleaver::break_sample() 
{
    for(uint i=0;i<cpus.size();i++)
        cpus[i].ins->clear();
}

