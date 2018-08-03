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

// #define _DEBUG_THIS_

#include "abaeterno_config.h"

#include "nic.h"
#include "nic_token_parser.h"
#include "logger.h"
#include "error.h"
#include "cotson.h"
#include "network_timing.h"


using namespace std;
using namespace boost;
using namespace boost::assign;

Nic::Nic(TokenQueue*q,uint64_t d,const string& n): 
	devid(d),devname(n+lexical_cast<string>(d))
{
	parser.reset(new NicTokenParser(q));
	LOG("set nic events",hex,devid,dec);
	Cotson::Nic::Tokens::on(devid); 
}

Nic::~Nic() { }

void Nic::timer(luabind::object table)
{
	ERROR_IF(timing.get(),"already defined a timer for this NIC");
	set<string> required;
	required += "name";
	
	Parameters p=Option::from_lua_table(table,required,"NicTimer");
	p.track();

	NicTimer*np;
	timing.reset(np=Factory<NicTimer>::create(p));
	TaggedMetrics tm=TaggedMetrics::get();
	tm.add(np,p.get<string>("name"));
	tm.add(np,"nic");
	if(p.has("tag"))
		tm.add(np,p.get<string>("tag"));

	add("timer.",*timing);
}

uint32_t Nic::read(uint64_t length,void* data,bool dma)
{
	NetworkTiming* network_timing(NetworkTiming::get());

	ERROR_IF(!timing.get(),"Timer not defined for this NIC");
	uint32_t net_delay = network_timing ? network_timing->net_delay(): 0;
	uint32_t delay =timing->accept(dma,true,length,data) + net_delay;
 	LOG("network read timing, devid=", devid, "delay=", delay,"netdelay",net_delay);
	return delay;
}

uint32_t Nic::write(uint64_t length,void* data,bool dma)
{
	NetworkTiming* network_timing(NetworkTiming::get());

	ERROR_IF(!timing.get(),"Timer not defined for this NIC");
	uint32_t net_delay = network_timing ? network_timing->net_delay(): 0;
	uint32_t delay=timing->accept(dma,false,length,data)+net_delay;
 	LOG("network write timing, devid=", devid, "delay=", delay,"netdelay",net_delay);
	return delay;
}

uint32_t Nic::end() 
{
	NetworkTiming* network_timing(NetworkTiming::get());
	
	if(network_timing)
		network_timing->send_packet_delay();
	return 0;
}

