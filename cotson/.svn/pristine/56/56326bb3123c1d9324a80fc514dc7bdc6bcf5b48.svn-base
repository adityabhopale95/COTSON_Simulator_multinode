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
#include "cache.h"
#include "logger.h"
#include "error.h"
#include "cpu_timer.h"
#include "liboptions.h"

namespace BWdetails {

class Waiting
{
	// insts and line addresses
	typedef boost::tuple<const Instruction*,uint64_t,uint64_t> Inst;
	std::list<Inst> q;

	const int line_shift;

	uint lines_;
	bool more_in_line_;
public:
	Waiting(int);
	void add(const Instruction*);
	const Instruction* next();
	void clear();

	inline uint lines() { return lines_; }
	inline bool more_in_line() { return more_in_line_; }
};

	
class LiveInstruction
{
public:
	const Instruction* inst;
	bool executed;
	uint64_t finish_cycle;		
	Instruction::MemIterator lds;
	Instruction::MemIterator sts;

	LiveInstruction() :
		inst(0),
		executed(false),
		finish_cycle(0)
	{}

	LiveInstruction(const Instruction* i) :
		inst(i),
		executed(false),
		finish_cycle(0),
		lds(inst->LoadsBegin()),
		sts(inst->StoresBegin())
	{}
};

class ReorderBuffer
{
public:
	typedef std::list<LiveInstruction> Storage;
	typedef Storage::iterator          iterator;

private:
	const uint32_t max_size_;
	Storage insns;
	iterator non_executed_;

public: 
	ReorderBuffer(Parameters&);

	void add(const Instruction *);
	inline void remove_oldest() { insns.pop_front(); }

	inline bool empty() { return insns.empty(); }
	inline bool full() { return insns.size()==max_size_; }
	inline uint32_t size() { return insns.size(); } 
	void clear();

	inline iterator begin() { return insns.begin(); }
	inline iterator end() { return insns.end(); }
	iterator& non_executed();
};
}

class Bandwidth : public CpuTimer
{
public: 
	Bandwidth(Parameters&);

	void simple_warming(const Instruction*);
	void full_warming(const Instruction*);
	void simulation(const Instruction*);
	void idle(uint64_t);

	void addMemory(std::string,Memory::Interface::Shared); 
protected:
	void endSimulation();
	void beginSimulation();

private: 
	boost::shared_ptr<Memory::Interface> icache;
	boost::shared_ptr<Memory::Interface> dcache;
	boost::shared_ptr<Memory::Interface> itlb;
	boost::shared_ptr<Memory::Interface> dtlb;

	const uint32_t data_cache_ports;
	uint64_t instructions;
	uint64_t cycles;
	uint64_t icache_stall_cycles;
	std::map<uint,uint64_t> fetch_width;
	std::map<uint,uint64_t> commit_width;
	std::map<uint,uint64_t> ports_usage;
	std::map<uint,uint64_t> rb_usage;

    uint64_t total_data_latency;

	boost::scoped_ptr<BWdetails::Waiting> wi;
	BWdetails::ReorderBuffer rb;

	uint32_t icache_waiting;  // cycles waiting for icache
	const Instruction* next;

    // limits bandwidth of fetch/execute/commit stages
    uint32_t max_fetch_per_cycle_;
    uint32_t max_execute_per_cycle_;
    uint32_t max_commit_per_cycle_;

	void simulate_one_cycle();
	void fetch_stage();
	void memory_stage();
	void commit_stage();
};

using namespace std;
using namespace boost;
using namespace BWdetails;

registerClass<CpuTimer,Bandwidth> bandwidth_c("bandwidth");

