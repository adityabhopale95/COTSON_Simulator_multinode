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
#include "abaeterno_config.h"
#include "error.h"
#include "logger.h"
#include "cpu_timer.h"
#include "twolev.h"
#include "btb.h"
#include "ras.h"
#include "liboptions.h"

using namespace std;
using namespace boost;

class Timer1 : public CpuTimer 
{
public: 
	Timer1(Parameters&);

	void simple_warming(const Instruction*);
	void full_warming(const Instruction*);
	void simulation(const Instruction*);
	void idle(uint64_t);

	void addMemory(std::string,Memory::Interface::Shared); 

protected:
	void endSimulation();
	void beginSimulation();

private:
	const uint32_t dcache_ports;

	const uint32_t fetch_width;
	const uint32_t commit_width;
	const uint32_t max_ftq_size;
	const uint32_t max_window_size;
	const uint32_t pipe_stages;
	const uint32_t bpred_latency;
	const uint32_t history;
	
	/* PREDECODING_LOOKAHEAD defines the number of future BB's that will be
	   predecoded when a new BB is going to be fetched. A value of '1' will
	   cause that many fetch blocks got from the BTB are not decoded, and so
	   there will be a lot of mispredictions. *Empirically*, I have found
	   that 4 is a good number to achieve good predecoding accuracy (2 is
	   not enough, and 8 doesn't provide further benefits). 
	   -- Ayose */ 
	static const uint8_t PREDECODING_LOOKAHEAD = 4;

	struct Request
	{
		BB bb;
		uint32_t ras_index;
		char *counter;

		Request() :
			ras_index(0),
			counter(0)
			{}
	};	
	
	struct Inst
	{
	private:
		const Instruction* instruction;
	public:
		uint64_t addr;
		uint64_t next;
		InstType type;
		uint8_t size;
		char *counter;
		uint32_t ras_index;
		uint64_t commit;
		bool interrupt; // is it the last inst before interrupt?
		bool ready;
		uint64_t id;
		Instruction::MemIterator lditer;
		Instruction::MemIterator ldend;
		Instruction::MemIterator stiter;
		Instruction::MemIterator stend;

		inline void set(const Instruction*);
		inline const Instruction* inst();

		Inst() : 
			instruction(0),
			addr(0),
			next(0),
			type(),
			size(0),
			counter(0),
			ras_index(0),
			commit(0),
			interrupt(false),
			ready(false),
			id(0),
			lditer(0),
			ldend(0),
			stiter(0),
			stend(0)
			{}
	}; 

	void simulateInstruction(const Instruction*);

	void sim_cycle();
	void sim_predict();
	void sim_icache();
	void sim_fetch();
	void sim_dcache();
	void sim_commit();
	
	Request bpred_predict(uint64_t fetch_addr);

	Memory::Interface::Shared icache;
	Memory::Interface::Shared dcache;
	Memory::Interface::Shared itlb;
	Memory::Interface::Shared dtlb;

	boost::shared_ptr<twolevT>   twolev;
	boost::shared_ptr<btbT>      btb;
	boost::shared_ptr<rasT>      ras;
	
	std::deque<Inst> buffer;
	std::deque<Inst> window;
	std::deque<Request> ftq;

	// simulation state
	Inst prev;
	BB block;
	BB predecode;
	uint8_t bpred_delay;
	bool bpred_activity;
	uint32_t fetch_delay;
	uint32_t fetched_bytes;
	bool beginsNewBB;
	uint64_t cycles_internal;

	// statistics
	uint64_t cycles;
	uint64_t bpred_used;
	uint64_t ftq_full;
	uint64_t window_full;
	uint64_t fetch_insn;
	uint64_t commit_insn;
	uint64_t prefetch;
	uint64_t branches[InstType::_MAXCTRL];
	uint64_t branch_misses[InstType::_MAXCTRL];
};

registerClass<CpuTimer,Timer1> timer1_c("timer1");

