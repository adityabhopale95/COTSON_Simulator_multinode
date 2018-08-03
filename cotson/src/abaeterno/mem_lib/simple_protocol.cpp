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

// $Id: simple_protocol.cpp 11 2010-01-08 13:20:58Z paolofrb $

#include "abaeterno_config.h"
#include "simple_protocol.h"
#include "logger.h"
#include "error.h"

using namespace std;

namespace Memory{
namespace Protocol{

MemState
SIMPLE::read(Interface* bus, Shared& next,  Intf_vector& prevs,const Access& m,uint64_t tstamp,Trace& mt,MOESI_state ms)
{
	//L2 wanna read, but missed
	//fetch from caches or MM (same as WT cache), and move to SHARED
	const Interface* who=mt.getMem();
	LOG("SIMPLE read",m.phys,who);
	bool found=false;
	for(vector<Interface*>::iterator i=prevs.begin(); i!=prevs.end() && !found;++i) 
	{
		Interface *mi = *i;
 		if(mi == who) 
		    continue;
		MOESI_state ms = mi->state(m,tstamp);
		if(ms.isValid())
			found=true;
 	}
	
	if (found) 
	{
		LOG("SIMPLE read found -> latency: 0 new_moesi: SHARED");
 		return MemState(0,SHARED);
	}
	else
	{
		mt.add(bus,READ);
		// in Simple we have only SHARED and INVALID
		uint32_t new_latency = next->read(m,tstamp,mt,ms).latency();
		LOG("SIMPLE read found -> latency:", new_latency,"new_moesi:",SHARED);
		return MemState(new_latency,SHARED);
	}
}

MemState
SIMPLE::readx(Interface* bus, Shared& next, Intf_vector& prevs,const Access& m,uint64_t tstamp,Trace& mt, MOESI_state ms)
{
	LOG("SIMPLE readx",m.phys);
	ERROR("Simple must never see a Simple::readx()");
	return MemState(0,MODIFIED);
}

MemState
SIMPLE::write(Interface* bus, Shared& next, Intf_vector& prevs ,const Access& m,uint64_t tstamp,Trace& mt,MOESI_state ms)
{
	//invalidate all copies (including me) and write in the upper level
	for(vector<Interface*>::iterator i=prevs.begin(); i!=prevs.end();++i) 
	{
		Interface* mi = *i;
		if (mi->state(m,tstamp).isValid())
			mi->invalidate(m,tstamp,mt,INVALID);
	}
	mt.add(bus,WRITE);
 	return next->write(m,tstamp,mt,ms);
}

}
}
