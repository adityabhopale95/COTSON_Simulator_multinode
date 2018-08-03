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

// #define _DEBUG_THIS_

#include "abaeterno_config.h"
#include "lua_helpers.h"

#include "main_memory.h"
#include "cache_timing_basic.h"
#include "cache_timing_l2.h"
#include "cache_timing_mainmem.h"
#include "error.h"


using namespace std;
using namespace boost;

namespace {

using namespace boost::assign;
using namespace Memory;

lua* build_memory(luabind::object table)
{
	set<string> required;
	required += "name", "latency";
	Parameters p=Option::from_lua_table(table,required,"Memory");
	p.set("type",     "memory");
	p.track();

	string timing=p.get<string>("timing","basic");
	Interface::Shared sh;
	if (timing=="basic")
	    sh = Interface::Shared(new Main<Timing::Basic>(p));
	else if(timing=="l2")
	    sh = Interface::Shared(new Main<Timing::L2>(p));
    else if(timing=="mainmem")
	    sh = Interface::Shared(new Main<Timing::MainMemory>(p));
	else
	    throw invalid_argument("unknown memory timing '"+timing+"'");

	// lua* th=new lua(sh=Interface::Shared(Factory<Interface>::create(p)));
	lua* th=new lua(sh);
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

luabind::scope build_memory_binder()
{
	return luabind::def("Memory",&build_memory);
}

register_lua o(build_memory_binder);

}