Timer1::Timer1(Parameters& p) : CpuTimer(&cycles,&commit_insn),
	dcache_ports(p.get<uint32_t>("dcache_ports","2")),
	fetch_width(p.get<uint32_t>("fetch_width","4")),
	commit_width(p.get<uint32_t>("commit_width","4")),
	max_ftq_size(p.get<uint32_t>("max_ftq_size","1")),
	max_window_size(p.get<uint32_t>("max_window_size","128")),
	pipe_stages(p.get<uint32_t>("pipe_stages","5")),
	bpred_latency(p.get<uint32_t>("bpred_latency","0")),
	history(p.get<uint32_t>("history","65536",false)), // no track

	// simulation state
	bpred_delay(bpred_latency), bpred_activity(false), fetch_delay(0),
	fetched_bytes(0), beginsNewBB(true), cycles_internal(0)
{
	twolev.reset(new twolevT(
		p.get<uint32_t>("twolev.l1_size","1"),
		p.get<uint32_t>("twolev.l2_size","16kB"),
		p.get<uint32_t>("twolev.hlength","14"),
		p.get<uint32_t>("twolev.use_xor","1")));

	btb.reset(new btbT(
		p.get<uint32_t>("btb.entries","64kB"),
		p.get<uint32_t>("btb.sets","4")));

	ras.reset(new rasT(
		p.get<uint32_t>("ras.size","64")));

	add("bpred_used",bpred_used);
	add("call_count",branches[InstType::CTRL_CALL]);
	add("call_miss",branch_misses[InstType::CTRL_CALL]);
	add("commit_insn",commit_insn);
	add("cond_count",branches[InstType::CTRL_BRANCH]);
	add("cond_miss",branch_misses[InstType::CTRL_BRANCH]);
	add("cycles",cycles);
	add("fetch_insn",fetch_insn);
	add("ftq_full",ftq_full);
	add("instructions",commit_insn);
	add("ret_count",branches[InstType::CTRL_RET]);
	add("ret_miss",branch_misses[InstType::CTRL_RET]);
	add("uncond_count",branches[InstType::CTRL_JUMP]);
	add("uncond_miss",branch_misses[InstType::CTRL_JUMP]);
	add("non_branches",branches[InstType::DEFAULT]);
	add("non_branches_miss",branch_misses[InstType::DEFAULT]);
	add("window_full",window_full);
	add("prefetch_count",prefetch);

	add_ratio("call_miss_rate","call_miss","call_count");
	add_ratio("cond_miss_rate","cond_miss","cond_count");
	add_ratio("fetch_ipc","fetch_insn","cycles");
	add_ratio("ftq_full_rate","ftq_full","cycles");
	add_ratio("ipc","commit_insn","cycles");
	add_ratio("ret_miss_rate","ret_miss","ret_count");
	add_ratio("uncond_miss_rate","uncond_miss","uncond_count");
	add_ratio("window_full_rate","window_full","cycles");
	
	add("btb.",*btb);
	add("twolev.",*twolev);

	clear_metrics();

	trace_needs.history=history;
	trace_needs.st[SIMPLE_WARMING].set(EmitFunction::bind<Timer1,&Timer1::simple_warming>(this));
	trace_needs.st[FULL_WARMING].set(EmitFunction::bind<Timer1,&Timer1::full_warming>(this));
	trace_needs.st[SIMULATION].set(EmitFunction::bind<Timer1,&Timer1::simulation>(this));

	uint32_t flags = (NEED_CODE | NEED_MEM | NEED_EXC | NEED_HB);
	trace_needs.st[SIMPLE_WARMING].setflags(flags);
	trace_needs.st[FULL_WARMING].setflags(flags);
	trace_needs.st[SIMULATION].setflags(flags);
}

inline void Timer1::Inst::set(const Instruction* inst) 
{
    instruction=inst;
	id=inst->instruction_id();
}

inline const Instruction* Timer1::Inst::inst()
{
	if (instruction && instruction->instruction_id() != id)
	    ERROR("INST OVERWRITTEN. INCREASE timer.history");
    return instruction;
}

