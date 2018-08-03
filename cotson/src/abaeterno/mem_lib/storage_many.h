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

// $Id: storage_many.h 11 2010-01-08 13:20:58Z paolofrb $
#ifndef STORAGE_MANY_H
#define STORAGE_MANY_H

#include "storage.h"
#include "hashmap.h"

namespace Memory {
namespace Storage {

typedef HashMap<uint64_t,Line*> CacheLineMap;

class Many : public detail::base<CacheLineMap>
{
public:
	Many(const Parameters&p):detail::base<CacheLineMap>(p)
	{
		uint64_t addr = 0;
		uint32_t num_lines = size/(num_sets*line_size);
		pool = new Line[num_sets*num_lines]; // max number of lines
		Line* cur=pool;
		// Fill cache with invalid tags (~0x0, ~0x1, ~0x2, ...)
		for (uint32_t line=0; line<num_lines; ++line) 
		{
	        CacheLineMap& v=(cache[addr]=CacheLineMap());
		    for(uint64_t s=0; s<num_sets; ++s)
			    v[~s]=new(cur++) Line(~s,0,INVALID); // placement new
			addr += line_size;
		}
	}
	~Many() { delete[] pool; }
	INLINE Line* find(uint64_t);
	INLINE Line* find_lru(uint64_t);
	INLINE void allocate(Line*,uint64_t,uint64_t,MOESI_state);

private:
	Line *pool;
};

INLINE Line* Many::find_lru(uint64_t addr)
{
	CacheLineMap& v=cache[addr];
	CacheLineMap::iterator lru=v.begin();
	CacheLineMap::iterator set=v.begin();
	while(++set!=v.end())
	    if (set->second->lru < lru->second->lru)
		    lru=set;
    return lru->second;
}

INLINE Line* Many::find(uint64_t addr)
{
	uint64_t tag=cache.group_id(addr);
	if(last_line && tag==last_tag)
		return last_line;	
	CacheLineMap& v=cache[addr];
	CacheLineMap::iterator set = v.find(tag);
	if (set!=v.end()) 
	{
		last_tag=set->first;
		return (last_line=set->second);
	}
	return 0;
}

INLINE void Many::allocate(Line* l,uint64_t addr,uint64_t lru,MOESI_state ms)
{
	uint64_t tag=cache.group_id(addr);
	CacheLineMap& v=cache[addr];
	CacheLineMap::iterator i = v.find(tag);
	if (i==v.end())
	{
	    CacheLineMap::iterator ie = v.find(l->tag);
		if (ie==v.end())
		    throw std::runtime_error("can't find line with LRU tag!");
		Line *nline=ie->second;
		v.erase(ie);
		v[tag]=new(nline) Line(tag,lru,ms);
	    last_line=nline;
		last_tag=tag;
	}
	else 
	{ // same tag found, update and don't erase!
	    i->second->lru=lru;
		i->second->moesi=ms;
    }
}

}}
#endif
