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

// $Id: cache_line.h 11 2010-01-08 13:20:58Z paolofrb $
#ifndef CACHE_LINE_H
#define CACHE_LINE_H

#include "memory_interface.h"

namespace Memory
{

struct Line 
{
	inline Line() : tag(~0ULL),lru(0),moesi(INVALID) {}
	inline Line(uint64_t t,uint64_t l,MOESI_state m) : tag(t),lru(l),moesi(m) {}
	uint64_t tag;   // tag
	uint64_t lru;	// counter for replacement
	MOESI_state moesi;
};

}

#endif