void Timer1::simple_warming(const Instruction* inst)
{
	if (!prev.inst())
	{
		prev.set(inst);
		return;
	}

	/* Just keep caches and branch predictor warmed-up */
	const Memory::Access& prev_pc = prev.inst()->PC();
	
	icache->read(prev_pc,cycles,this);
	itlb->read(prev_pc,cycles,this);
	
	Instruction::MemIterator il = prev.inst()->LoadsBegin();
	Instruction::MemIterator el = prev.inst()->LoadsEnd();
	for (; il!=el; il++)
	{
	    dcache->read(*il,cycles,this);
	    dtlb->read(*il,cycles,this);
	}
	Instruction::MemIterator is = prev.inst()->StoresBegin();
	Instruction::MemIterator es = prev.inst()->StoresEnd();
	for (; is!=es; is++)
	{
	    dcache->write(*is,cycles,this);
	    dtlb->read(*is,cycles,this);
	}

	// Branch predictor
	if (prev.inst()->is_branch())
	{
		bool taken = (inst->PC().phys != (prev_pc.phys + prev_pc.length));
		char *counter = twolev->Get2bitCounter (prev_pc.phys);
		twolev->SpecUpdate(prev_pc.phys, taken);
		twolev->Update(prev_pc.phys, taken, counter);
	}
	
	cycles+=1000;
	prev.set(inst);
}

void Timer1::beginSimulation()
{
	LOG("clear metrics");
	clear_metrics();
}

void Timer1::endSimulation()
{
	LOG("FLUSHING pipe at prev.inst:",prev.inst()?prev.inst()->PC():0);
	buffer.clear();
	window.clear();
	ftq.clear();

	prev=Inst();
	block=BB();
	predecode=BB();

	bpred_delay=bpred_latency;
	bpred_activity=false;
	fetch_delay=0;
	fetched_bytes=0;
	beginsNewBB=true; 
	cycles_internal=0;
}

void Timer1::full_warming(const Instruction* inst)
{
	simulateInstruction (inst);
}

void Timer1::simulation(const Instruction* inst)
{
	simulateInstruction(inst);
}

void Timer1::idle(uint64_t c)
{
    cycles += c;
    cycles_internal += c;
}

void Timer1::simulateInstruction(const Instruction* inst)
{
	if (prev.addr)
	{
	    Inst new_insn;
		ERROR_IF(!prev.inst(),"!prev_inst");
		new_insn.set(prev.inst());
		new_insn.addr = prev.addr;
		new_insn.type = prev.type;
		new_insn.size = prev.size;
		new_insn.next = inst->PC().phys;
		new_insn.interrupt = false;

		/* Interrupt detected -> end of BB in *previous* address */
		if (!prev.type.is_control()
			&& (prev.addr+prev.size) != inst->PC().phys)
		{
			LOG("--Interrupt detected-- ",hex,prev.addr,"+",static_cast<int>(prev.size)," != ",hex,inst->PC());
			predecode.end(prev.addr + prev.size,InstType::DEFAULT); 
			btb->Predecode(predecode);
			beginsNewBB = true;
			new_insn.interrupt = true;
			LOG("END OF BB: ",predecode,inst->PC());
		}
		
		buffer.push_back(new_insn);
	}

	if (!prev.next)
		prev.next = block.addr = prev.addr;
			
	// Predecode new BB
	if (beginsNewBB)
	{
		LOG("NEW BB begins at ",inst->PC(),"!");
		predecode.reset(inst->PC().phys);
		predecode.end(0,InstType::DEFAULT);
		beginsNewBB = false;
	}
	predecode.push_back(static_cast<uint8_t>(inst->PC().length));
	
	// Branch detected -> end of BB in *current* address
	if (inst->is_control())
	{
		const Memory::Access& pc=inst->PC();
		predecode.end(pc.phys+pc.length,inst->Type());
		btb->Predecode(predecode);
		beginsNewBB = true;
		LOG("END OF BB: ",predecode,pc);
	}
	
	// simulate pipeline 
	while (buffer.size() > PREDECODING_LOOKAHEAD*commit_width && beginsNewBB)
		sim_cycle();

	// next instruction 
	const Memory::Access& ipc=inst->PC();
	prev.addr = ipc.phys;
	prev.type = inst->Type();
	prev.size = ipc.length;
	prev.set(inst);
}

/* simulate pipeline end to start to allow bypassing */
void Timer1::sim_cycle()
{
	LOG("Cycle",cycles);

	sim_dcache();
	sim_commit();
	sim_icache();
	sim_fetch();
	sim_predict();

	cycles++;
		
	cycles_internal++;
	
	if(bpred_activity) 
	{
		bpred_used++;
		bpred_activity = false;
	}
}

