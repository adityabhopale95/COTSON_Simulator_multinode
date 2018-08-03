// (C) Copyright 2006-2012 Hewlett-Packard Development Company, L.P.
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

// #define _DEBUG_THIS_

#include "abaeterno_config.h"

#include "lua_helpers.h"
#include "storage_one_way.h"
#include "storage_two_way.h"
#include "storage_four_way.h"
#include "storage_many.h"
#include "storage_basic.h"

#include "cache_timing_basic.h"
#include "cache_timing_l2.h"
#include "cache_timing_mainmem.h"
#include "cache_impl.h"

using namespace std;
using namespace boost;

namespace {

using namespace boost::assign;
using namespace Memory;

template <typename CT>
Interface::Shared bestCache(Parameters& p)
{
	uint32_t num_sets = p.get<uint32_t>("num_sets");
	switch (num_sets) 
	{
	    case 1: return Interface::Shared(new CacheImpl<Storage::OneWay,CT>(p));
		case 2: return Interface::Shared(new CacheImpl<Storage::TwoWay,CT>(p)); 
		case 4: return Interface::Shared(new CacheImpl<Storage::FourWay,CT>(p));
		default: break;
    }
	if (num_sets > 8) 
	    return Interface::Shared(new CacheImpl<Storage::Many,CT>(p));
    return Interface::Shared(new CacheImpl<Storage::Basic,CT>(p));
} 

Interface::Shared buildCache(Parameters& p)
{
	string timing=p.get<string>("timing","basic");
	if(timing=="basic")
		return bestCache<Timing::Basic>(p);
	if(timing=="l2")
		return bestCache<Timing::L2>(p);
    if(timing=="mainmem") 
		return bestCache<Timing::MainMemory>(p);
	throw invalid_argument("unknown cache timing '"+timing+"'");
}

lua* build_cache(luabind::object table)
{
	set<string> required;
	required += "name","size","line_size","num_sets","latency";
	Parameters p=Option::from_lua_table(table,required,"Cache");
	p.set("type",      "cache");
	p.track();

	Interface::Shared sh = buildCache(p);
	Interface* mp=sh.get();
	TaggedMetrics tm=TaggedMetrics::get();
	tm.add(mp,p.get<string>("name"));
	tm.add(mp,p.get<string>("type"));
	tm.add(mp,"mem");
	if(p.has("tag"))
		tm.add(mp,p.get<string>("tag"));
	
	lua* th=new lua(sh);
	
	lua::destroy().add(th);
	luabind::object next=table["next"];
	if(luabind::type(next)==LUA_TUSERDATA)
	{
		lua l=luabind::object_cast<lua>(next);
		th->next(l);
	}
	return th;
}

lua* build_tlb(luabind::object table)
{
	set<string> required;
	required += "name","page_size","entries","num_sets","latency";
	Parameters p=Option::from_lua_table(table,required,"TLB");
	p.set("type",      "tlb");
	p.track();

	uint32_t page_size=p.get<uint32_t>("page_size");
	uint32_t entries=p.get<uint32_t>("entries");
	string size=lexical_cast<string>(page_size*entries);
	p.set("line_size", p.get<string>("page_size"));
	p.set("size",      size);

	Interface::Shared sh=buildCache(p);
	lua* th=new lua(sh);
	Interface* mp=sh.get();
	TaggedMetrics tm=TaggedMetrics::get();
	tm.add(mp,p.get<string>("name"));
	tm.add(mp,p.get<string>("type"));
	tm.add(mp,"mem");
	if(p.has("tag"))
		tm.add(mp,p.get<string>("tag"));
	
	lua::destroy().add(th);
	luabind::object next=table["next"];
	if(luabind::type(next)==LUA_TUSERDATA)
	{
		lua l=luabind::object_cast<lua>(next);
		th->next(l);
	}
	return th;
}

luabind::scope build_tlb_binder()
{
	return luabind::def("TLB",&build_tlb);
}

luabind::scope build_cache_binder()
{
	return luabind::def("Cache",&build_cache);
}

register_lua o1(build_tlb_binder);
register_lua o2(build_cache_binder);

}
