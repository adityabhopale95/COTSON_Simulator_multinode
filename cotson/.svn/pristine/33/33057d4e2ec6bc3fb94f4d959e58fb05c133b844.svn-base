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
#ifndef HYBRID_MEMORY_MAP_H
#define HYBRID_MEMORY_MAP_H

#include "memory_interface.h"
#include "liboptions.h"
#include "cache.h"
#include "hybrid_memory.h"
//#include <map>

namespace Memory {

class MapPage
{
	public:
	MapPage(uint64_t dram, uint64_t timestamp, uint64_t flash) 
	    : dram_id(dram), flash_id(flash), tstamp(timestamp) 
	{};
	MapPage() : dram_id(0), flash_id(0), tstamp(0)
	{};
	
	uint64_t dram_id;
	uint64_t flash_id;
	uint64_t tstamp;
	//uint32_t hits;
};

typedef std::map<uint64_t, MapPage *> PageMap;

class HybridMap : public Hybrid
{   
    public:
    HybridMap(Parameters&);

    ~HybridMap();

    private:

    uint32_t get_latency(const Access& m, uint64_t tstamp, bool write);
    MapPage * free_drampage(uint64_t &latency);
    struct Hash 
    { 
    	size_t operator()(uint64_t x) const { return x ^ ((x >> 32) << 20); }
    };
    
    
    //PageMap::iterator get_mintstamp(PageMap pmap);
    void show(PageMap pmap);

    uint64_t get_pageid(uint64_t address);
    
    std::vector<MapPage *> free_pages;
    std::vector<MapPage> pages;
    PageMap read_pages;
    PageMap written_pages;
    PageMap timestamp_read;
    PageMap timestamp_write;
	enum policy 
	{
		READ, 
		LRU, 
		PREF_READ
	};

    int policy;
    float pref_read_ratio;
}; 

}
#endif
