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

// $Id: memory_interface.h 11 2010-01-08 13:20:58Z paolofrb $
#ifndef MEMORY_INTERFACE_H
#define MEMORY_INTERFACE_H

#include "memory_access.h"
#include "memory_trace.h"
#include "libmetric.h"
#include "state_observer.h"
#include "inline.h"

namespace Memory {

enum _MOESI_states {
    NOT_FOUND=0,
    MODIFIED, 
    OWNER, 
    EXCLUSIVE, 
    SHARED, 
    INVALID,
    MOESI_STATES_
};

class MOESI_state {
public:
    INLINE bool isExclusive() const { return (m==MODIFIED || m==EXCLUSIVE); }
    INLINE bool isDirty()     const { return (m==MODIFIED || m==OWNER); }
    INLINE bool isValid()     const { return (MODIFIED<=m && m<=SHARED); }
	INLINE MOESI_state(_MOESI_states s):m(s){}
	INLINE bool operator ==(_MOESI_states s) const { return m==s; }
	INLINE operator _MOESI_states() const { return m; }
private:
    _MOESI_states m;
};

class MemState
{
public:
	INLINE MemState(uint32_t l, MOESI_state s):lat(l),ms(s) {}
    INLINE uint32_t latency() const { return lat; }
    INLINE MOESI_state moesi() const { return ms; }
private:
    uint32_t lat;
	MOESI_state ms;
};

class Interface : public metric, public SimpleStateObserver
{
	public:
	typedef boost::shared_ptr<Interface> Shared;

	Interface() {}
	virtual ~Interface() {}

	const std::string& Name() const { return name; }

	INLINE uint32_t read(const Access& m,uint64_t tstamp,CpuTimer*cpu) 
	{
		Trace mt(cpu);
		return read(m,tstamp,mt,INVALID).latency();
	}

	INLINE void write(const Access& m,uint64_t tstamp,CpuTimer*cpu) 
	{ 
		Trace mt(cpu);
		write(m,tstamp,mt,INVALID);
	}

	virtual uint32_t item_size() const=0;

	///////////////////////////////////////////
	// TOP (CPU) -> BOTTOM (buses) interface //
	///////////////////////////////////////////

	virtual MemState read(const Access&, uint64_t, Trace&, MOESI_state ms)=0;
	virtual MemState readx(const Access&, uint64_t, Trace&, MOESI_state ms)=0;
	virtual MemState write(const Access&, uint64_t, Trace&, MOESI_state ms)=0;

	///////////////////////////////////////////
	// BOTTOM (buses) -> TOP (CPU) interface //
	///////////////////////////////////////////

	virtual MOESI_state state(const Access&, uint64_t)=0;
	virtual void invalidate(const Access&, uint64_t, const Trace&, MOESI_state ms)=0;

	// allows us to link to our unique next memory interface
	void setNext(Shared);

	protected:

	Shared next;
	std::vector<Interface*> prevs;
	std::string name;

	private:

	// automatically called by setNext to link back
	void setPrev(Interface*);
};

} // namespace Memory

std::ostream& operator<<(std::ostream&,Memory::MOESI_state);

#endif
