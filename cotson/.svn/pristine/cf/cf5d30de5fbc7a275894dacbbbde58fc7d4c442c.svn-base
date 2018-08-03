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
#ifndef CPU_TIMER_H
#define CPU_TIMER_H

#include "libmetric.h"
#include "instruction.h"
#include "states.h"
#include "trace_needs.h"
#include "memory_interface.h"
#include "state_observer.h"
#include "logger.h"

class CpuTimer : public metric, public SimpleStateObserver
{
public: 
	CpuTimer(uint64_t*c,uint64_t*i) : trace_needs(c,i)
	{
		trace_needs.st[FUNCTIONAL].clear();
		trace_needs.st[SIMPLE_WARMING].set(EmitFunction::bind<CpuTimer,&CpuTimer::simple_warming>(this));
		trace_needs.st[FULL_WARMING].set(EmitFunction::bind<CpuTimer,&CpuTimer::full_warming>(this));
		trace_needs.st[SIMULATION].set(EmitFunction::bind<CpuTimer,&CpuTimer::simulation>(this));
		trace_needs.st[END].clear();
		trace_needs.idle=IdleFunction::bind<CpuTimer,&CpuTimer::idle>(this);
	}
	virtual ~CpuTimer() {}
	
	typedef void (CpuTimer::*process) (const Instruction*);
	virtual void simulation(const Instruction*)=0;
	virtual void simple_warming(const Instruction*)=0;
	virtual void full_warming(const Instruction*)=0;
	virtual void idle(uint64_t)=0;

	virtual void addMemory(std::string,Memory::Interface::Shared) {}

	virtual const TraceNeeds * needs() { return &trace_needs; }
	uint64_t id() const { return devid; }
	void id(uint64_t n) { devid = n; }
protected:
	TraceNeeds trace_needs;
private:
	uint64_t devid;
};

#endif 
