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

#include "hybrid_memory.h"

using namespace std;
using namespace boost;

namespace Memory {

//registerClass<Interface,Hybrid> hybrid_memory_c("hybrid_memory");

Hybrid::Hybrid(Parameters& p) 
{
	name=p.get<string>("name");
	
	hit_id=0;

	linesize_=p.get<uint32_t>("linesize","64",false);
	dram_latency=p.get<uint32_t>("latency");
	dram_size=p.get<uint32_t>("dram_size");
	
	flash_read_latency=p.get<uint32_t>("flash_rlatency");
	flash_write_latency=p.get<uint32_t>("flash_wlatency");
	page_size=p.get<uint32_t>("flash_pagesize");
	pages_per_block=p.get<uint32_t>("flash_pages_per_block");
	
	//blocks=p.get<uint32_t>("flash_blocks");
	//flash_size = blocks * pages_per_block * page_size;
	
	flash_size=p.get<uint64_t>("flash_size");
	if ( flash_size%(pages_per_block * page_size) )
	{
		throw runtime_error("Flash size must be a multiple of its block size (pages_per_block * page_size) for memory object '"+name+"'.");
	}
	blocks=flash_size/(pages_per_block * page_size);
		
	add("read", read_);
	add("write", write_);
	add("access", access_);
	add("flash_write", flash_write_);
	add("flash_read", flash_read_);
	add("dram_miss", miss_);
	add("dram_hit", hit_); 
	add("outbounds_read", outbounds_read_);
	add("outbounds_write", outbounds_write_);
	
	add("pages_used", pages_used);
	
	add_ratio("dram_miss_ratio", "dram_miss", "access"); 

	page_accesses.reset(new uint32_t[flash_size/page_size]);
	for(uint i=0;i<flash_size/page_size;i++)
		page_accesses[i]=0;
	
//	cout << "Hybrid memory constructor called (dram latency = " << dram_latency << ", flash read = " 
//		 << flash_read_latency << ", Flash write = " << flash_write_latency << " )" << endl <<
//		 " flash_size=" << flash_size << " blocks=" << blocks << endl << endl;
	
	clear_metrics();

}

MemState Hybrid::read(const Access& m,uint64_t tstamp,Trace& mt,MOESI_state ms)
{
	uint32_t latency;
	hit_id++;
	//cout << "READ ";
	
	latency = get_latency(m, tstamp, false);
	
//	cout << "r Access to " << (m.phys % flash_size) / page_size << " - Physical: " << m.phys << " Tstamp: " << tstamp << " Trace: " << mt.getType() << endl;	
	
	read_++;
	
	access_++;
	
	if(sim_state()==SIMULATION)
	{
		if (page_accesses[(m.phys % flash_size)/page_size] == 0)
		{
			pages_used ++;
		}
		page_accesses[(m.phys % flash_size)/page_size]++;
	}
		

	if(m.phys>flash_size)
	{
/*		
 		cout<< "Warning: Memory access (read) "
		    <<"out of bounds. Access to " <<m.phys
			<< " but mem size is " << flash_size << "." << endl;
*/
		outbounds_read_++;
	}
    return MemState(latency,EXCLUSIVE); 
}

MemState Hybrid::readx(const Access& m,uint64_t tstamp,Trace& mt,MOESI_state ms)
{
	if (ms.isValid())
		return MemState(0,MODIFIED); 
	mt.add(this,READ);
	return read(m,tstamp,mt,ms);
}

MemState Hybrid::write(const Access& m,uint64_t tstamp,Trace& mt,MOESI_state ms)
{
	uint32_t latency;

	
	//cout << "WRITE ";
	hit_id++;

	latency = get_latency(m, tstamp, true);

//	cout << "w Access to " << (m.phys % flash_size) / page_size << " - Physical: " << m.phys << " Tstamp: " << tstamp << " Trace: " << mt.getType() << endl; 

	write_++;
	
	access_++;
	
	if(sim_state()==SIMULATION)
	{
		if (page_accesses[(m.phys % flash_size)/page_size] == 0)
		{
			pages_used ++;
		}
		page_accesses[(m.phys % flash_size)/page_size]++;
	}

	if(m.phys>flash_size)
	{
/*		
 		cout<< "Warning: Memory access (write) "
		    <<"out of bounds. Access to " <<m.phys
			<< " but mem size is " << flash_size << "." << endl;
*/
		outbounds_write_++;
	}
	
	return MemState(latency,MODIFIED);
}
	
MOESI_state Hybrid::state(const Access&,uint64_t)
{
	throw runtime_error("Hybrid should not answer to state");
	return INVALID;
}

void Hybrid::invalidate(const Access&,uint64_t tstamp,const Trace&,MOESI_state)
{
	throw runtime_error("Hybrid should not answer to invalidate");
}

Hybrid::~Hybrid()
{
}

}
