// This file is the simulation component and needs to be 
// compiled as part of abaeterno itself. In this example, 
// we register the "magic" code 1234 as the ID that triggers
// the execution of functional_callback (when in functional)
// or simulation_callback (when in simulation)

#include "options.h"
#include "cpuid_call.h"
#include "cotson.h"

#define MAGIC 1234

using namespace std;
using namespace boost;

namespace {

uint8_t simbuf[20];

void* functional_callback(
	FunctionalState f,
	uint64_t nanos,
	uint64_t devid,
	uint64_t mymagic,uint64_t a,uint64_t vcp_withb) 
{
	static bool init=true;
	if (init) 
	{
		init=false;
		cout << "first time around" << endl;
	}

	cout << "Functional Callback:" << endl;
	if(f==ONLY_FUNCTIONAL)
		cout << "  simulation is not going to be called" << endl;
	else 
		cout << "  simulation will be called later" << endl;
	cout << "  nanos:    " << nanos << endl;
	cout << "  core:     " << devid << endl;
	cout << "  mymagic:  " << mymagic << endl;
	cout << "  a:        " << a << endl;

	// Read the buf pointed to by 'vcp_withb' (we know the size, in this simple case)
	// in simulated memory. Notice the virtual->physical conversion first
	uint64_t pcp = Cotson::Memory::physical_address(vcp_withb);
	Cotson::Memory::read_physical_memory(pcp,20,simbuf);
	cout << "  vcp_withb: " << hex << "0x" << vcp_withb << dec << ": \"" << simbuf << "\"" << endl;

	return 0;
}

InstructionInQueue simulation_callback(
	Instruction* inst,
	uint64_t devid)
{
	cout << "Simulation Callback" << endl;
	uint64_t op        = inst->get_xdata(0);
	uint64_t mymagic   = inst->get_xdata(1);
	uint64_t a         = inst->get_xdata(2);
	uint64_t vcp_withb = inst->get_xdata(3);
	// cout << "  core: " << devid << endl; // right now this does not work!
	cout << "       op: " << hex << op << dec << endl;
	cout << "  mymagic: " << mymagic << endl;
	cout << "        a: " << a << endl;
	// Note that we can't call Cotson::Memory here because we're past
	// functional execution, hence we have no injector available and 
	// memory could have been overwritten in between.
	// Anything we need has to be collected in the functional part
	// such as the 'simbuf' variable.
	cout << "vcp_withb: " << hex << "0x" << vcp_withb << dec << ": \"" << simbuf << "\"" << endl;
	return DISCARD; 
}

cpuid_call c1(MAGIC,&functional_callback,simulation_callback);

} 
