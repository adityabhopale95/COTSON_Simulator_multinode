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
#include "logger.h"
#include "error.h"
#include "lua_helpers.h" 
#include "cpu_timer.h"
#include "instructions.h"
#include "opcodes.h"
#include "cotson.h"
#include "selector.h"
#include "fileutil.h"

#include "mem_bintracer.h"
#include "memgate.h"
 
#ifdef USE_MEMGATE
use_static_memgate usm;
#endif

#include <lua.hpp>
#include <time.h>

using namespace std;
using namespace boost;
using namespace boost::assign;

namespace {
option  o01("help",                             "this help");
option  o02("injection_type",            "cpu", "type of trace to be injected");
option  o03("debug_messages",            "0",   "level of selection of messages");
options o04("debug_this",                       "print debug messages related to the arg");
option  o05("instructions",              "0",   "how much to simulate");
option  o07("memory_accesses",           "0",   "how many memory accesses to simulate (memory injection_type only)");
optionR o20("heartbeat",                        "what heartbeat driver to use");
}

Opcodes opcodes;

ReadGzip& operator>>(ReadGzip& gz,Instruction& inst)
{
    Memory::Access pc(0,0,0);
    gz >> pc;
    if(!gz)
        return gz;
    ERROR_IF(!pc.length,"instructions must have length to be read");

    char type;
    gz >> type;
    if(!gz)
        return gz;

    uint8_t *opc = opcodes.malloc(pc.length);
    for(uint i=0;i<pc.length;i++)
        gz >> opc[i];
    if(!gz)
        return gz;

    const Opcode* newop = 
        opcodes.insert(
            pc.phys, 
            Opcode(opc,pc.length,type,false,
                   Opcode::regs(),
                   Opcode::regs(),
                   Opcode::regs()));
    Instruction::init(&inst,pc.virt,pc.phys,pc.length,0,newop,type);

    Memory::Access e(0,0,0);
    uint32_t size;

    gz >> size; 
    for(uint32_t i=0;i<size;i++)
    {
        gz >> e;
        inst.LoadsPush(e);
    }
    
    gz >> size; 
    for(uint32_t i=0;i<size;i++)
    {
        gz >> e;
        inst.StoresPush(e);
    }

    return gz;
}

class Cpu : public metric
{
    public:
    Cpu(uint,string);
    void timer(luabind::object table);
    void instruction_cache(Memory::lua& m) { timing->addMemory("icache",m.pointer); } 
    void data_cache(Memory::lua& m) { timing->addMemory("dcache",m.pointer); } 
    void instruction_tlb(Memory::lua& m) { timing->addMemory("itlb",m.pointer); } 
    void data_tlb(Memory::lua& m) { timing->addMemory("dtlb",m.pointer); } 

    CpuTimer * get_timer() { return timing.get(); }
    
    void emit();

    private:
    const uint index;
    ReadGzip gz;
    scoped_ptr<CpuTimer> timing;
    Instructions ipul;
};

Cpu::Cpu(uint i,string tracefile) : 
    index(i),
    gz(tracefile),
    timing(0)
{
    if(!gz)
        throw invalid_argument("Could not open tracefile " +tracefile);
}

void Cpu::timer(luabind::object table) 
{
    set<string> required;
    required += "name";

    Parameters p=Option::from_lua_table(table,required,"CpuTimer");
    p.track();

    timing.reset(Factory<CpuTimer>::create(p));
    timing->id(index); 
    ipul.create(1,timing->needs()->history+1);

    add("cpu"+lexical_cast<string>(index)+".",*timing);
}

void Cpu::emit()
{
    Instruction *inst=ipul.alloc();
    gz >> *inst;
    if(!gz)
        throw runtime_error("end");
    LOG("fetch",hex,inst->PC().phys,dec);
    timing->simulation(ipul.next());
}

#define NULL_EVENT 0xffffffffffffffff

class InjectMemory
{
    public:
    InjectMemory(Memory::lua& m, string tracefile, Cpu *cpu) :
        Mem(m.pointer), gz(tracefile),
        FakeTrace(cpu->get_timer())
    {
        read_=write_=0;
        if (gz == 0)
            throw runtime_error("Could not open trace file "+ tracefile); 
        
        trace = new Memory::TraceMem(true, 0, 0, 0, 0);
        read_next();
        
        first_nanos=trace->getNanos();
        first_tmstamp = trace->getTstamp();
    }
        
