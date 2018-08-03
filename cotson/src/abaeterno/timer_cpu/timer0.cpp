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

// $Id: timer0.cpp 227 2011-09-19 16:11:32Z paolofrb $
#include "abaeterno_config.h"
#include "logger.h"
#include "cpu_timer.h"
#include "twolev.h"
#include "liboptions.h"

using namespace std;
using namespace boost;

class Timer0 : public CpuTimer
{
public: 
	Timer0(Parameters&);

	void simple_warming(const Instruction*);
	void full_warming(const Instruction*);
	void simulation(const Instruction*);
	void idle(uint64_t);

	void addMemory(std::string,Memory::Interface::Shared); 

protected:
	void endSimulation();
	void beginSimulation();

private: 
	const Instruction* prev_inst;
	Memory::Interface::Shared icache;
	Memory::Interface::Shared dcache;
	Memory::Interface::Shared itlb;
	Memory::Interface::Shared dtlb;
	boost::shared_ptr<twolevT>	twolev;

	uint64_t instructions;
	uint64_t cycles; // rounded up version of dcycles
	double dcycles; // keep cycles accounting at sub-cycle precision
	
	const uint32_t branch_mispred_penalty;
	const bool bpred_perfect;
	const double commit_cpi;
	const double dcache_fudge;
	const double icache_fudge;
};

registerClass<CpuTimer,Timer0> timer0_c("timer0");

Timer0::Timer0(Parameters& p) : CpuTimer(&cycles,&instructions),
	prev_inst(0),
	branch_mispred_penalty(p.get<uint32_t>("branch_mispred_penalty","8")),
	bpred_perfect(p.get<bool>("bpred_perfect","false")),
	commit_cpi(p.get<double>("commit_cpi","1.0")),
	dcache_fudge(p.get<double>("dcache.fudge","1.0")),
	icache_fudge(p.get<double>("icache.fudge","1.0"))
{
	twolev.reset(new twolevT(
		p.get<uint32_t>("twolev.l1_size","1"),
		p.get<uint32_t>("twolev.l2_size","16kB"),
		p.get<uint32_t>("twolev.hlength","14"),
		p.get<uint32_t>("twolev.use_xor","1")));

	add("instructions",instructions);
	add("cycles",cycles);
	add_ratio("ipc","instructions","cycles");	

	add("twolev.", *twolev);
	clear_metrics();
	dcycles = 0.0;

	trace_needs.history=2;
	trace_needs.st[SIMPLE_WARMING].set(EmitFunction::bind<Timer0,&Timer0::simple_warming>(this));
	trace_needs.st[FULL_WARMING].set(EmitFunction::bind<Timer0,&Timer0::full_warming>(this));
	trace_needs.st[SIMULATION].set(EmitFunction::bind<Timer0,&Timer0::simulation>(this));

	uint32_t flags = (NEED_CODE | NEED_MEM | NEED_EXC | NEED_HB);
	trace_needs.st[SIMPLE_WARMING].setflags(flags);
	trace_needs.st[FULL_WARMING].setflags(flags);
	trace_needs.st[SIMULATION].setflags(flags);
}