Bandwidth::Bandwidth(Parameters& p) : CpuTimer(&cycles,&instructions),
	data_cache_ports(p.get<uint32_t>("data_cache_ports")),
	rb(p),
	icache_waiting(0),
	next(0),
    max_fetch_per_cycle_(p.get<uint32_t>("max_fetch_per_cycle","1")),
    max_execute_per_cycle_(p.get<uint32_t>("max_execute_per_cycle","1")),
    max_commit_per_cycle_(p.get<uint32_t>("max_commit_per_cycle","1"))
{
	add("instructions",instructions);
	add("cycles",cycles);
	add_ratio("ipc","instructions","cycles");	
	add("icache_stall_cycles",icache_stall_cycles);

    add("total_data_latency",total_data_latency);


	int rbs=p.get<int>("reorder_buffer_size");
	for(int i=0;i<=rbs;++i)
	{
        rb_usage[i] = 0;
		add("rb_usage_"+lexical_cast<string>(i),rb_usage[i]);
	}
	for(uint i=0;i<=data_cache_ports;++i)
	{
        ports_usage[i] = 0;
		add("ports_usage_"+lexical_cast<string>(i),ports_usage[i]);
	}
    for(uint i=0;i<=max_fetch_per_cycle_;++i)
	{
        fetch_width[i] = 0;
		add("fetch_width_"+lexical_cast<string>(i),fetch_width[i]);
	}
	for(uint i=0;i<=max_commit_per_cycle_;++i)
	{
        commit_width[i] = 0;
		add("commit_width_"+lexical_cast<string>(i),commit_width[i]);
	}
	clear_metrics();

	trace_needs.history=rbs*2; // account for waiting !!
	trace_needs.st[SIMPLE_WARMING].set(EmitFunction::bind<Bandwidth,&Bandwidth::simple_warming>(this));
	trace_needs.st[FULL_WARMING].set(EmitFunction::bind<Bandwidth,&Bandwidth::full_warming>(this));
	trace_needs.st[SIMULATION].set(EmitFunction::bind<Bandwidth,&Bandwidth::simulation>(this));

	uint32_t flags=(NEED_CODE|NEED_MEM|NEED_EXC|NEED_HB);
	trace_needs.st[SIMPLE_WARMING].setflags(flags);
	trace_needs.st[FULL_WARMING].setflags(flags);
	trace_needs.st[SIMULATION].setflags(flags);

}

void Bandwidth::beginSimulation()
{
	LOG("clear metrics");
	clear_metrics();
}

void Bandwidth::endSimulation()
{
	LOG("clearing timer");
	wi->clear();
	rb.clear();
	icache_waiting=0;
	next=0;
}

void Bandwidth::idle(uint64_t c)
{
    cycles += c;
}

void Bandwidth::simulation(const Instruction* inst)
{
	wi->add(inst);
	for(;wi->lines()>1;)
		simulate_one_cycle();
}

void Bandwidth::simple_warming(const Instruction* inst) 
{
	simulation(inst); 
}

void Bandwidth::full_warming(const Instruction* inst) 
{ 
	simulation(inst); 
}

void Bandwidth::fetch_stage()
{
	// there are three posibilities here:
	//   1. waiting for a line to load (icache_waiting!=0 and next!=0)
	//   2. we have a line loaded (icache_waiting==0 && next!=0)
	//   3. we have to load a line (icache_waiting==0 && next==0)
    uint32_t remaining_instrs = max_fetch_per_cycle_;

	if(icache_waiting)
	{
		icache_waiting--;
		icache_stall_cycles++;
		fetch_width[0]++;
		return;
	}

	uint fetched_this_cycle=0;

    while (!rb.full() && remaining_instrs > 0)
    {
        remaining_instrs--;

		if(next==0)
        {
            next=wi->next();

			const Memory::Access pc=next->PC();
			icache_waiting=icache->read(pc,cycles,this)+
				itlb->read(pc,cycles,this);
			if(icache_waiting!=0)
			{
				LOG("fetch__",cycles,icache_waiting);
				icache_waiting--;
				icache_stall_cycles++;
				break;
			}
		}

		rb.add(next);
		fetched_this_cycle++;
		LOG("fetched",cycles,hex,next->PC().phys);		
		next=0;
	
		if(!wi->more_in_line()) // next is another line, must load it next cycle
			break;
		else
			next=wi->next();
	}
	fetch_width[fetched_this_cycle]++;
}

