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

#include "disk.h"
#include "disk_token_parser.h"
#include "logger.h"
#include "error.h"
#include "cotson.h"

using namespace std;
using namespace boost;
using namespace boost::assign;

Disk::Disk(TokenQueue*q,uint64_t d,const string& n): 
	devid(d),devname(n+lexical_cast<string>(d>>32)),
	devnum(d>>32),
	devclass(d&0xFFFFFFFF)
{
	parser.reset(new DiskTokenParser(q));
	LOG("set disk events",hex,devid);
	Cotson::Disk::Tokens::on(devid);
}

Disk::~Disk() {}

void Disk::timer(luabind::object table)
{
	ERROR_IF(timing.get(),"already defined a timer for this disk");

	set<string> required;
	required += "name";
	
	Parameters p=Option::from_lua_table(table,required,"DiskTimer");
	p.track();

	DiskTimer* dp;
	timing.reset(dp=Factory<DiskTimer>::create(p));
	TaggedMetrics tm=TaggedMetrics::get();
	tm.add(dp,p.get<string>("name"));
	tm.add(dp,"disk");
	if(p.has("tag"))
		tm.add(dp,p.get<string>("tag"));

	add("timer.",*timing);

	parser->setCapacity(bind(&DiskTimer::setCapacity,timing.get(),_1));
	parser->setBlocksize(bind(&DiskTimer::setBlocksize,timing.get(),_1));
	timing->disk_id=devnum;
}

uint32_t Disk::read(uint64_t block_number,uint64_t block_count,bool dma) 
{
	ERROR_IF(!timing.get(),"timer not defined for this disk");
	uint32_t delay=timing->accept(dma,true,block_number,block_count);
	//delay is measured in microseconds (us)
 	LOG("blockdev read timing, devid=", devid, "delay=", delay);
	return delay;
}

uint32_t Disk::write(uint64_t block_number,uint64_t block_count,bool dma)
{
	ERROR_IF(!timing.get(),"timer not defined for this disk");
	uint32_t delay=timing->accept(dma,false,block_number,block_count);
	//delay is measured in microseconds (us)
 	LOG("blockdev write timing, devid=", devid, "delay=", delay);
	return delay;
}

uint32_t Disk::end() { return 0; }
