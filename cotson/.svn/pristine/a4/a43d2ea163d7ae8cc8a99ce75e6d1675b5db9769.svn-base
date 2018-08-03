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
#ifndef TRACE_NEEDS_H
#define TRACE_NEEDS_H

#include "delegate.h"
#include "states.h"

class Instruction; 
typedef delegate<const Instruction *> EmitFunction;
typedef delegate<uint64_t>IdleFunction;

#define NEED_CODE (1)
#define NEED_MEM  (1<<1)
#define NEED_REG  (1<<2)
#define NEED_EXC  (1<<3)
#define NEED_HB   (1<<4)

class StateNeeds
{
public:
	EmitFunction emit;
	bool has_emit;	
	bool needs_code;
	bool needs_memory;
	bool needs_register;
	bool needs_exception;
	bool needs_heartbeat;

	StateNeeds() : has_emit(false), needs_code(false), 
		needs_memory(false), needs_register(false),
		needs_exception(false), needs_heartbeat(false)
		{}
	
	void setflags(uint32_t f)
	{
		needs_code=(f&NEED_CODE);
		needs_memory=(f&NEED_MEM);
		needs_register=(f&NEED_REG);
		needs_exception=(f&NEED_EXC);
		needs_heartbeat=(f&NEED_HB);
	}

	void set(const EmitFunction& e)
	{
		emit=e; 
		has_emit=true; 
		needs_code=true;
		needs_memory=true;
		needs_register=true;
		needs_exception=true;
		needs_heartbeat=true;
	}

	void clear() 
	{
		has_emit=false; 
		needs_code=false;
		needs_memory=false;
		needs_exception=false;
		needs_heartbeat=false;
		needs_register=false;
	}
};

class TraceNeeds
{
public:
	int  history; // max number of instructions that a timer may remember

	StateNeeds st[SIM_STATES];
	IdleFunction idle;

	uint64_t* cycles;
	uint64_t* instructions;

	TraceNeeds(uint64_t*c,uint64_t*i) : 
		// opcodes(false), types(false), 
		history(0),
		cycles(c), instructions(i) {}
};

#endif
