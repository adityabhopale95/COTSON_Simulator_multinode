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

// $Id: abaeterno.cpp 725 2015-07-03 16:14:08Z paolofrb $


#include "abaeterno_config.h"
#include "abaeterno.h"
#include "callme.h"
#include "logger.h"
#include "interleaver.h"
#include "lua_export.h"
#include "error.h"
#include "network_timing.h"
#include "cotson.h"
#include "machine.h"
#include "callbacks.h"
#include "cpuid_call.h"
#include "fileutil.h"

using namespace std;
using namespace boost;
using namespace boost::assign;

namespace {

option  o01("help",                           "this help");
option  o02("version",                        "print version and die");

option  o04("log_options",           "false", "do we log to stdout all options requested");
option  o05("log_sim_time",          "false", "do we print the CPU time used by the process");

option  o10("print_unknown",         "false", "print unknown options");
option  o11("allow_unknown",          "true", "allow unknown options");

option  o12("debug_messages",            "0", "level of selection of messages");
options o13("debug_this",                     "print debug messages related to the arg");

option  o14("max_nanos",                 "0", "time (in ns) to simulate");
option  o31("max_samples",               "0", "number of samples to simulate");
option  o21("exit_trigger",               "", "filename to trigger simulation exit");

option  o17("deterministic",          "true", "should we try to be deterministic"); 

option  o30("init_wait",                "1M", "Wait for simnow initialization");
option  o35("fastforward",               "0", "length of fastforwarding phase (nanos)");

option  o36("time_feedback",          "true", "should we send time feedback to Simnow"); 
option  o37("min_cpi_feedback",       "0.01", "minimum CPI value (for time feedback)");
option  o38("max_cpi_feedback",        "100", "minimum CPI value (for time feedback)");

option  o39("cr3_tracing",           "false", "only trace user application with COTSONtracer"); 

optionR o15("sampler",                        "what event driver to use");
optionR o20("heartbeat",                      "what heartbeat driver to use");

option  o41("mp_sync_quantum",       "10000", "max unsynced ns of CPU execution (default)");
option  o42("mp_sync_quantum_sim",            "max unsynced ns of CPU execution (in simulation)");
option  o43("prefetch",                       "add load to prefetch operations");
option  o44("clflush",                        "add load to clflush operations");

option  o50("print_stats",                    "print statistics on stdout");
option  o51("network_cpuid",                  "send cpuid commands to all nodes in the cluster");
option  o52("custom_asm",                     "extend custom asm to prefetchnta");
option  o53("network_terminate",              "terminate execution on network terminate command");


}

namespace {

void abaeterno_start() { AbAeterno::get().start(); }
void abaeterno_stop() { AbAeterno::get().stop(); }
void abaeterno_prepare() { AbAeterno::get().prepareToRun(); }

run_at_start   f1(&abaeterno_start);
run_at_stop    f2(&abaeterno_stop);
run_at_prepare f3(&abaeterno_prepare);

}

AbAeterno::AbAeterno() :
    exitTrigger(  Option::get<string>  ("exit_trigger")),
    maxNanos(     Option::get<uint64_t>("max_nanos")),
    maxSamples(   Option::get<uint64_t>("max_samples")),
    log_sim_time( Option::get<bool>    ("log_sim_time")),
    net_terminate( Option::get<bool>   ("network_terminate",true)),
    code_tag(0),
    samples(0),
    next_sample(0),
    nanos(0),
    cycles(0),
    cyclesPerUsec(0),
    translated_insts(0),
    tag_prefetch(Option::get<bool>("prefetch",false)),
    print_stats(Option::get<bool>("print_stats",false)),
    net_cpuid(Option::get<bool>("network_cpuid",false)),
    custom_asm(Option::get<bool>("custom_asm",false))
{
    add("nSimulation",    stateCounter[SIMULATION]);
    add("nSimpleWarming", stateCounter[SIMPLE_WARMING]);
    add("nFullWarming",   stateCounter[FULL_WARMING]);
    add("nFunctional",    stateCounter[FUNCTIONAL]);
    add("nanos",nanos);
    add("cycles",cycles);
    add("cycles_per_usec",cyclesPerUsec);

    if(log_sim_time)
    {
        add("sim_clock_ticks",sim_clock_ticks);
        add("sim_user_time_abs",sim_user_time);
        add("sim_system_time_abs",sim_system_time);
        add("sim_user_time_dead_abs",sim_user_time_dead);
        add("sim_system_time_dead_abs",sim_system_time_dead);
        add_ratio("sim_user_time","sim_user_time_abs","sim_clock_ticks");
        add_ratio("sim_user_time_dead","sim_user_time_dead_abs","sim_clock_ticks");
        add_ratio("sim_system_time","sim_system_time_abs","sim_clock_ticks");
        add_ratio("sim_system_time_dead","sim_user_time_dead_abs","sim_clock_ticks");
    }

    NetworkTiming* nt=NetworkTiming::get();
    if(nt)
        add("cluster.",*nt);

    clear_metrics();

    HeartBeat::add(*this);

}

