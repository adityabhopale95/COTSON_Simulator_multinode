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

// $Id: storage_four_way.h 11 2010-01-08 13:20:58Z paolofrb $
#ifndef STORAGE_FOUR_WAY_H
#define STORAGE_FOUR_WAY_H

#include "storage.h"

namespace Memory {
namespace Storage {

struct FourLines { Line e0,e1,e2,e3; };

struct FourWay: public detail::base<FourLines>
{
	FourWay(const Parameters&p):detail::base<FourLines>(p)
	{
	    cache.init(FourLines());
	}
	INLINE Line* find(uint64_t);
	INLINE Line* find_lru(uint64_t);
	INLINE void allocate(Line*,uint64_t,uint64_t,MOESI_state);
};

INLINE Line* FourWay::find_lru(uint64_t addr)
{
	FourLines& v=cache[addr];
	uint64_t lru0 = v.e0.lru;
	uint64_t lru1 = v.e1.lru;
	uint64_t lru2 = v.e2.lru;
	uint64_t lru3 = v.e3.lru;
	bool lt01 = lru0 <= lru1;
	bool lt02 = lru0 <= lru2;
	bool lt03 = lru0 <= lru3;
	bool lt12 = lru1 <= lru2;
	bool lt13 = lru1 <= lru3;
	bool lt23 = lru2 <= lru3;

    if (!lt03 && !lt13 && !lt23) 
	    return &v.e3;
	if (!lt02 && !lt12 && lt23) 
	    return &v.e2;
	if (!lt01 && lt12 && lt13) 
	    return &v.e1;
	return &v.e0;
}

INLINE Line* FourWay::find(uint64_t addr)
{
	uint64_t tag=cache.group_id(addr);
	if(last_line && tag==last_tag)
		return last_line;	
	FourLines& v=cache[addr];
	if (tag==v.e0.tag)
	    return last_tag=v.e0.tag, last_line=&v.e0;
	if (tag==v.e1.tag)
		return last_tag=v.e1.tag, last_line=&v.e1;
	if (tag==v.e2.tag)
		return last_tag=v.e2.tag, last_line=&v.e2;
	if (tag==v.e3.tag)
		return last_tag=v.e3.tag, last_line=&v.e3;
	return 0;
}

INLINE void FourWay::allocate(Line* l,uint64_t addr, uint64_t lru,MOESI_state ms)
{
	l->tag=last_tag=cache.group_id(addr);
	l->lru=lru;
	l->moesi=ms;
	last_line=l;
}

}}

#endif
