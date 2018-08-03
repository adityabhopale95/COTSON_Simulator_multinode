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

// $Id: moesi.cpp 11 2010-01-08 13:20:58Z paolofrb $

#include "abaeterno_config.h"
#include "moesi.h"
#include "logger.h"

using namespace std;
using namespace boost;

namespace Memory{
namespace Protocol{

MemState
MOESI::read(Interface* bus, Shared& next, Intf_vector& prevs, const Access& m,uint64_t tstamp,Trace& mt,MOESI_state ms)
{
	//L2 wanna read, but missed
	//fetch from caches or MM (same as WT cache), and move to SHARED or EXCLUSIVE 
	//snoops -> move copies to SHARED or OWNER, if MODIFIED (possibly copy back any MODIFIED copy)
	LOG("MOESI: read");		
	const Interface* who = mt.getMem();
	bool found=false;
 	for(vector<Interface*>::iterator i=prevs.begin(); i!=prevs.end();++i) 
	{
		Interface* mi = *i;
 		if(mi == who) 
		    continue;
		MOESI_state ms = mi->state(m,tstamp);
		if(ms.isValid())
		{
			found=true;
			if(ms==MODIFIED)
				mi->invalidate(m,tstamp,mt,OWNER); // do not write back
			else if(ms==EXCLUSIVE)
				mi->invalidate(m,tstamp,mt,SHARED); // write back, accounted for in the cache model
		}
 	}
 	
	if (!found) 
	{
	    mt.add(bus,READ);
	    return next->read(m,tstamp,mt,ms);
	}
	else 
	{
		snoop_read++;
		return MemState(0,SHARED);
	}
		
}

MemState
MOESI::readx(Interface* bus, Shared& next, Intf_vector& prevs, const Access& m,uint64_t tstamp,Trace& mt, MOESI_state ms)
{
	LOG("MOESI: readX from", ms);		
	// if lastMem has the line this is a hit, 
	// 	if MODIFIED or EXCLUSIVE, then no need for readX, just return MODIFIED 
	// 	if OWNER, readX to invalidate copies and return MODIFIED
	// 	if other: SHARED, look if there any copy to invalidate around, then move to MODIFIED
	// if NOT_FOUND or INVALID look for a copy to fetch (possibly in the memory) and possibly invalidate around, then move to MODIFIED
	
	const Interface* who = mt.getMem();
	if (ms==MODIFIED || ms==EXCLUSIVE)
 		return MemState(0,MODIFIED); // just tag change
	
	// go on with readX  
	bool found=false;
	if(ms==SHARED || ms==OWNER || ms==NOT_FOUND|| ms==INVALID)
	{
		//this is the readX
		for(vector<Interface*>::iterator i=prevs.begin(); i!=prevs.end();++i) 
		{
		    Interface* mi = *i;
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
		snoop_readx++;
 		return MemState(0,MODIFIED); 
	}
}

MemState
MOESI::write(Interface* bus, Shared& next, Intf_vector& prevs, const Access& m,uint64_t tstamp,Trace& mt,MOESI_state ms)
{
	LOG("MOESI: write");		
	//write_++;
	const Interface* who=mt.getMem();
	//bool found=false;
	
 	for(vector<Interface*>::iterator i=prevs.begin(); i!=prevs.end();++i)
 	{
		Interface* mi = *i;
 		if(mi == who) 
		    continue;
		MOESI_state ms = mi->state(m,tstamp);
		if(ms.isValid())
		{
			//found=true;
			mi->invalidate(m,tstamp,mt,INVALID); // do not write back
		}
			
 	}
	mt.add(bus,WRITE);
	return next->write(m,tstamp,mt,ms);
}

}
}
