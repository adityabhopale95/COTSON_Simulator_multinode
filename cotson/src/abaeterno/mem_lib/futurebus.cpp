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

// $Id: futurebus.cpp 11 2010-01-08 13:20:58Z paolofrb $

#include "abaeterno_config.h"
#include "futurebus.h"
#include "logger.h"

using namespace std;
using namespace boost;

namespace Memory{
namespace Protocol{

MemState
FUTUREBUS::read(Interface* bus, Shared& next, Intf_vector& prevs, const Access& m,uint64_t tstamp,Trace& mt,MOESI_state ms)
{
	const Interface* who=mt.getMem();
	LOG("FUTUREBUS read",m.phys,who);
	bool found=false;
 	for(vector<Interface*>::iterator i=prevs.begin(); i!=prevs.end();++i) 
	{
		Interface *mi = *i;
 		if(mi == who) 
		    continue;
 		
		MOESI_state ms=mi->state(m,tstamp);
		if(ms.isValid()) 
		{
			found=true;
			if(ms==MODIFIED)
				mi->invalidate(m,tstamp,mt,OWNER);
			else if(ms==EXCLUSIVE)
				mi->invalidate(m,tstamp,mt,SHARED);
		}
 	}
	
	if (!found) 
	{
	    mt.add(bus,READ);
	    return next->read(m,tstamp,mt,ms);
	}
	else 
		return MemState(0,SHARED);
}

MemState
FUTUREBUS::readx(Interface* bus, Shared& next, Intf_vector& prevs, const Access& m,uint64_t tstamp,Trace& mt, MOESI_state ms)
{
	const Interface* who=mt.getMem();
	LOG("FUTUREBUS readx",m.phys,who);
	if(ms==MODIFIED || ms==EXCLUSIVE)
 		return MemState(0,MODIFIED);

	if(ms==SHARED || ms==OWNER)  // BusRdX*/BC (what is BusRdX*?)
	{	
		bool found=false; 
	 	for(vector<Interface*>::iterator i=prevs.begin(); i!=prevs.end();++i) 
		{
			Interface *mi = *i;
	 		if(mi == who) 
			    continue;
			MOESI_state ms=mi->state(m,tstamp);
			if(ms==SHARED || ms==OWNER) 
			{
				mi->invalidate(m,tstamp,mt,SHARED);
				found=true;
			}
	 	}
 		return MemState(0, found ? OWNER : MODIFIED);
	}

	// ms==(INVALID || NOT_FOUND) => BusRdX
	for(vector<Interface*>::iterator i=prevs.begin(); i!=prevs.end();++i) 
	{
		Interface *mi = *i;
		if(mi == who) 
		    continue;
		MOESI_state ms=mi->state(m,tstamp);
		if(ms.isValid())
			mi->invalidate(m,tstamp,mt,INVALID);
	}
	mt.add(bus,READ);
 	return next->read(m,tstamp,mt,ms);
}

MemState
FUTUREBUS::write(Interface* bus, Shared& next, Intf_vector& prevs, const Access& m,uint64_t tstamp,Trace& mt,MOESI_state ms)
{
	const Interface* who=mt.getMem();
	LOG("FUTUREBUS write",m.phys,who);
	//write_++;
 	for(vector<Interface*>::iterator i=prevs.begin(); i!=prevs.end();++i) 
	{
		Interface *mi = *i;
 		if(mi == who) 
		    continue;
		MOESI_state ms=mi->state(m,tstamp);
		if(ms.isValid()) 
		{
			LOG("FUTUREBUS_write at ", mi->Name(), " to INVALID ");
			mi->invalidate(m,tstamp,mt,INVALID); // do not write back
		}
 	}
	mt.add(bus,WRITE);
 	return next->write(m,tstamp,mt,ms);
}

}
}