Timer1::Request Timer1::bpred_predict(uint64_t fetch_addr)
{
	LOG("BPRED_PREDICT: ",hex,fetch_addr);
	
	BB btb_record;
	bool hit = btb->Lookup(fetch_addr, btb_record);

	TwolevUpdate twolev_record;
	twolev->Lookup(fetch_addr, twolev_record,false); //do not know yet

	Request request;
	request.bb.reset(fetch_addr);
	request.ras_index = ras->BackupIndex();
	request.counter = twolev_record.count;
// 	LOG("RAS_BACKUP_INDEX: ",request);

	if (hit) 
	{
		request.bb=btb_record;

		switch (btb_record.type) 
		{
			case InstType::CTRL_RET:
			case InstType::CTRL_IRET:
				request.bb.next = ras->Pop();
				break;
	
			case InstType::CTRL_CALL:
				ras->Push(fetch_addr + btb_record.bytes);
				break;
	
			case InstType::CTRL_BRANCH:
				twolev->SpecUpdate(fetch_addr, twolev_record.pred);
				if (!twolev_record.pred) 
					request.bb.next = fetch_addr + btb_record.bytes;
				break;
		    default:
			    break;
		}
		
// 		LOG("BTB_HIT: ",request);
	} 
	else 
	{
		LOG("BTB_MISS");
		request.bb.push_back(2); // assume 4 instructions of 2 bytes
		request.bb.push_back(2);
		request.bb.push_back(2);
		request.bb.push_back(2);
		request.bb.end(fetch_addr + 8,InstType::DEFAULT);
	}
	
	return request;
}

void Timer1::sim_predict()
{
	LOG("SIM_PREDICT");

	if (bpred_delay > 0) 
	{
		bpred_delay--;
		return;
	}

	if (ftq.size() >= max_ftq_size) 
	{
		ftq_full++;
		return;
	}

	Request request = bpred_predict(prev.next);
	
	// set delay for predictor latency
	bpred_delay = bpred_latency;
	bpred_activity=true;

	// feed back address for next cycle
	prev.next = request.bb.next;

	ftq.push_back(request);
}

void Timer1::sim_icache()
{
	LOG("SIM_ICACHE");

	if (fetch_delay > 0) 
	{
		fetch_delay--;
		return;
	}

	if (ftq.size() == 0) 
		return;

	Request& request = ftq[0];
	uint64_t addr = request.bb.addr;

	uint32_t icache_line_size=icache->item_size();
	// calculate number of useful bytes in line 
	fetched_bytes = (icache_line_size - 
			((uint32_t) addr & (icache_line_size - 1)));

	// iCache access
	Memory::Access m(addr);
	fetch_delay=icache->read(m,cycles,this)+itlb->read(m,cycles,this);
}

void Timer1::sim_fetch()
{
	LOG("SIM_FETCH");

	if (fetch_delay > 0) 
		return;

	if (ftq.size() == 0) 
		return; 

	Request& request = ftq[0];

	// skip if we can't fetch a full width 
	if (max_window_size - window.size() < fetch_width) 
	{
		window_full++;
		return;
	}

	for (uint32_t i = 0; i < fetch_width && request.bb.size() > 0 ; i++)
	{
		// if i==0 we consume instruction always
		// to advance and not enter an infinite loop
		if(i !=0 && fetched_bytes < request.bb.sizes[0])
			break;
			
		Inst new_insn;

		new_insn.addr = request.bb.addr;
		new_insn.size = request.bb.sizes[0];
		new_insn.ras_index = request.ras_index;
		new_insn.commit = cycles_internal + pipe_stages;
		new_insn.counter = request.counter;
		new_insn.ready = false;
		
		if(fetched_bytes >= new_insn.size)
			fetched_bytes -= new_insn.size;
		else
			fetched_bytes=0;
		
		if (request.bb.size() == 1) 
		{
			new_insn.next = request.bb.next;
			new_insn.type = request.bb.type;
		} 
		else 
		{
			new_insn.next = request.bb.addr + request.bb.sizes[0];
			new_insn.type = InstType::DEFAULT;
		}

		window.push_back(new_insn);

		fetch_insn++;

		request.bb.advance();
	}

	if (request.bb.size() == 0 && !ftq.empty()) 
		ftq.pop_front();
}

