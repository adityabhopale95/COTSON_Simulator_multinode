#include "abaeterno_config.h"

#include "lua_helpers.h"
#include "bus.h"
#include "protocol.h"
#include "mesi.h"
#include "moesi.h"
#include "simple_protocol.h"
#include "futurebus.h"

using namespace std;
using namespace boost;

namespace {

using namespace boost::assign;
using namespace Memory;

lua* build_bus(luabind::object table)
{
	set<string> required;
	required += "name","protocol","latency";
	Parameters p=Option::from_lua_table(table,required,"Bus");
	p.track();

	string protocol=p.get<string>("protocol");
	
	Interface::Shared sh;
	if (protocol=="MESI")
	{
		sh = Interface::Shared(new Bus<Memory::Protocol::MESI>(p));
	} 
	else if (protocol=="MOESI")
	{
		sh = Interface::Shared(new Bus<Memory::Protocol::MOESI>(p));
	}
	else if (protocol=="SIMPLE")
	{
		sh = Interface::Shared(new Bus<Memory::Protocol::SIMPLE>(p));
	}
	else if (protocol=="FUTUREBUS")
	{
		sh = Interface::Shared(new Bus<Memory::Protocol::FUTUREBUS>(p));
	}

	lua* th = new lua(sh);
	Interface* mp=sh.get();
	TaggedMetrics tm=TaggedMetrics::get();
	tm.add(mp,p.get<string>("name"));
	tm.add(mp,"bus");
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

luabind::scope build_bus_binder()
{
	return luabind::def("Bus",&build_bus);
}

register_lua o(build_bus_binder);

}
