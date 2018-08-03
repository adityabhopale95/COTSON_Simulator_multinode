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

#include "machine.h"
#include "logger.h"
#include "error.h"
#include "cpu.h"
#include "disk.h"
#include "nic.h"
#include "cotson.h"
#include "token_queue.h"
#include "token_parser.h"

using namespace std;
using namespace boost;

Machine::Machine() {}
Machine::~Machine() 
{
	for(Disks::iterator i=disks_.begin();i!=disks_.end();++i)
		delete i->second;
	for(Nics::iterator i=nics_.begin();i!=nics_.end();++i)
		delete i->second;
	for(Cpus::iterator i=cpus_.begin();i!=cpus_.end();++i)
		delete i->second;
}

void Machine::build()
{
	queues=Cotson::queues();
	for(Queues::iterator i=queues.begin();i!=queues.end();++i)
	{
		TokenQueue* q=*i;
		uint64_t devid = q->devid;
		uint64_t flags = q->flags;

		if (flags & TOKEN_QUEUE_CPU)
		{
			LOG("CPU  queue:",hex,q,"deviceID",hex,devid,"flags",flags);
			ERROR_IF(cpus_.find(devid)!=cpus_.end(),"Duplicate cpu");
			ERROR_IF(q->type!=Linear,"CPU queue must be linear");
			Cpu* cpu=new Cpu(q,devid,"cpu");
			cpus_[devid]=cpu;
			add(cpu->name()+'.',*cpu);
		}
		else if (flags & TOKEN_QUEUE_DISK)
		{
			LOG("disk queue:",hex,q,"deviceID",hex,devid,"flags",flags);
			ERROR_IF(disks_.find(devid)!=disks_.end(),"Duplicate disk");
			ERROR_IF(q->type!=Circular,"Disk queue must be circular");
			Disk* disk=new Disk(q,devid,"disk");
			disks_[devid]=disk;
			add(disk->name()+'.',*disk);
		}
		else if(flags & TOKEN_QUEUE_NIC)
		{
			LOG("net  queue:",hex,q,"deviceID",hex,devid,"flags",flags);
			ERROR_IF(nics_.find(devid)!=nics_.end(),"Duplicate nic");
			ERROR_IF(q->type!=Circular,"Nic queue must be circular");
			Nic* nic=new Nic(q,devid,"nic");
			nics_[devid]=nic;
			add(nic->name()+'.',*nic);
		}
		else
			ERROR("Unknown queue flags");
	}

 	Option::run_function("build");
}

void Machine::inject(uint64_t devid,InjectState state)
{
	Cpus::iterator i=cpus_.find(devid);
	ERROR_IF(i==cpus_.end(),"Cannot find cpu");
	i->second->inject(state);
}

void Machine::tag(uint64_t devid,uint32_t t,const Cotson::Inject::info_tag& ti)
{
	Cpus::iterator i=cpus_.find(devid);
	ERROR_IF(i==cpus_.end(),"Cannot find cpu");
	i->second->tag(t,ti);
}

bool Machine::execute(uint64_t devid,uint32_t t)
{
	Cpus::iterator i=cpus_.find(devid);
	ERROR_IF(i==cpus_.end(),"Cannot find cpu");
	return i->second->execute(t);
}

uint32_t Machine::nic_read(uint64_t devid,uint64_t length,void* data,bool dma)
{
	Nics::iterator j=nics_.find(devid);
	ERROR_IF(j==nics_.end(),"cannot find nic");
	return j->second->read(length,data,dma);
}

uint32_t Machine::nic_write(uint64_t devid,uint64_t length,void* data,bool dma)
{
	Nics::iterator j=nics_.find(devid);
	ERROR_IF(j==nics_.end(),"cannot find nic");
	return j->second->write(length,data,dma);
}

uint32_t Machine::nic_end(uint64_t devid)
{
	Nics::iterator j=nics_.find(devid);
	ERROR_IF(j==nics_.end(),"cannot find nic");
	return j->second->end();
}

uint32_t Machine::disk_read(uint64_t devid,uint64_t block_number,
	uint64_t block_count,bool dma)
{
	Disks::iterator j=disks_.find(devid);
	ERROR_IF(j==disks_.end(),"cannot find disk");
	return j->second->read(block_number,block_count,dma);
}

uint32_t Machine::disk_write(uint64_t devid,uint64_t block_number,
	uint64_t block_count,bool dma)
{
	Disks::iterator j=disks_.find(devid);
	ERROR_IF(j==disks_.end(),"cannot find disk");
	return j->second->write(block_number,block_count,dma);
}

uint32_t Machine::disk_end(uint64_t devid)
{
	Disks::iterator j=disks_.find(devid);
	ERROR_IF(j==disks_.end(),"cannot find disk");
	return j->second->end();
}

int Machine::cpus()
{
	return get().cpus_.size();
}

int Machine::disks()
{
	return get().disks_.size();
}

int Machine::nics()
{
	return get().nics_.size();
}

Cpu* Machine::cpu(int i)
{
	if(i>=cpus() || i<0)
		throw std::invalid_argument("incorrect index "+
			boost::lexical_cast<std::string>(i) + 
			" passed to get_cpu. Cpus are indexed from 0 to cpus()-1");

	return get().cpus_[i];
}

Disk* Machine::disk(int i)
{
	if(i>=disks() || i<0)
		throw std::invalid_argument("incorrect index "+
			boost::lexical_cast<std::string>(i) + 
			" passed to get_disk. Disks are indexed from 0 to disks()-1");
	Disks::iterator j=get().disks_.begin();
	for(;i;++j,--i) {}
	return j->second;
}

Nic* Machine::nic(int i)
{
	if(i>=nics() || i<0)
		throw std::invalid_argument("incorrect index "+
			boost::lexical_cast<std::string>(i) + 
			" passed to get_nic. Nics are indexed from 0 to nics()-1");

	Nics::iterator j=get().nics_.begin();
	for(;i;++j,--i) {}
	return j->second;
}

void Machine::empty_queue(TokenQueue*tq) 
{
	TokenParser* parser=reinterpret_cast<TokenParser*>(tq->anything);
	ERROR_IF(parser==0,"No parser for this queue");

	try 
	{
		parser->run();
	}
	catch(runtime_error& e) 
	{
		Cotson::exit();
		ERROR("(emptyQueue) caught an exception: ", Cotson::nanos(), " ", e.what());
	}
	catch(bad_alloc& e)
	{
		Cotson::exit();
		ERROR("(emptyQueue) caught a bad alloc: ", Cotson::nanos(), " ", e.what());
	}
	catch(...) 
	{
		Cotson::exit();
		ERROR("(emptyQueue) caught an unknown exception: ", Cotson::nanos());
	}
}

void Machine::empty_queues()
{
	for(Queues::iterator i=queues.begin();i!=queues.end();++i) 
		empty_queue(*i);
}