    ~InjectMemory()
    {
        cout << "END OF TRACE: Reads: "<< read_ << " writes: "<< write_ << endl;
        cout << "              Nanos from "<< first_nanos << " to "<< last_nanos << endl;
        cout << "              Cycles from "<< first_tmstamp << " to "<< last_tmstamp << endl;      
    }
    
    void emit(void);
    uint64_t next_event(void)
    {
        if(trace==NULL)
            return NULL_EVENT;
        return trace->getTstamp();
    }
    bool in_use(void)
    {
        return(Mem.use_count()?true:false);
    }
    void ending(void)
    {
        cout << "Reads: "<< read_ << " writes: "<< write_ << endl;
    }
    private:
    Memory::Interface::Shared Mem;
    ReadGzip gz;
    Memory::Trace FakeTrace;
    Memory::TraceMem *trace;
    
    uint64_t read_, write_;
    
    uint64_t first_nanos, first_tmstamp, last_nanos, last_tmstamp;
        
    void read_next(void)
    {
//      cout << "Reading a trace element" << endl;
        last_nanos=trace->getNanos();
        last_tmstamp = trace->getTstamp();
        gz >> *trace;
//      cout << "-----Read: " <<*trace <<endl;
        if ( gz.eof() )
        {
            free(trace);
            trace = NULL;
            cout << "END OF FILE: Reads: "<< read_ << " writes: "<< write_ << endl;
            cout << "             Nanos from "<< first_nanos << " to "<< last_nanos << endl;
            cout << "             Cycles from "<< first_tmstamp << " to "<< last_tmstamp << endl;
            return;
        }
//      cout << "Trace after " << *trace << endl;
    }
};

void InjectMemory::emit(void)
{
    if(trace == NULL)
    {
        throw runtime_error("Emitting a null trace value");
    }
    
    LOG("Injecting at ", Mem->Name(), " - ", *trace);
    Memory::Access access(trace->getPhys(), trace->getPhys(), trace->getLength());
    
    //SimNow::Nanos::setnanos(trace.getNanos());
    
    Memory::Trace mt(FakeTrace.getCpu());
    if (trace->isRead())
    {
        read_++;
        Mem->read(access, trace->getTstamp(), mt, Memory::INVALID);
    }
    else
    {
        write_++;
        Mem->write(access, trace->getTstamp(), mt, Memory::INVALID);
    }
    read_next();
}


class Machine : public metric
{
    typedef set<Cpu*> Cpus;
    typedef set<InjectMemory*> Mems;

    Cpus cpus;
    Mems mems;
    public:

    ~Machine() 
    {
        for(Cpus::iterator i=cpus.begin();i!=cpus.end();++i)
            delete *i;
        
        for(Mems::iterator i=mems.begin();i!=mems.end();++i)
            delete *i;
    }

    static void memory_inject(Memory::lua& m, string tracefile) { get()->memory_inject_(m, tracefile); }
    void memory_inject_(Memory::lua& m, string tracefile);
    
    static Cpu* new_cpu(string tracefile) { return get()->new_cpu_(tracefile); }
    Cpu* new_cpu_(string);

    void emit() { for_each(cpus.begin(),cpus.end(),mem_fun(&Cpu::emit)); }
    
    void emit_memory();
    
    static Machine* get() 
    { 
        static Machine singleton;
        return &singleton;
    }
};

void Machine::memory_inject_(Memory::lua& m, string tracefile)
{
    InjectMemory *im = new InjectMemory(m, tracefile, *cpus.begin());
    
    cout << "Memory injection was called for the file " <<  tracefile << endl;
    cout << "Item size: " << m.pointer->item_size() << endl; 
    cout << "Opening file " << tracefile << endl;
    mems.insert(im);
}

