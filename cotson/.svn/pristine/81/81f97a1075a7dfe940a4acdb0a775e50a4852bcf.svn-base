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

#include "hybrid_memory_map.h"

using namespace std;
using namespace boost;

namespace Memory {

registerClass<Interface,HybridMap> hybrid_memory_map_c("hybrid_memory_map");

HybridMap::HybridMap(Parameters& p) : Hybrid(p) 
{
	uint64_t page_count = dram_size/page_size;
	string Strategy;
	
	Strategy=p.get<string>("strategy", "ReadFirst");

	policy=READ;
	
	if (!Strategy.compare("ReadFirst"))
	{
		cout <<  "Hybrid flash map with ReadFirst strategy." << endl;
		policy=READ;
	}
	if (!Strategy.compare("LRU"))
	{
		cout <<  "Hybrid flash map with LRU strategy." << endl;
		policy=LRU;
	}
	if (!Strategy.compare("PrefRead"))
	{
		policy=PREF_READ;
		pref_read_ratio= ((float) p.get<long>("prefread_ratio","85"))/100;
		cout <<  "Hybrid flash map with PrefRead " << pref_read_ratio << " strategy." << endl;
	}

	MapPage *mpage;
	
	for(uint64_t i=0;i<page_count;i++)
	{
		mpage=new MapPage(i,0,0);
		pages.push_back(*mpage);
		free_pages.push_back(mpage);
	}

}

uint32_t HybridMap::get_latency(const Access& m,  uint64_t tstamp, bool write)
{
	uint64_t page_id;
	MapPage *page;
	
	page_id = get_pageid(m.phys);
	
	if (read_pages.find(page_id) == read_pages.end())
	{
		if (written_pages.find(page_id) == written_pages.end())
		{
			//Not in DRAM: DRAM fault!
			uint64_t latency=dram_latency;

			miss_++;
			flash_read_++;

			page = free_drampage(latency);
			
			page->tstamp = hit_id;
			page->flash_id=page_id;
			
			if (write)
			{
				written_pages[page_id] = page;
				timestamp_write[hit_id] = page;
			}
			else
			{
				read_pages[page_id] = page;
				timestamp_read[hit_id] = page;
			}
			
			return(latency);
		}
		else
		{
			//In written pages: Update timestamp.
			page=written_pages[page_id];
			
			timestamp_write.erase(page->tstamp);
			timestamp_write[hit_id]=page;
			page->tstamp=hit_id;
		}
	}
	else
	{
		//In read pages
		page=read_pages[page_id];
		if(write)
		{
			//Move to written
			written_pages[page_id]=read_pages[page_id];
			read_pages.erase(page_id);
			
			timestamp_read.erase(page->tstamp);
			timestamp_write[hit_id]=page;
			
			
			page->tstamp=hit_id;
		}
		else
		{
			//Update tstamp
			timestamp_read.erase(page->tstamp);
			timestamp_read[hit_id]=page;
		}
		page->tstamp=hit_id;
	}
	
	hit_++;

	return(dram_latency);
}

MapPage * HybridMap::free_drampage(uint64_t &latency)
{
	MapPage *page;
	
	if (!free_pages.empty())
	{
		page = free_pages.back();
		free_pages.pop_back();

		latency = flash_read_latency;
		return(page);
	}
	
	PageMap::iterator min_read;
	PageMap::iterator min_write;
	bool take_out_from_read=true;
	
	min_read = timestamp_read.begin();//get_mintstamp(read_pages);
	
	//DRAM full
	switch (policy)
	{
		case READ:
			//Just free a written page if there is none in read pages
			if(!read_pages.empty())
			{
				take_out_from_read=true;
			}
			else
			{
				if(!written_pages.empty())
				{
					min_write=timestamp_write.begin();//get_mintstamp(written_pages);
	
					take_out_from_read=false;
				}
				else
				{
					//This should never occur!!!
					cout<<"error: no pages found in any list for hybrid map memory!"<<endl;
					throw runtime_error("error: no pages found in any list for hybrid map memory!");
				}
			}
			break;
		case PREF_READ:
			if(written_pages.size() < pref_read_ratio * dram_size/page_size)
			{
				take_out_from_read=true;
				break;
			}
			//Do not put "break;" statement here! 
		case LRU:
			min_write=timestamp_write.begin();//get_mintstamp(written_pages);

			if(!read_pages.empty())
			{
				if(!written_pages.empty())
				{
					if(min_read->first <= min_write->first)
					{
						take_out_from_read=true;
					}
					else
					{
						take_out_from_read=false;
					}
				}
				else
				{
					take_out_from_read=true;
				}
			}
			else
			{
				take_out_from_read=false;
			}
			break;
			
	}
	
	if(take_out_from_read)
	{
		page=min_read->second;
		
		timestamp_read.erase(page->tstamp);
		
		read_pages.erase(page->flash_id);
		
		latency = flash_read_latency;
		return(page);
	}

	//Take out from write
	page=min_write->second;
	
	timestamp_write.erase(page->tstamp);
	
	written_pages.erase(page->flash_id);
	
	latency = flash_write_latency;
	flash_write_++;
	return(page);
}

void HybridMap::show(PageMap pmap)
{
	PageMap::iterator iter, min;
	
	min = pmap.begin();
	
	cout << "[";
	for(iter = pmap.begin(); 
	        iter != pmap.end();
	        iter++)
	{
		cout<< (*iter).first << ",";
	}
	cout <<"]" << endl;
}

uint64_t HybridMap::get_pageid(uint64_t address)
{
	return(address & (~(page_size-1)));
}

HybridMap::~HybridMap()
{
}

}