AbAeterno::~AbAeterno() {}

void AbAeterno::printStats()
{
    if (!print_stats)
        return;
    cout << "INSTS_CYCLES ";
    for(int i=0;i<Machine::cpus();i++)
    {
        const Cotson::Cpu::Stats& x = Cotson::Cpu::stats(i);
        cout << x.insts << " " << x.cycles << " " << x.idles << " ";
    }
    cout << endl;
}

void AbAeterno::start()
{
    LOG("Start");
    static bool first_time = true;
    if (first_time)
    {
        first_time = false;
        compute_metrics();
        startSample();
    }
}

void AbAeterno::stop()
{
    LOG("stop");
    Machine::get().empty_queues();
}

void AbAeterno::do_compute_metrics() {
    nanos=Cotson::nanos();
    cycles=Cotson::t2c(nanos);
    if(log_sim_time)
    {
        if(sim_clock_ticks==0)
            sim_clock_ticks=sysconf(_SC_CLK_TCK);
        struct tms cpu;
        times(&cpu);
        sim_user_time=cpu.tms_utime;
        sim_system_time=cpu.tms_stime;
        sim_user_time_dead=cpu.tms_cutime;
        sim_system_time_dead=cpu.tms_cstime;
    }
}

void AbAeterno::prepareToRun()
{
    LOG("prepareToRun");    
	Interleaver::get().initialize();
    Machine& machine=Machine::get();

    cyclesPerUsec=Cotson::t2c(1000);
    
    machine.build();
    add("",machine);

    set<string> required_sam;
    required_sam += "type";
    Parameters ps=Option::from_named_option("sampler",required_sam,"Sampler");
    
    ps.set("name", "sampler");
    ps.track();
    // sampler needs machine built
    sampler.reset(Factory<Sampler>::create(ps));
    sim_state=sampler->preState();

    if(Option::get<bool>("log_options"))
    {
        format fmt("%1%%|60t|%2%");
        const map<string,string>& v=Option::requested();
        cout << "Input values: " << endl << endl;
        for(map<string,string>::const_iterator i=v.begin();i!=v.end();i++)
            cout << fmt % i->first % i->second << endl;
        cout << "[missing values may be requested later!!]" << endl << endl;
    }
    cerr << "COTSON STARTED" << endl;
}

void AbAeterno::end()
{
    printStats();
    LOG("ending right now");
    HeartBeat::last_beat();
    Cotson::exit();
    if (print_stats)
        Profiler::get().dump();
    exit(0);
}

void AbAeterno::endQuantum()
{
    LOG("endQuantum");
    try
    {
        Machine::get().empty_queues();
        Interleaver::get().end_quantum();
    }
    catch(runtime_error& e)
    {
        Cotson::exit();
        ERROR("(endQuantum) caught an exception: ", Cotson::nanos(), " ", e.what());
    }
    catch(bad_alloc& e)
    {
        Cotson::exit();
        ERROR("(endQuantum) caught a bad alloc: ", Cotson::nanos(), " ", e.what());
    }
    catch(...)
    {
        Cotson::exit();
        ERROR("(endQuantum) caught an unknown exception: ", Cotson::nanos());
    }
    if (sim_state==SIMULATION)
        Cotson::Cpu::set_quantum_s();
    else
        Cotson::Cpu::set_quantum_f();
}

namespace {
    const char* SimStateNames[]={"FUNCTIONAL", "SIMPLE_WARMING",
        "FULL_WARMING", "SIMULATION", "END"};
}

