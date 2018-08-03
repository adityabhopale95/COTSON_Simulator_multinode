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
#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "dump_gzip.h"
#include "read_gzip.h"
#include "memory_access.h"
#include "simplevec.h"
#include "inline.h"
#include "opcodes.h"

class Instructions;

// FIXME: this shouldn't be here
class InstType {
public:
	enum itype {
	   UNDEF=0,
	   // hack to preserve validity of old injector traces
	   DEFAULT='F', CTRL_BRANCH='B', CTRL_CALL='C', 
	   CTRL_JUMP='J', CTRL_RET='R', CTRL_IRET='r',
	   _MAXCTRL=127,
	   // any other type above MAXCTRL
	   CPUID=_MAXCTRL+1,
	   PREFETCH=_MAXCTRL+2,
	   CLFLUSH=_MAXCTRL+3,
	   MEMFAKE=_MAXCTRL+4,
	};

	InstType(const uint8_t*);
	INLINE InstType(itype t=UNDEF):type(t){}
	INLINE InstType(uint8_t t):type(t){}

	INLINE bool is_control()  const { return type && type < _MAXCTRL && type != DEFAULT; }
	INLINE bool is_branch()   const { return type==CTRL_BRANCH; }
	INLINE bool is_cpuid()    const { return type==CPUID; }
	INLINE bool is_prefetch() const { return type==PREFETCH; }
	INLINE bool is_clflush()  const { return type==CLFLUSH; }
	INLINE bool is_ret()      const { return type==CTRL_RET || type==CTRL_IRET; }
	INLINE bool is_call()     const { return type==CTRL_CALL; }
	INLINE operator uint8_t() const { return type; }
	// major hack to aggregate ret/iret stats
	INLINE uint8_t stat_index() const { return toupper(type); }
private:
    uint8_t type;
};

class Instruction : public boost::noncopyable
{
public:
	typedef simplevec<Memory::Access> Accesses;
	typedef Accesses::const_iterator MemIterator;

private:
	Memory::Access pc;
	const Opcode* opcode;
	Accesses loads,stores;
	InstType type;
	
	simplevec<uint64_t> xdata; // runtime data for cpuid | custom asm
	uint64_t cr3;
	uint64_t iid;
	static uint64_t unique_id;

	INLINE void clone(const Instruction& other)
	{
		pc=other.pc;
		type=other.type;
		iid=other.iid;
		cr3=other.cr3;
		// pointer to runtime data (custom)
		xdata.copy(other.xdata);
		// pointer to opcode data
		opcode=other.opcode;
		// memory ops: copy and shrink to fit
		loads.copy(other.loads);
		stores.copy(other.stores);
	}

public:

	INLINE Instruction():
		pc(0,0,0),opcode(0),cr3(0),iid(0) {}

	// this function reuses another instruction !
	INLINE static void init(
	    Instruction* p,
		uint64_t vpc,uint64_t ppc,uint l,
		uint64_t lcr3,
		const Opcode *op,
		InstType t)
	{
		p->pc=Memory::Access(vpc,ppc,l);
		p->opcode=op;
		p->type=t;
		p->loads.clear();
		p->stores.clear();
		p->cr3=lcr3;
		p->xdata.clear();
		p->iid = ++unique_id;
	}

	~Instruction() { }

	uint64_t instruction_id() const { return iid; }

	INLINE void Type(InstType t) { type=t; }
	INLINE InstType Type() const { return type; }

	INLINE bool is_control()  const { return type.is_control(); }
	INLINE bool is_branch()   const { return type.is_branch(); }
	INLINE bool is_cpuid()    const { return type.is_cpuid(); }
	INLINE bool is_prefetch() const { return type.is_prefetch(); }
	INLINE bool is_clflush()  const { return type.is_clflush(); }
	INLINE bool is_ret()      const { return type.is_ret(); }
	INLINE bool is_call()     const { return type.is_call(); }

	INLINE void add_xdata(uint64_t x) { xdata.push_back(x); }
	INLINE uint64_t get_xdata(int i) const { return xdata[i]; }

	void disasm(std::ostream&) const;

	// INLINE void PC(uint64_t _pc) { pc = _pc; }

	INLINE void StoresPush(const Memory::Access& a) { stores.push_back(a); }
	INLINE void LoadsPush(const Memory::Access& a) { loads.push_back(a); }

	INLINE int Stores() const { return stores.size(); }
	INLINE int Loads() const { return loads.size(); }

	//these functions are used by the user of the instruction   
	//we should separate this into two different interfaces
	INLINE const Memory::Access& PC() const { return pc; }

	INLINE MemIterator LoadsBegin() const { return loads.begin(); }
	INLINE MemIterator LoadsEnd() const { return loads.end(); }
	INLINE MemIterator StoresBegin() const { return stores.begin(); }
	INLINE MemIterator StoresEnd() const { return stores.end(); }
	
	INLINE uint64_t getCR3() const { return cr3; }
	INLINE const Opcode* getOpcode() const { return opcode; }

	friend DumpGzip& operator<<(DumpGzip&,const Instruction&);
	friend std::ostream& operator<<(std::ostream&,const Instruction&);

	friend class Instructions;
};


#endif
