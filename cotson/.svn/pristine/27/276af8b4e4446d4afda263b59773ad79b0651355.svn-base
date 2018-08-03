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
#ifndef STORAGE_BASIC_H
#define STORAGE_BASIC_H

#include "storage.h"
#include "logger.h"

namespace Memory {
namespace Storage {

typedef std::vector<Line> CacheLineVec;

struct Basic : public detail::base<CacheLineVec>
{
	Basic(const Parameters&p) : detail::base<CacheLineVec>(p)
	{
		cache.init(CacheLineVec(num_sets,Line()));
	}
	inline Line* find(uint64_t);
	inline Line* find_lru(uint64_t);
	inline void allocate(Line*,uint64_t,uint64_t,MOESI_state);
};

inline Line* Basic::find_lru(uint64_t addr)
{
	CacheLineVec& v=cache[addr];
	CacheLineVec::iterator lru=v.begin(), set=v.begin();
	for(++set; set!=v.end(); ++set)
	    if (set->lru < lru->lru)
		    lru=set;
	return &(*lru);
}

INLINE Line* Basic::find(uint64_t addr)
{
	uint64_t tag=cache.group_id(addr);
	if(last_line && tag==last_tag)
		return last_line;

	CacheLineVec& v=cache[addr];
	for(CacheLineVec::iterator set=v.begin(); set!=v.end(); ++set)
		if (set->tag==tag) 
		{
			last_tag=set->tag;
			return (last_line=&(*set));
		}
	return 0;
}

inline void Basic::allocate(Line* l,uint64_t addr,uint64_t lru,MOESI_state ms)
{
	l->tag=last_tag=cache.group_id(addr);
	l->lru=lru;
	l->moesi=ms;
	last_line=l;
}

}}
#endif
