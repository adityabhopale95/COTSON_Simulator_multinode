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

// $Id: cpuid_call.h 717 2015-05-19 15:17:38Z robgiorgi $
#ifndef CPUID_CALL_H
#define CPUID_CALL_H

// Reserved function of CPUID in AMD64.
// We will use this function to pass information from the
// code executing in the vm to abaeterno

#define COTSON_RESERVED_CPUID_BASE       0x80000009
#define COTSON_RESERVED_CPUID_RANGE      30000
#define COTSON_RESERVED_CPUID_EXT        (COTSON_RESERVED_CPUID_BASE+1000)
#define COTSON_RESERVED_CPUID_SELECTIVE  10
#define COTSON_RESERVED_ASM_BASE         5000
#define COTSON_RESERVED_CASM_SELECTIVE	 255

#define IS_COTSON_CPUID(rax) ( \
    ((rax) >= COTSON_RESERVED_CPUID_BASE) && \
    ((rax) <  COTSON_RESERVED_CPUID_BASE+COTSON_RESERVED_CPUID_RANGE) )

#define IS_COTSON_EXT_CPUID(x) \
    ((x) >= COTSON_RESERVED_CPUID_EXT)

#define COTSON_EXT_CPUID_OP(x) \
    ((x)-COTSON_RESERVED_CPUID_BASE)

#include "instruction.h"
#include "custom_asm_regtab.h"
#include "cotson.h" // needed by casm_read_arg1, ...

typedef enum { ONLY_FUNCTIONAL, FUNCTIONAL_AND_TIMING } FunctionalState;
typedef enum { DISCARD, KEEP } InstructionInQueue;

typedef boost::function<void*(FunctionalState,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t)> FunctionalCall;
typedef boost::function<InstructionInQueue(Instruction*,uint64_t)> SimulationCall;
typedef void (*ResetCall)();

class CpuidCall : public boost::noncopyable
{
    typedef std::map<uint64_t,std::pair<FunctionalCall,SimulationCall> > Funcs;
    Funcs funcs;
	std::set<ResetCall> rfuncs; // reset functions (optional)

public:
         
    static CpuidCall& get() 
    {
        static CpuidCall singleton;
        return singleton;
    }

    static void reset()
	{
        CpuidCall& me=get();
	    for(std::set<ResetCall>::iterator i=me.rfuncs.begin();i!=me.rfuncs.end();++i)
		    if (*i)
			    (*i)();
	}

    static void add(uint64_t a, FunctionalCall& f, SimulationCall& s, ResetCall r)
    {
        if(a >= COTSON_RESERVED_CPUID_RANGE)
            throw std::runtime_error("cpuid op "+ 
                boost::lexical_cast<std::string>(a) + " exceeds max of " +
                boost::lexical_cast<std::string>(COTSON_RESERVED_CPUID_RANGE-1));

        CpuidCall& me=get();
        if(me.funcs.find(a)!=me.funcs.end())
            throw std::runtime_error("cannot add two hooks to the same cpuid: "+
                boost::lexical_cast<std::string>(a));
        me.funcs[a]=std::make_pair(f,s);
		me.rfuncs.insert(r);
    }

    static bool has(uint64_t a) 
    {
        CpuidCall& me=get();
        return me.funcs.find(a)!=me.funcs.end();
    }

    static void* functional(
        FunctionalState f,
        uint64_t nanos,
        uint64_t devid,
        uint64_t a,uint64_t b,uint64_t c) 
    {
        CpuidCall& me=get();
        Funcs::iterator i=me.funcs.find(a);
        if(i==me.funcs.end())
            return 0;
        FunctionalCall fcall=i->second.first;
        if(!fcall)
            return 0;
        return fcall(f,nanos,devid,a,b,c);      
    }

    static void add_asm_xdata(Instruction *inst, uint64_t op, uint64_t xdata)
	{
	    inst->add_xdata(xdata);  // 0 ptr to custom data
	    inst->add_xdata(op);     // 1 opcode
	}

    static void add_cpuid_xdata(
	    Instruction *inst, 
	    uint64_t a, uint64_t b, uint64_t c, uint64_t d)
	{
	    inst->add_xdata(a); // 0 magic
		inst->add_xdata(b); // 1 opcode
		inst->add_xdata(c); // 2 arg1
		inst->add_xdata(d); // 3 arg2
	}

    inline static uint64_t casm_read_arg1(uint8_t op)
    {
        uint64_t arg=0;
        #define _CUSTOM_ASM_ARG1(_b,_R1,_R2) case _b: arg=Cotson::X86::_R1(); break
        switch(op) {
            _CUSTOM_ASM_REGTAB(_CUSTOM_ASM_ARG1);
            default:
//                XERR("### ERROR: unknown arg1 CUSTOM_ASM register 0x%02x\n",op);
//                throw runtime_error("unknown arg1 CUSTOM_ASM register");
            break;
        }
        return arg;
    }

    inline static uint64_t casm_read_arg2(uint8_t op)
    {
        uint64_t arg=0;
        #define _CUSTOM_ASM_ARG2(_b,_R1,_R2) case _b: arg=Cotson::X86::_R2(); break
        switch(op) {
            _CUSTOM_ASM_REGTAB(_CUSTOM_ASM_ARG2);
            default:
//                XERR("### ERROR: unkown CUSTOM_ASM register 0x%02x\n",op);
//                throw runtime_error("unkown CUSTOM_ASM register");
            break;
        }
        return arg;
    }

    inline static void casm_write_res(uint8_t op, uint64_t res)
    {
        #define _CUSTOM_ASM_RES(_b,_R1,_R2)  case _b: Cotson::X86::_R1(res); break
        switch(op) {
            _CUSTOM_ASM_REGTAB(_CUSTOM_ASM_RES);
            default:
//                XERR("### ERROR: unkown CUSTOM_ASM register 0x%02x\n",op);
//                throw runtime_error("unkown CUSTOM_ASM register");
            break;
        }
        Cotson::X86::UpdateRegs();
    }

    static InstructionInQueue simulation(Instruction* inst, uint64_t devid)
    {
		if (!inst->is_cpuid())
            return KEEP;
        CpuidCall& me=get();
        Funcs::iterator i=me.funcs.find(inst->get_xdata(1)); // opcode, see above
        if(i==me.funcs.end())
            return DISCARD;
        SimulationCall& scall=i->second.second;
        if(!scall)
            return DISCARD;
        return scall(inst,devid);     
    }
};

struct cpuid_call
{
    cpuid_call(uint64_t a,FunctionalCall f,SimulationCall s,ResetCall r=0)
    {
        CpuidCall::add(a,f,s,r); 
    }
};

#endif
