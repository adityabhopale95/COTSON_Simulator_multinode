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
#ifndef STORAGE_ONE_WAY_H
#define STORAGE_ONE_WAY_H

#include "storage.h"

namespace Memory {
namespace Storage {

struct OneWay : public detail::base<Line>
{
	OneWay(const Parameters&p):detail::base<Line>(p)
	{
	    cache.init(Line());
	}
	inline Line* find(uint64_t);
	inline Line* find_lru(uint64_t addr) { return &(cache[addr]); }
	inline void allocate(Line*, uint64_t,uint64_t,MOESI_state);
};

INLINE Line* OneWay::find(uint64_t addr)
{
	uint64_t tag=cache.group_id(addr);
	if(last_line && tag==last_tag)
		return last_line;	
	Line& v=cache[addr];
	if (tag==v.tag)
		return last_tag=v.tag, last_line=&v;
	return 0;
}

inline void OneWay::allocate(Line *l,uint64_t addr,uint64_t lru,MOESI_state ms)
{
	l->tag=last_tag=cache.group_id(addr);
	l->lru=lru;
	l->moesi=ms;
	last_line=l;
}

}}
#endif
