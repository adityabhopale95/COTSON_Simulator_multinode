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
#ifndef STORAGE_TWO_WAY_H
#define STORAGE_TWO_WAY_H

#include "storage.h"

namespace Memory {
namespace Storage {

struct TwoLines { Line e0,e1; };

struct TwoWay : public detail::base<TwoLines>
{
	TwoWay(const Parameters&p):detail::base<TwoLines>(p)
	{
	    cache.init(TwoLines());
	}
	INLINE Line* find(uint64_t);
	INLINE Line* find_lru(uint64_t);
	INLINE void allocate(Line*,uint64_t,uint64_t,MOESI_state);
};

INLINE Line* TwoWay::find_lru(uint64_t addr)
{
	TwoLines& v=cache[addr];
	return (v.e0.lru <= v.e1.lru) ? &v.e0 : &v.e1;
}

INLINE Line* TwoWay::find(uint64_t addr)
{
	uint64_t tag=cache.group_id(addr);
	if(last_line && tag==last_tag)
		return last_line;

	TwoLines& v=cache[addr];
	if (tag==v.e0.tag) 
		return last_tag=v.e0.tag, last_line=&v.e0;
	if (tag==v.e1.tag) 
		return last_tag=v.e1.tag, last_line=&v.e1;
	return 0;
}

INLINE void TwoWay::allocate(Line* l,uint64_t addr,uint64_t lru,MOESI_state ms)
{
	l->tag=last_tag=cache.group_id(addr);
	l->lru=lru;
	l->moesi=ms;
	last_line=l;
}

}}

#endif