void Timer0::simulation(const Instruction* inst)
{
	instructions++;
	dcycles+=commit_cpi;

	const Memory::Access& pc = inst->PC();
	
	/* Branch predictor */
	if (!bpred_perfect && prev_inst && prev_inst->is_branch())
	{
		const Memory::Access& lastPC = prev_inst->PC();
		uint64_t lpc = lastPC.phys;
		bool real_taken = ((lpc+lastPC.length) != pc.phys);

		TwolevUpdate twolev_record;
		if (twolev->Lookup(lpc, twolev_record,real_taken))
			dcycles += (double)branch_mispred_penalty;
		
		// Update branch predictor
		twolev->SpecUpdate(lpc, real_taken);
		twolev->Update(lpc, real_taken, twolev_record.count);
	}

	if(icache)
	{
		LOG(id(),dec,cycles," -- INSTRUCTION:", hex, pc);
		uint64_t latency = icache->read(pc,(uint64_t)dcycles,this)+
			itlb->read(pc,(uint64_t)dcycles,this);
		if (latency) 
		{
		    LOG(id()," icache latency =",dec,latency);
			dcycles += icache_fudge * latency;
	    }
	}

	if(dcache)
	{
		Instruction::MemIterator il = inst->LoadsBegin();
		const Instruction::MemIterator el = inst->LoadsEnd();
		uint64_t max_load_latency=0;
		bool is_prefetch=inst->is_prefetch();
		for(; il != el; ++il)
		{
			LOG(id()," -- LOAD:", *il);
			uint64_t cache_lat = dcache->read(*il,(uint64_t)dcycles,this);
			uint64_t tlb_lat = dtlb->read(*il,(uint64_t)dcycles,this);
			uint64_t latency = tlb_lat + (is_prefetch ? 0 : cache_lat);
			if (latency > max_load_latency)
			{
			    LOG(id()," load latency =",dec,latency);
				max_load_latency=latency;
		    }
		}
		if(max_load_latency)
			dcycles += dcache_fudge * max_load_latency;

		uint64_t max_store_latency=0;
		Instruction::MemIterator is = inst->StoresBegin();
		const Instruction::MemIterator es = inst->StoresEnd();
		for(; is != es; ++is)
		{
			LOG(id()," -- STORE:", *is);
			dcache->write(*is,(uint64_t)dcycles,this);
			uint64_t latency = dtlb->read(*is,(uint64_t)dcycles,this);
			if (latency > max_store_latency)
			{
			    LOG(id()," store latency =",dec,latency);
				max_store_latency=latency;
		    }
		}
		if(max_store_latency)
			dcycles += dcache_fudge * max_store_latency;
	}
	
	prev_inst = inst;
	cycles = (uint64_t)dcycles;
}

void Timer0::idle(uint64_t c)
{
    dcycles += (double)c;
	cycles = (uint64_t)dcycles;
}

void Timer0::full_warming(const Instruction* inst)
{
	if(!prev_inst)
	{
		prev_inst = inst;
		return;
	}

	const Memory::Access& prev_pc = prev_inst->PC();
	
	if(icache)
	{
		icache->read(prev_pc,cycles,this);
		itlb->read(prev_pc,cycles,this);
	}

	if(dcache)
	{
		Instruction::MemIterator il = prev_inst->LoadsBegin();
		Instruction::MemIterator el = prev_inst->LoadsEnd();
		for (; il!=el; il++)
		{
			dcache->read(*il,cycles,this);
			dtlb->read(*il,cycles,this);
		}

		Instruction::MemIterator is = prev_inst->StoresBegin();
		Instruction::MemIterator es = prev_inst->StoresEnd();
		for (; is!=es; is++)
		{
			dcache->write(*is,cycles,this);
			dtlb->read(*is,cycles,this);
		}
	}
	
	/* Branch predictor */		
	if (!bpred_perfect && prev_inst->is_branch())
	{
		/* set branch direction */
		uint64_t ppc = prev_pc.phys;
		bool taken = inst->PC().phys != (ppc + prev_pc.length);
		char *counter = twolev->Get2bitCounter (prev_pc.phys);
		twolev->SpecUpdate(ppc, taken);
		twolev->Update(ppc, taken, counter);
	}
	
	cycles+=1000;
	prev_inst = inst;
}

void Timer0::beginSimulation()
{
	LOG(id(),"clear metrics");
	clear_metrics();
	dcycles = 0.0;
}

void Timer0::endSimulation()
{
	LOG(id(),"clearing timer");
	prev_inst = 0;
	dcycles = 0.0;
}

void Timer0::simple_warming(const Instruction* inst)
{
	full_warming(inst);
}

void Timer0::addMemory(string name,Memory::Interface::Shared c) 
{
	if(name=="icache")
	{
		icache=c; 
		add("icache.",*icache); 
	}
	else if(name=="dcache")
	{
		dcache=c; 
		add("dcache.",*dcache); 
	}
	else if(name=="itlb")
	{
		itlb=c; 
		add("itlb.",*itlb); 
	}
	else if(name=="dtlb")
	{
		dtlb=c; 
		add("dtlb.",*dtlb); 
	}
	else 
		throw runtime_error("unknown memory interface "+name);
}