void Bandwidth::memory_stage()
{
	uint64_t ports=data_cache_ports;
    uint32_t remaining_instrs = max_execute_per_cycle_;

	ReorderBuffer::iterator& non=rb.non_executed();
	ReorderBuffer::iterator end=rb.end();

	while(ports>0 && non!=end && remaining_instrs>0)
	{
		const Instruction* inst=non->inst;
		Instruction::MemIterator& lds=non->lds;
		Instruction::MemIterator& sts=non->sts;

		Instruction::MemIterator e = inst->LoadsEnd();
		for(;ports>0 &&  lds!=e; ++lds, ports--)
		{
			LOG("load",cycles,ports,hex,inst->PC().phys,lds->phys);
			uint64_t latency = dcache->read(*lds,cycles,this)+
				dtlb->read(*lds,cycles,this);
            total_data_latency+=latency;
			if((latency+cycles) > non->finish_cycle)
				non->finish_cycle=latency+cycles;
			LOG("finish_latency",cycles,latency,non->finish_cycle);
		}
		
		e = inst->StoresEnd();
		for(;ports>0 &&  sts!=e; ++sts,ports--)
		{
			LOG("store",cycles,ports,hex,inst->PC().phys,sts->phys);
			dtlb->read(*sts,cycles,this);
			dcache->write(*sts,cycles,this);
			//notice how the latency returned by either the tlb fetch or
			//the store are not taken into account. This results in considering
			//that the store enters into a write buffer that holds it until 
			//completion, although it is already sent to the cache. This could
			//be augmented by using the return latency of the tlb access to
			//augment the cycles timestamp of the store, but the way our caches
			//work, this would mean that the present time is that timestamp
			//A better solution, of course, would be to hold off until the tlb
			//is fetched and then issue the store, but this falls out of this
			//model
		}

		if(lds==inst->LoadsEnd() && sts==inst->StoresEnd())
		{
			non->executed=true;
			++non;
            remaining_instrs--;
		}
		else
			break;
	}
	ports_usage[data_cache_ports-ports]++;
}

void Bandwidth::commit_stage()
{
    uint32_t remaining_instrs = max_commit_per_cycle_;

	LOG("commit",cycles);
	uint ci=0;
	for(;!rb.empty() && remaining_instrs > 0;ci++)
	{
		ReorderBuffer::iterator beg=rb.begin();
		if(!beg->executed || beg->finish_cycle>cycles)
			break;
		LOG("cc",hex,beg->inst->PC().phys);
		rb.remove_oldest();
		instructions++;
	}
	commit_width[ci]++;
}

void Bandwidth::simulate_one_cycle()
{
	rb_usage[rb.size()]++;

	commit_stage();	
	memory_stage();
	fetch_stage();
	cycles++;
}

void Bandwidth::addMemory(string name,Memory::Interface::Shared c) 
{
	if(name=="icache")
	{
		icache=c; 
		add("icache.",*icache); 
		wi.reset(new Waiting(Memory::ilog2(icache->item_size())));
		/*for(uint i=0;i<=icache->item_size();++i)
		{
			fetch_width[i]=0;
			add("fetch_width_"+lexical_cast<string>(i),fetch_width[i]);
		}*/
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

namespace BWdetails {

Waiting::Waiting(int ls) : line_shift(ls),lines_(0),more_in_line_(false) {}

void Waiting::add(const Instruction* inst)
{
	uint64_t addr=inst->PC().phys;
	uint64_t line=addr>>line_shift;
	if(q.empty())
		lines_=1;
	else if((q.back().get<1>()!=line) || (q.back().get<2>()>addr))
		lines_++;
	q.push_back(boost::make_tuple(inst,line,addr));
	LOG("new",hex,inst->PC().phys,line,dec,lines_);//,q.size());
}

const Instruction* Waiting::next()
{
	ERROR_IF(q.empty(),"no more instructions");
	
	uint64_t line,addr;
	const Instruction* n;
	tie(n,line,addr)=q.front();
	q.pop_front();
	more_in_line_=false;
	if(q.empty())
		lines_=0;
	else if((q.front().get<1>()!=line) || (q.front().get<2>()<addr))
		lines_--;
	else
		more_in_line_=true;
	return n;
}

void Waiting::clear() 
{ 
	LOG("waiting clear");
	q.clear(); 
	lines_=0;
	more_in_line_=false;
}

ReorderBuffer::ReorderBuffer(Parameters&p) : 
	max_size_(p.get<uint32_t>("reorder_buffer_size")),
	non_executed_(insns.end())
{}

void ReorderBuffer::clear()
{ 
	LOG("reorder clear");
	insns.clear();
	non_executed_=insns.end();
}

void ReorderBuffer::add(const Instruction *inst)
{
	if(non_executed_!=insns.end())
 		insns.push_back(LiveInstruction(inst));
	else
	{
		non_executed_=insns.insert(insns.end(),LiveInstruction(inst));
	}
}

ReorderBuffer::iterator& ReorderBuffer::non_executed()
{
	if(insns.empty())
		return non_executed_;//=insns.end();
	
	if(non_executed_!=insns.end())
		return non_executed_;

	for(non_executed_=insns.begin();
		non_executed_!=insns.end() && non_executed_->executed;
		++non_executed_) {}
	return non_executed_;
}
}
