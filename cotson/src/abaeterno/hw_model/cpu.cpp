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

// $Id: cpu.cpp 725 2015-07-03 16:14:08Z paolofrb $

#include "abaeterno_config.h"

#include "cpu.h"
#include "logger.h"
#include "error.h"
#include "cpu_token_parser.h"
#include "predictor.h"

using namespace std;
using namespace boost;
using namespace boost::assign;

Cpu::Cpu(TokenQueue*q,uint64_t d,const string& n) : 
	devid(d),devname(n+lexical_cast<string>(d)),
	hb(16,0),
	init(true)
{
	Cotson::Cpu::init(devid);
	parser.reset(new CpuTokenParser(q,hb,devid));
	predictor.reset(new Predictor(devid));

	add("hb_CR3_equal",hb[0]);
	add("hb_CR3_different",hb[1]);
	add("hb_ATC_flush",hb[2]);
	add("hb_ev_Exception",hb[3]);
	add("hb_ev_SW_interrupt",hb[7]);
	add("hb_ev_HW_interrupt",hb[15]);

	add("instcount",instructions);
	add("idlecount",idlecount);
	add("cycles",cycles);

	set<string> stats_names(Cotson::statistics_names());
	for(set<string>::iterator i=stats_names.begin();i!=stats_names.end();++i)
	{
		stats[*i]=difference();
		add(*i,stats[*i]);
	}

	clear_metrics();
	cpi_=1.0;
}

Cpu::~Cpu() { }

void Cpu::timer(luabind::object table)
{
	ERROR_IF(timing.get(),"already defined a timer for this cpu");
	set<string> required;
	required += "name";
	
	Parameters p=Option::from_lua_table(table,required,"CpuTimer");
	p.track();

	CpuTimer* cp;
	timing.reset(cp=Factory<CpuTimer>::create(p));
	timing->id(devid);
	TaggedMetrics tm=TaggedMetrics::get();
	tm.add(cp,p.get<string>("name"));
	tm.add(cp,"cpu");
	if(p.has("tag"))
		tm.add(cp,p.get<string>("tag"));

	add("timer.",*timing);
	HeartBeat::add_aggregable(*timing.get());

	parser->provide(timing->needs());
}

void Cpu::instruction_cache(Memory::lua& m) 
{ 
	ERROR_IF(!timing.get(),"timer not defined for this cpu");
	TaggedMetrics::get().add(m.pointer.get(),"icache");
	timing->addMemory("icache",m.pointer); 
}

void Cpu::data_cache(Memory::lua& m) 
{ 
	ERROR_IF(!timing.get(),"timer not defined for this cpu");
	TaggedMetrics::get().add(m.pointer.get(),"dcache");
	timing->addMemory("dcache",m.pointer); 
}

void Cpu::instruction_tlb(Memory::lua& m) 
{
	ERROR_IF(!timing.get(),"timer not defined for this cpu");
	TaggedMetrics::get().add(m.pointer.get(),"itlb");
	timing->addMemory("itlb",m.pointer); 
}

void Cpu::data_tlb(Memory::lua& m) 
{
	ERROR_IF(!timing.get(),"timer not defined for this cpu");
	TaggedMetrics::get().add(m.pointer.get(),"dtlb");
	timing->addMemory("dtlb",m.pointer); 
}

void Cpu::do_compute_metrics()
{
	uint64_t _idlecount=Cotson::Cpu::idlecount(devid);
	uint64_t _cycles=Cotson::Cpu::cycles(devid);
	uint64_t _instructions=Cotson::Cpu::instructions(devid);

	if(init)
	{
		init=false;
		idlecount.reset(_idlecount);
		cycles.reset(_cycles);
		instructions.reset(_instructions);
		for(map<string,difference>::iterator i=stats.begin();i!=stats.end();++i)
			i->second.reset(Cotson::Cpu::statistic(devid,i->first));
	}
	idlecount.set(_idlecount);
	cycles.set(_cycles);
	instructions.set(_instructions);
	for(map<string,difference>::iterator i=stats.begin();i!=stats.end();++i)
		i->second.set(Cotson::Cpu::statistic(devid,i->first));

	if(sim_state()==SIMULATION)
	{
		set_sample();
		ERROR_IF(!timing.get(),"timer not defined for this cpu");
		uint64_t t_instrs = (*timing)["instructions"];
		cpi_=1.0;
		if (t_instrs!=0)
		{
		    uint64_t t_cycles = (*timing)["cycles"];
		    cpi_=static_cast<double>(t_cycles)/t_instrs;
	        LOG("cycles", t_cycles, "instrs", t_instrs, "simulated CPI",cpi_);
	        if (sample_idle) // adjust for idle fraction
	        {
	            double id_frac = static_cast<double>(sample_idle) /
				                 (sample_instructions + sample_idle);
	            cpi_ = cpi_ * (1.0 - id_frac) + id_frac;
	            LOG("idle adjusted CPI",cpi_);
	        }
		}
	}
}

void Cpu::setIPC()
{
	// predict next CPI at the end of a SIMULATION sample
	if(sim_state()==SIMULATION) 
	{
	    cpi_ = predictor->predict(cpi_);
	    LOG("predicted CPI",cpi_);
	    uint64_t insts = 1000;
	    uint64_t cycles = static_cast<uint64_t>(cpi_*insts);

	    LOG("setIPC (cpu", devid, "):", insts, "/", cycles);
	    ERROR_IF(insts==0 || cycles==0,"Illegal IPC values");
	    Cotson::Cpu::set_ipc(devid,insts,cycles);
	}
}

// void Cpu::cpi(double x) { cpi_=x; }
double Cpu::cpi() const { return cpi_; }

void Cpu::beginFunctional()    { setIPC(); reset_sample(); }
void Cpu::beginSimpleWarming() { setIPC(); reset_sample(); }
void Cpu::beginFullWarming()   { setIPC(); reset_sample(); }
void Cpu::beginSimulation()    { setIPC(); reset_sample(); }

void Cpu::set_sample()
{
	sample_instructions.set(instructions);
	sample_cycles.set(cycles);
	sample_idle.set(idlecount);
}

void Cpu::reset_sample()
{
	sample_instructions.reset(instructions);
	sample_cycles.reset(cycles);
	sample_idle.reset(idlecount);
}

void Cpu::inject(InjectState state) 
{ 
    parser->inject(state); 
}

void Cpu::tag(uint32_t t,const Cotson::Inject::info_tag& ti) 
{ 
    parser->tag(t,ti); 
}

bool Cpu::execute(uint32_t t) 
{ 
    return parser->execute(t); 
}

