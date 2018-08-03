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

// $Id: memory_access.h 84 2010-03-12 15:08:23Z paolofrb $
#ifndef MEMORY_ACCESS_H
#define MEMORY_ACCESS_H

#include "dump_gzip.h"
#include "read_gzip.h"
#include "inline.h"

namespace Memory { 
struct Access
{
	uint64_t virt;
	uint64_t phys;
	uint32_t length;
	
	INLINE Access():virt(0),phys(0),length(0) {}
	INLINE Access(uint64_t v):virt(v),phys(v),length(8) {}
	INLINE Access(uint64_t v,uint64_t p):virt(v),phys(p),length(8) {}
	INLINE Access(uint64_t v,uint64_t p,uint32_t l):virt(v),phys(p),length(l) {}
};
}

std::ostream& operator<<(std::ostream&,const Memory::Access&);
DumpGzip& operator<<(DumpGzip&,const Memory::Access&);
ReadGzip& operator>>(ReadGzip&,Memory::Access&);

#endif