void Timer1::sim_dcache()
{
	LOG("SIM_DCACHE");
	uint32_t mems_done = 0;

	for  (uint32_t i=0; i < window.size() && 
						i < buffer.size() &&
						i < 2*commit_width && 
						mems_done < dcache_ports; i++)
	{
		Inst& binst=buffer[i];
		Inst& winst=window[i];
		
		if (winst.addr != binst.addr)
			break;

		if (winst.ready) 
			continue;

		ERROR_IF(!binst.inst(),"!buffer.insn[ib].inst()");
		
		bool is_prefetch = binst.type.is_prefetch();
		if (is_prefetch)
		    prefetch++;
		Instruction::MemIterator& ld = binst.lditer;
		Instruction::MemIterator& ldend = binst.ldend;
		if (ld == Instruction::MemIterator()) 
		{
		   ld = binst.inst()->LoadsBegin();
		   ldend = binst.inst()->LoadsEnd();
		}
		for (; ld != ldend && mems_done < dcache_ports; ld++)
		{
			uint64_t cache_lat = dcache->read(*ld,cycles,this);
			uint64_t tlb_lat = dtlb->read(*ld,cycles,this);
			winst.commit += tlb_lat + (is_prefetch ? 0 : cache_lat); 
			mems_done++;
 			LOG("MEM: LOAD ACCESS! ",binst.inst()->PC(),"; @:",*ld,")");
		}
		
		Instruction::MemIterator& st = binst.stiter;
		Instruction::MemIterator& stend = binst.stend;
		if (st == Instruction::MemIterator())
		{
		   st = binst.inst()->StoresBegin();
		   stend = binst.inst()->StoresEnd();
		}
		for (; st != stend && mems_done < dcache_ports; st++)
		{
			dcache->write(*st,cycles,this);
			winst.commit += static_cast<uint64_t>(dtlb->read(*st,cycles,this)); // read is intentional
			mems_done++;
 			LOG("MEM: STORE ACCESS! ",binst.inst()->PC(),"; @:",*st,")");
		}

		if (ld == ldend && st == stend)
			winst.ready = true;
	}
}

void Timer1::sim_commit()
{
	LOG("SIM_COMMIT");
	for (uint i = 0; i < commit_width && window.size() > 0; i++)
	{
		Inst& sim_insn = window[0];
		
		// commit only fully executed instructions
		if (!sim_insn.ready || sim_insn.commit > cycles_internal)
		    break;

		const Inst& real_insn = buffer[0];
		const InstType& rt = real_insn.type;

		ERROR_IF(real_insn.addr != sim_insn.addr,"");

		branches[rt.stat_index()]++;
		
		bool squash = false;
		if (sim_insn.next != real_insn.next) // detect mispredictions
		{
			squash = true;
			ftq.clear();

			prev.next = real_insn.next;	// pipeline redirect
			ras->RestoreIndex(sim_insn.ras_index); // RAS recovery

			// non-speculative RAS update
			if (rt.is_ret())
				ras->Pop();
			else if (rt.is_call())
				ras->Push(real_insn.addr + real_insn.size);

			twolev->ResetHistory();
			bpred_delay = (uint32_t) Timer1::bpred_latency;

			branch_misses[rt.stat_index()]++;
		}

		block.push_back(real_insn.size);
		
		if (rt.is_control())
		{
			bpred_activity=true;

			bool taken = (real_insn.next != real_insn.addr + real_insn.size);
			if (taken || !sim_insn.type.is_control())
			{
				block.end(real_insn.next,rt);
				btb->Update(block);
			}

			block.reset(real_insn.next);
			
			if (rt.is_branch())
				twolev->Update(real_insn.addr, taken, sim_insn.counter);
		}
		
		if (real_insn.interrupt) 
			block.reset(real_insn.next);
		
		if (!buffer.empty())
		    buffer.pop_front();

		if (squash) 
		{
			LOG("****SQUASH**** -> now ",hex,prev.next);
			window.clear();
		} 
		else 
		{
			LOG("****COMMIT****");
			if (!window.empty())
			    window.pop_front();
		}

		commit_insn++;
	}  
}

void Timer1::addMemory(string name,Memory::Interface::Shared c) 
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

