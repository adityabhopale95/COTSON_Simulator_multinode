#include "abaeterno_config.h"
#include "lua_helpers.h"

using namespace std;
using namespace boost;

namespace {

using namespace boost::assign;
using namespace Memory;

lua* build_mem_bintracer(luabind::object table)
{
	set<string> required;
	required += "name", "latency", "trace_file";
	Parameters p=Option::from_lua_table(table,required,"MemBinTracer");
	p.set("type",     "mem_bintracer");
	p.track();

	Interface::Shared sh;
	lua* th=new lua(sh=Interface::Shared(Factory<Interface>::create(p)));
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

luabind::scope build_mem_bintracer_binder()
{
	return luabind::def("MemBinTracer",&build_mem_bintracer);
}

register_lua o(build_mem_bintracer_binder);

}

