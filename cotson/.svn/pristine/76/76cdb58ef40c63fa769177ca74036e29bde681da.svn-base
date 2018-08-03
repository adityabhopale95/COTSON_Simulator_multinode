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
#include "mesi.h"
#include "logger.h"

using namespace std;

namespace Memory{
namespace Protocol{

MemState 
MESI::read(Interface* bus, Shared& next,  Intf_vector& prevs,const Access& m,uint64_t tstamp,Trace& mt,MOESI_state ms)
{
	//L2 wanna read, but missed
	//fetch from caches or MM (same as WT cache), and move to SHARED or EXCLUSIVE
	//snoops -> move copies to SHARED (possibly copy back any MODIFIED copy)
	//read_++;
	LOG("MESI: read", m.phys);		
	const Interface* who=mt.getMem();
	bool found=false;
	for(vector<Interface*>::iterator i=prevs.begin(); i!=prevs.end();++i) 
	{
		Interface *mi = *i;
 		if(mi == who) 
		    continue;
		MOESI_state ms = mi->state(m,tstamp);
		if(ms.isValid()) 
		{
			mi->invalidate(m,tstamp,mt,SHARED);
			found=true;
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
MESI::readx(Interface* bus, Shared& next, Intf_vector& prevs,const Access& m,uint64_t tstamp,Trace& mt, MOESI_state ms)
{
	// if lastMem has the line this is a hit, 
	// 	if MODIFIED or EXCLUSIVE, then no need for readX, just return MODIFIED 
	// 	if other: SHARED, look if there any copy to invalidate around, and return MODIFIED
	// if NOT_FOUND or INVALID look for a copy to fetch (possibly in the memory) and possibly invalidate around, and return MODIFIED
	// INVALID should never show up here
				
	LOG("MESI: readX from", ms);
			
	if (ms==MODIFIED || ms==EXCLUSIVE)
		//I have the only copy => No need to worry about others
 		return MemState(0,MODIFIED); // just tag change

	const Interface* who=mt.getMem();
	bool found=false;
	if(ms==SHARED || ms==NOT_FOUND || ms==INVALID) 
	{
		//this is the readX
		for(vector<Interface*>::iterator i=prevs.begin(); i!=prevs.end();++i) 
		{
		    Interface *mi = *i;
			if(mi == who) 
			    continue;
			MOESI_state ms=mi->state(m,tstamp);
			if(ms.isValid())
			{
				mi->invalidate(m,tstamp,mt,INVALID); // check cache def. as above
				found=true;
			}
		}
	}

	LOG("MESI: readX found=", found ? "true" : "false");
 	if (!found) 
	{
		// Read from cache level above bus
	    mt.add(bus,READ);
		return (MemState(next->read(m,tstamp,mt,ms).latency(), MODIFIED));
    } 
	else 
	{
		// Snoop from another cache 
		// FIXME: Add read latency from another cache + bus latency
 		if (ms==INVALID || ms==NOT_FOUND || ms==EXCLUSIVE)
			return MemState(0,MODIFIED); 
		else	
			return MemState(0,EXCLUSIVE); 
	}
}

MemState
MESI::write(Interface* bus, Shared& next, Intf_vector& prevs ,const Access& m,uint64_t tstamp,Trace& mt,MOESI_state ms)
{
	bool found=false;
	const Interface* who=mt.getMem();
 	for(vector<Interface*>::iterator i=prevs.begin(); i!=prevs.end();++i) 
	{
		Interface *mi = *i;
 		if(mi == who) 
		    continue;
		MOESI_state ms=mi->state(m,tstamp);
		if(ms.isValid()) 
		{
			mi->invalidate(m,tstamp,mt,INVALID); // do not write back
			found=true;
		}
 	}
	
	mt.add(bus,WRITE);
	const MemState& nstate = next->write(m,tstamp,mt,ms);
	// Normally, it would go straight from SHARED to MODIFIED but
	// MESI is an exception. A write hit in a SHARED goes to EXCLUSIVE
	if (found && ms==SHARED && nstate.moesi()==MODIFIED)
	    return MemState(nstate.latency(),EXCLUSIVE);

	return nstate;
}

}
}