void AbAeterno::callme(uint64_t duration)
{
    if(sim_state!=SIMULATION)
    {
        if(duration == 0)
            endSample();
        else
            CallMe::later(duration,bind(&AbAeterno::callme,this,0));
        return;
    }
    uint64_t now=Cotson::nanos();
    if(next_sample==0)
        next_sample=now+duration;

#if 0
cout << "called me at " << now
<< " next_sample: " << next_sample
<< " dur: " << duration
<< " state: " << SimStateNames[sim_state]
<< endl;
#endif

    if(next_sample == now)
    {
        next_sample=0;
        endSample();
        return;
    }

    CallMe::later(next_sample-now,bind(&AbAeterno::callme,this,0));
}

void AbAeterno::endSample()
{
    LOG("endSample");
    endQuantum();

    compute_metrics();

    uint64_t now=Cotson::nanos();
    (void)SimStateNames; // avoid warning
    LOG("END",SimStateNames[sim_state],now);
    if(sim_state==SIMULATION)
    {
        samples++;
        try {
            HeartBeat::beat();
        }
        catch(runtime_error& e)
        {
            Cotson::exit();
            ERROR("(heartbeat) caught an exception: ", Cotson::nanos(), " ", e.what());
        }

        cout << "TIME=" << now*1e-6 << " ms IPC ( ";
        for(int i=0;i<Machine::cpus();i++)
            cout << 1.0/Machine::cpu(i)->cpi() << " ";
        cout << ")\n";
        printStats();
    }
    check_end();
    startSample();
}

void AbAeterno::startSample()
{
    LOG("startSample");
    uint64_t now=Cotson::nanos();

    uint64_t duration;
    tie(sim_state,duration) = sampler->changeState(sim_state);
    if(sim_state==END)
    {
        cerr << "END requested by sampler" << now << endl;
        end();
    }

    duration = Cotson::c2t(duration);
    stateCounter[sim_state] += duration;
    LOG("START",SimStateNames[sim_state],duration,now);

    StateObserver::transition(sim_state);
    callme(duration);
}

void AbAeterno::break_sample()
{
    Interleaver::get().break_sample();
    Cotson::reload_options(); // new sampler may change options
    cout << "### TRACER CHANGE " << Cotson::nanos() << endl;
    endSample();
}

void AbAeterno::check_end()
{
    uint64_t now = Cotson::nanos();
    LOG("check_end");
    if (!exitTrigger.empty() && File::exists(exitTrigger))
    {
        cerr << "EXIT TRIGGER: " << exitTrigger << endl;
        end();
    }

    if(maxSamples && samples >= maxSamples)
    {
        cerr << "MAX SAMPLES: " << samples << endl;
        end();
    }

    if (maxNanos && now >= maxNanos)
    {
        cerr << "MAX NANOS: " << now << endl;
        end();
    }

    if (net_terminate && NetworkTiming::Terminated())
    {
        cerr << "NETWORK END: " << now << endl;
        end();
    }
}

bool AbAeterno::needs_sim_tag(const uint8_t* op, uint32_t n) const
{
    if (sim_state != SIMULATION)
        return false;
    if (tag_prefetch && n > 1 && Cotson::X86::is_prefetch(op))
        return true;
    if (n > 1 && Cotson::X86::is_clflush(op,op+2))
        return true;
    return false;
}

// Called for every instruction translated in a basic block
void AbAeterno::translate(uint64_t devid)
{
    if (translated_insts==0) 
    {
        translated_tags.clear();
        if (Profiler::get().on())  // tag beginning of each translated block
        {
            Cotson::Inject::tag(++code_tag,true);
            translated_tags.push_back(code_tag);
        }
    }
    translated_insts++;

    const Cotson::Inject::info_opcode& op = Cotson::Inject::translate_info();
    if (op.length > 1 && Cotson::X86::is_cpuid(op.opcode))
    {   // cpuid
        Cotson::Inject::tag(++code_tag,true);
    }
	else if (custom_asm && Cotson::X86::is_cotson_asm(op.opcode))
	{
		// prefetchnta (mapped to cpuid-style custom calls)
        Cotson::Inject::tag(++code_tag,false);
		asm_tags[devid][code_tag]=   (Cotson::X86::cotson_asm_imm(op.opcode)<<16)
		                           | (Cotson::X86::cotson_asm_opc(op.opcode)<<8)
								   | (Cotson::X86::cotson_asm_reg(op.opcode));
	}
    else if (Profiler::get().cr3() && op.length > 2
             && Cotson::X86::is_cr3mov(op.opcode,op.opcode+2)) 
    {   // cr3 change
        Cotson::Inject::tag(++code_tag,true); 
        translated_tags.push_back(code_tag);
    }
    else if (needs_sim_tag(op.opcode,op.length))
    {  // Save a list of the instructions we tag
        Cotson::Inject::tag(++code_tag,true);
        translated_tags.push_back(code_tag);
    }
}

