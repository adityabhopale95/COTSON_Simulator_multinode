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

// $Id: hybrid_memory_cache.cpp 11 2010-01-08 13:20:58Z paolofrb $
#include "abaeterno_config.h"

#include "hybrid_memory_cache.h"

using namespace std;
using namespace boost;

namespace Memory {

registerClass<Interface,HybridCache> hybrid_memory_cache_c("hybrid_memory_cache");

HybridCache::HybridCache(Parameters& p) : Hybrid(p)
{
	cache.reset(new Memory::Cache<CachePage>( dram_size, page_size));
}

uint32_t HybridCache::get_latency(const Access& m, uint64_t tstamp, bool write)
{
	uint64_t relative = m.phys % flash_size;
	uint32_t latency;

	CachePage cache_page =  (*cache)[relative];
	
	if( cache->group_id(relative) == cache_page.group_id)
	{
		hit_ ++;
		latency = dram_latency;
	}
	else
	{
		miss_++;
		flash_read_++;
		
		if( cache_page.modified )
		{
			latency = flash_write_latency;
			flash_write_++;
		}
		else
		{
			latency = flash_read_latency;
		}
		
		cache_page.modified = false;
		cache_page.group_id = cache->group_id(relative);
	}
	
	if(write)
	{
		cache_page.modified = true;
	}
	
	(*cache)[relative] = cache_page;

	return(latency);
}


HybridCache::~HybridCache()
{
/*	uint32_t tot_access=0, most_accessed=0, highest_access=0, pages_used=0; 
	uint32_t pages = flash_size/page_size;
	
	
	for(uint32_t i=0;i < pages; i++)
	{
		if(page_access[i])
		{
			pages_used++;
			tot_access+=page_access_count[i];
			if(page_access_count[i]>highest_access)
			{
				highest_access=page_access_count[i];
				most_accessed=i;
			}
		}
	}
	
	cout << endl;
	
	cout << "Hybrid Cache: Pages usage statistics" << endl;
	cout << "   Memory hits: " << tot_access << " Hit/Miss: " << hit_ << " / " << miss_  
		 << "   Miss Ratio: " << ((float) miss_ *100)/(hit_+miss_) << " % " << endl;
	cout << "   Flash Read/Write: " << flash_read_ << " / " << flash_write_ << endl;
	cout << "   Pages Used / Total: " << pages_used << " ( "  << pages_used * page_size / 1024 << "KB ) / " << pages << " ( "  << pages * page_size / 1024 << "KB ) [ " << ((float)pages_used * 100)/pages <<" % ]" << endl;
	cout << "   Most accessed page: " << most_accessed <<  " with " << highest_access << " hits." << endl;
	cout << "   DRAM / Flash sizes: " << dram_size << " / " << flash_size << endl;
	cout << endl;
*/	
}

}