void Machine::emit_memory()
{
    //Select the next access with the minimum timestamp and emit it
    InjectMemory *im;
    unsigned int out=0;
    im = *(mems.begin());
    for(Mems::iterator i=mems.begin();i!=mems.end();++i)
    {
        if((*i)->next_event()!=NULL_EVENT)
        {
            if((*i)->next_event() < im->next_event() || im->next_event() == NULL_EVENT)
                im=*i;
        }
        else
            out++;
    }
    if (out == mems.size())
    {
        cout << "ending process" << endl;
        for(Mems::iterator i=mems.begin();i!=mems.end();++i)
            (*i)->ending();
        throw runtime_error("end");
    }
    im->emit();
}

Cpu* Machine::new_cpu_(string tracefile) 
{
    Cpu* c=new Cpu(cpus.size(),tracefile);
    cpus.insert(c);
    add("",*c);

    return c;
}

luabind::scope definition()
{
    luabind::scope a=luabind::def("new_cpu", &Machine::new_cpu);

    typedef StaticSet<luabind::scope (*)()> Funcs;
    for(Funcs::iterator i=Funcs::begin();i!=Funcs::end(); ++i)
        a=a,(*i)();

    return 
        luabind::class_<Cpu>("Cpu")
            .def("timer",             &Cpu::timer)
            .def("instruction_cache", &Cpu::instruction_cache)
            .def("data_cache",        &Cpu::data_cache)
            .def("instruction_tlb",   &Cpu::instruction_tlb)
            .def("data_tlb",          &Cpu::data_tlb),
        luabind::class_<Memory::lua>("Memory")
            .def("next", &Memory::lua::next),
        luabind::def("memory_inject", &Machine::memory_inject),
        a;
}

int main(int argc,char*argv[])
{
    try 
    {
        Option::init(argc,argv);
#ifdef USE_MEMGATE
        MemGate::depth=Option::get<int>("memgate_depth");
        MemGate::check_base=Option::get<bool>("memgate_check_base");
#endif
        Selector::all()=Option::get<int>("debug_messages");
        if(Option::has("debug_this")) 
        {
            vector<string> dt=Option::getV<string>("debug_this");
            for(vector<string>::iterator i=dt.begin();i!=dt.end();i++)
                Selector::on(*i);
        }
        if(Option::has("help"))
        {
            Option::print(cout);
            cout << kill;
        }
        LOG("Initialize");
        HeartBeat::add(*Machine::get());
        Option::run_function("build");
    
        string injection_type = Option::get<string>("injection_type");
        if (injection_type == "cpu")
        {
            try
            {
                StateObserver::transition(SIMULATION);
                uint64_t maxInsns=Option::get<uint64_t>("instructions");
                for(uint64_t qInsns=0; maxInsns==0 || qInsns < maxInsns; qInsns++)
                    Machine::get()->emit();
            }
            catch(runtime_error& err)
            {
                if(string("end") != err.what())
                    ERROR("caught exception: ", err.what());
            }
        }
        else if (injection_type == "memory")
        {
            cout << "Memory injection selected ... " << endl;
            StateObserver::transition(SIMULATION);              
            time_t init, act, next;
            next = init = time(NULL);
            try
            {
                uint64_t maxAccesses=Option::get<uint64_t>("memory_accesses");
                for(uint64_t qAcc=0; maxAccesses==0 || qAcc < maxAccesses; qAcc++)
                {
                    Machine::get()->emit_memory();
                    act = time(NULL);
                    if( (act-init)%5==0 && act>next )
                    {
                        cout << "Time " << act - init << " : " << qAcc/1000000 << " M accesses. "
                             << (long long int) ((float)qAcc ) / (act-init) << " acc/sec."
                             << endl;
                        next=act;
                    }
                }
            }
            catch(runtime_error& err)
            {
                if(string("end") != err.what())
                    ERROR("caught exception: ", err.what());
            }
        }
        else if (injection_type == "disk")
        {
            cout << "Disk injection is not yet implemented. " << endl;
        }
        else
        {
            throw runtime_error("Invalid injection type '"+ injection_type + "'");
        }
        Machine::get()->compute_metrics();
        HeartBeat::last_beat();
    }
    catch(std::exception& err)
    {
        ERROR("caught exception: ", err.what());
    }
    return 0;
}

namespace Cotson
{
    uint64_t nanos()
    {
        return 0;
    }
}

extern "C" void slirp_input(const uint8_t*b, int len) { }