// Called for every instruction after the basic block is translated
bool AbAeterno::inject_tag(uint64_t devid,uint32_t n,const uint8_t* op)
{
    if (translated_insts)
    {
        if (Profiler::get().on())
        {
            uint32_t tag = translated_tags.front();
            translated_tags.pop_front();
            // virtual if we use cr3 else physical
            uint64_t pc = Cotson::Inject::PC(!Profiler::get().cr3());
            Profiler::get().tag_pc(devid,tag,pc,translated_insts);
        }
        translated_insts=0;
    }
    if (Profiler::get().cr3() && n > 1 && Cotson::X86::is_cr3mov(op,op+2)) 
    {
        uint32_t tag = translated_tags.front();
        translated_tags.pop_front();
        Profiler::get().tag_cr3(devid,tag);
    }
    if (needs_sim_tag(op,n))
    {
        // Retrieve the info of the tagged instruction 
        uint32_t tag = translated_tags.front();
        translated_tags.pop_front();
        if (Cotson::X86::is_prefetch(op))
            Cotson::Inject::save_tag_info(devid,tag,Cotson::Inject::PREFETCH);
        if (Cotson::X86::is_clflush(op,op+2))
            Cotson::Inject::save_tag_info(devid,tag,Cotson::Inject::CLFLUSH);
        return true;
    }
    return false;
}

void AbAeterno::execute(uint64_t nanos,uint64_t devid, uint32_t tag)
{

    FunctionalState fs= (sim_state == FUNCTIONAL) ? ONLY_FUNCTIONAL : FUNCTIONAL_AND_TIMING;

	// Look for the special COTSON ASM ("prefetchnta"), which was previously tagged
	uint32_t atag = asm_tags[devid][tag];
	if (atag) 
	{
		// imm=byte[2], op=byte[1], reg=byte[0]
		uint8_t reg = atag & 0xff;
	    uint8_t op  = (atag>>8)  & 0xff;
		uint8_t imm = (atag>>16) & 0xff;
		int xop = op+COTSON_RESERVED_ASM_BASE;
        void* xdata = CpuidCall::functional(fs,nanos,devid,xop,reg,imm);

		// Save tag info for passing to the timing simulator
        Cotson::Inject::info_tag ti;
		ti.type=Cotson::Inject::ASM;
		ti.info.xasm.op=xop;
		ti.info.xasm.xdata = xdata;
		Machine::get().tag(devid,tag,ti); // register the tag
	}

	// Look for the special COTSON CPUID
    uint64_t RAX = Cotson::X86::RAX();  // CPUID function
    if (IS_COTSON_CPUID(RAX)) // Is this our own CPUID call?
	{
        uint64_t RDI = Cotson::X86::RDI(); 
        uint64_t RSI = Cotson::X86::RSI();
	    if (IS_COTSON_EXT_CPUID(RAX))
		{
            LOG("XCPUID: nanos",nanos,"devid",devid,"RAX",RAX);
            LOG("\tEXT(RAX)",COTSON_EXT_CPUID_OP(RAX),"RDI",RDI,"RSI",RSI);
            CpuidCall::functional(fs,nanos,devid,COTSON_EXT_CPUID_OP(RAX),RDI,RSI);
		}
	    else
	    {
            uint64_t RBX = Cotson::X86::RBX();
            LOG("CPUID: nanos",nanos,"devid",devid,"RAX",RAX);
            LOG("\tRDI",RDI,"RSI",RSI,"RBX",RBX);
            CpuidCall::functional(fs,nanos,devid,RDI,RSI,RBX);
            if (net_cpuid && NetworkTiming::get())
                NetworkTiming::get()->cpuid(RBX,RDI,RSI);
        }
    }

    // Call the tagged operation handler in parsing
    Machine::get().execute(devid,tag);
    // Call the tagged operation handler in profiling
    Profiler::get().execute(nanos,tag,devid);
}

void AbAeterno::network_cpuid(uint64_t a, uint16_t b, uint16_t c)
{
    cerr << "Got cpuid from network: " << a << " " << b << " " << c << endl;
    FunctionalState fs= (sim_state == FUNCTIONAL) ? ONLY_FUNCTIONAL : FUNCTIONAL_AND_TIMING;
    CpuidCall::functional(fs,Cotson::nanos(),0,a,b,c);
}

