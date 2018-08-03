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
#include "abaeterno_config.h"
#include "memory_interface.h"
#include "memory_trace.h"
#include "liboptions.h"
#include "lua_helpers.h"
#include "cpu_timer.h"

using namespace std;
using namespace boost;
using namespace boost::assign;

namespace Memory {

class Tracer : public Interface
{	
public:
	Tracer(const Parameters&);
	
	MemState read(const Access&,uint64_t,Trace&,MOESI_state);
	MemState readx(const Access&,uint64_t,Trace&,MOESI_state);
	MemState write(const Access&,uint64_t,Trace&,MOESI_state);
	
	MOESI_state state(const Access&,uint64_t);
	void invalidate(const Access&,uint64_t,const Trace&,MOESI_state);

	uint32_t item_size() const { return 64; }

private:
	DumpGzip gz;
}; 

registerClass<Interface,Tracer> mem_tracer_c("tracer");

Tracer::Tracer(const Parameters& p) : 
	gz(p.get<string>("trace_file"))
{
	name=p.get<string>("name");
}


MemState Tracer::read(const Access& m, uint64_t tstamp, Trace& mt, MOESI_state ms) 
{
	stringstream s;
	s << format("%10llu r 0x%016X 0x%016X") % tstamp % m.phys % m.virt;
	s << " [" << mt << "]\n";
	gz.as_text(s.str());
	return next->read(m,tstamp,mt,ms);;
}

MemState Tracer::readx(const Access& m, uint64_t tstamp, Trace& mt, MOESI_state ms) 
{
	stringstream s;
	s << format("%10llu x 0x%016X 0x%016X") % tstamp % m.phys % m.virt;
	s << " [" << mt << "]\n";
	gz.as_text(s.str());
	return next->readx(m,tstamp,mt,ms);;
}

MemState Tracer::write(const Access& m, uint64_t tstamp, Trace& mt, MOESI_state ms) 
{
	stringstream s;
	s << format("%10llu w 0x%016X 0x%016X") % tstamp % m.phys % m.virt;
	s << " [" << mt << "]\n";
	gz.as_text(s.str());
	return next->write(m,tstamp,mt,ms);
}

MOESI_state Tracer::state(const Access& m, uint64_t tstamp) 
{
	for(vector<Interface*>::iterator i=prevs.begin();i!=prevs.end();++i)
	{
		MOESI_state pst=(*i)->state(m,tstamp);
		if(pst!=INVALID)
			return pst;
	}
	return INVALID;
}

void Tracer::invalidate(const Access& m, uint64_t tstamp, const Trace& mt, MOESI_state ms) 
{
	vector<Interface*>::iterator i = prevs.begin();
	for(; i != prevs.end(); ++i)
		(*i)->invalidate(m,tstamp,mt,ms);
}

lua* build_mem_tracer(luabind::object table)
{
	set<string> required;
	required += "name", "trace_file";
	Parameters p=Option::from_lua_table(table,required,"Tracer");
	p.set("type",     "tracer");
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

luabind::scope build_mem_tracer_binder()
{
	return luabind::def("Tracer",&build_mem_tracer);
}

register_lua o(build_mem_tracer_binder);

}
