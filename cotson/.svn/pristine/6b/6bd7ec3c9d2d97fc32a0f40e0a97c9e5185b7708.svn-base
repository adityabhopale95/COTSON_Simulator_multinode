// This file is the simulation component and needs to be 
// compiled as part of abaeterno itself. In this example, 
// we register a special custom assembler instruction

#include "options.h"
#include "cotson.h"
#include "cpuid_call.h"
#include "custom_asm.h"

#include <boost/scoped_ptr.hpp>
#include <boost/pool/pool.hpp>
#include "machine.h"

#include <iostream>

using namespace std;

namespace { // Wrap everything into an anonymous namespace

// Data passed between functional and timing -- locally managed (see pool below)
struct XData { uint64_t arg1, arg2; };
boost::pool<> xdata_pool(sizeof(struct XData));

// Local memory
vector<uint64_t> local_table;

//===========================================================================================
// The functional execution part
//===========================================================================================
// Auxiliary functions to parse & read register arguments (functional)
#include "regtab.h"

void asm_reset()
{
    xdata_pool.release_memory();
}

inline static uint64_t read_arg1(uint8_t op)
{
    uint64_t arg=0;
    #define _TSU_ARG1(_b,_R1,_R2) case _b: arg=Cotson::X86::_R1(); break
    switch(op) {
        _TSU_REGTAB(_TSU_ARG1);
        default: throw runtime_error("unkown ASM register");
    }
    return arg;
}

inline static uint64_t read_arg2(uint8_t op)
{
    uint64_t arg=0;
    #define _TSU_ARG2(_b,_R1,_R2) case _b: arg=Cotson::X86::_R2(); break
    switch(op) {
        _TSU_REGTAB(_TSU_ARG2);
        default: throw runtime_error("unkown ASM register");
    }
    return arg;
}

inline static void write_res(uint8_t op, uint64_t res)
{
    #define _TSU_RES(_b,_R1,_R2)  case _b: Cotson::X86::_R1(res); break
	switch(op) {
	    _TSU_REGTAB(_TSU_RES);
		default: throw runtime_error("unkown TSU register");
	}
    Cotson::X86::UpdateRegs();
}

void* asm_func(
	FunctionalState f, uint64_t nanos, uint64_t devid,
	uint64_t op,uint64_t regop,uint64_t im) 
{
	// Exchange data structure from functional to timing
	XData* const xdp = (f==ONLY_FUNCTIONAL) ? 0 : new (xdata_pool.malloc()) XData();

    switch (op) {
        case _ASMOP(_TSU_TINIT): {
            uint64_t n = read_arg1(regop);
			cout << "F" << devid <<": INIT " << n << endl;
			local_table.resize(n); // allocate a vector of size 'n'
			if (xdp) xdp->arg1 = n; // save argument for timing simulation
		    break;
        }
        case _ASMOP(_TSU_TREAD): {
            uint64_t i = read_arg1(regop);
			if (i >= local_table.size())
                throw runtime_error("index too large");
			uint64_t x = local_table[i]; // read element 'i'
			cout << "F" << devid <<": READ table[" << i << "] => " << x << endl;
			write_res(regop,x);
			if (xdp) xdp->arg1 = i; // save argument for timing simulation
		    break;
		}
        case _ASMOP(_TSU_TWRITE): {
            uint64_t i = read_arg1(regop);
            uint64_t x = read_arg2(regop);
			if (i >= local_table.size())
                throw runtime_error("index too large");
			local_table[i] = x; // write 'x' to element 'i'
			cout << "F" << devid <<": WRITE table[" << i << "] <= " << x << endl;
			if (xdp) {
			    xdp->arg1 = i; // save argument for timing simulation
			    xdp->arg2 = x; // save argument for timing simulation
			}
		    break;
		}
        default:
            throw runtime_error("unkown opcode");
            break;
    }
    return static_cast<void*>(xdp);
}

//===========================================================================================
// The custom timer part
//===========================================================================================

// This is called before emitting the simulation to the timer
// The timer is not invoked if this function returns DISCARD
// 
InstructionInQueue asm_sim(
	Instruction* inst,
	uint64_t devid)
{
	CpuTimer* timer = Machine::get().cpu(devid)->cpu_timer();

	XData* xdp = reinterpret_cast<XData*>(inst->get_xdata(0)); // saved execution data
	if (!xdp) {
		cerr << "Warning: asm instruction with no execution data" << endl;
	    return KEEP;
    }
	uint64_t op = inst->get_xdata(1);
    switch (op) {
        case _ASMOP(_TSU_TINIT): {
			cout << "T" << devid <<": INIT " << xdp->arg1 << endl;
			if (timer) timer->idle(1); // advance 1 cycle
		    break;
		}
        case _ASMOP(_TSU_TWRITE): {
			cout << "T" << devid <<": WRITE " << xdp->arg1 << " " << xdp->arg2 << endl;
			if (timer) timer->idle(10); // advance 10 cycles
		    break;
		}
        case _ASMOP(_TSU_TREAD):  {
			cout << "T" << devid <<": READ " << xdp->arg1 << endl;
			if (timer) timer->idle(100); // advance 100 cycles
		    break;
		}
        default:
            throw runtime_error("unkown opcode");
            break;
	}
	return DISCARD; 
}

//===========================================================================================
// Register all functions
//===========================================================================================

cpuid_call asm_init(_ASMOP(_TSU_TINIT),&asm_func,&asm_sim,&asm_reset);
cpuid_call asm_read(_ASMOP(_TSU_TREAD),&asm_func,&asm_sim);
cpuid_call asm_write(_ASMOP(_TSU_TWRITE),&asm_func,&asm_sim);

}  // namespace

