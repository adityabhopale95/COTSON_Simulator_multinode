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
#ifndef MEMORY_TRACE_H
#define MEMORY_TRACE_H

#include "inline.h"

class CpuTimer;

namespace Memory 
{
class Interface;

typedef enum {
	ND=0,       // Not defined (default)
	READ,       // I want to fetch to read
	READX,      // I want to fetch, so that I can write
	WRITE,      // I want to write to the next level
} AccessType;

class Trace
{
private:
	struct MemType 
	{
	    const Interface* mem;
	    AccessType type;
		INLINE MemType(const Interface* m,AccessType t):mem(m),type(t){}
		INLINE MemType():mem(0),type(ND){}
	};
	
	static const int max_mem=8; // Max fixed size, for speed 
	const CpuTimer* cpu;
	int next_mem;
	MemType mtrace[max_mem];

public:
	INLINE Trace(const CpuTimer* c): cpu(c),next_mem(0) {}

	INLINE Trace& add(const Interface *i, AccessType t) 
	{
		if (next_mem<max_mem)
	        mtrace[next_mem++]=MemType(i,t);
		return *this;
	}

	INLINE Trace& operator=(const Trace& mt) 
	{
	    cpu=mt.cpu;
		next_mem=mt.next_mem;
		for(int i=0; i<next_mem; ++i)
		    mtrace[i]=mt.mtrace[i];
	    return *this;
	}

	INLINE int size() const { return next_mem; }
	INLINE void reset() { next_mem=0; }
	INLINE const CpuTimer* getCpu() const { return cpu; }

	INLINE const Interface* getMem(int i) const { return mtrace[i].mem; }
	INLINE const Interface* getMem() const { return next_mem ? getMem(next_mem-1) : 0; }

	INLINE AccessType getType(int i) const { return mtrace[i].type; }
	INLINE AccessType getType() const { return next_mem ? getType(next_mem-1) : ND; }
};

} // namespace Memory
std::ostream& operator<<(std::ostream&,const Memory::Trace&);

#endif

