#include "abaeterno_config.h"
#include "lua_helpers.h"

using namespace std;
using namespace boost;

namespace {

using namespace boost::assign;
using namespace Memory;

lua* build_hybrid_memory(luabind::object table)
{
	std::string classname("hybrid_memory_");
	std::string policy;
	set<string> required;

	required += "name", "latency", "flash_rlatency", "flash_wlatency", 
				"policy", "flash_pagesize", "flash_pages_per_block", "flash_size";
	Parameters p=Option::from_lua_table(table,required,"HybridMemory");
	policy=p.get<string>("policy");
	classname.append(policy);
	p.set("type",     classname.c_str());
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
	return th;
}

luabind::scope build_hybrid_memory_binder()
{
	return luabind::def("HybridMemory",&build_hybrid_memory);
}

register_lua o(build_hybrid_memory_binder);

}
