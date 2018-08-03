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
#include "btb.h"
#include "logger.h"
#include "error.h"

using namespace std;

btbT::btbT(uint32_t entries, uint32_t sets) : 
	btb_cycle(0)  
{
	cache.reset(new Memory::Cache<vector<Entry> >(entries/sets,1));
	cache->init(vector<Entry>(sets));

	add("lookup",lookup);
	add("update",update);
	add("miss",miss);
	add("pseudohit",pseudohit);
	add_ratio("miss_rate","miss","lookup");
	clear_metrics();
}

bool btbT::Lookup (uint64_t addr,BB& bb)
{
	lookup++;

	if (!addr) return false;

	vector<Entry>& v=(*cache)[addr];
	const uint32_t n_sets=v.size();

	for(uint32_t set = 0; set < n_sets; set++) 
	{
		if(v[set].bb.start() == addr) 
		{
			bb=v[set].bb;
			v[set].LRU = btb_cycle++;
			if(v[set].predecoded) pseudohit++;
			return true;
		}
	}

	bb.reset(addr);
	miss++;

	return 0;
}

void btbT::Update(const BB& bb)
{
	update++;

	vector<Entry>& v=(*cache)[bb.start()];
	const uint32_t n_sets=v.size();

	uint32_t lru = 0;
	for (uint32_t set = 0; set < n_sets; set++) 
	{
		if(v[set].bb.start()==bb.start()) 
		{
			v[set].LRU = btb_cycle++;
			v[set].predecoded=true;
			v[set].bb=bb;
			return;
		}
		if(v[set].LRU<v[lru].LRU)
			lru=set;
	}

	v[lru].bb=bb;
	v[lru].predecoded=true;
	v[lru].LRU=btb_cycle++;
}

void btbT::Predecode(const BB& bb)
{
	vector<Entry>& v=(*cache)[bb.start()];
	const uint32_t n_sets=v.size();

	uint32_t lru = 0;
	for (uint32_t set = 0; set < n_sets; set++) 
	{
		if(v[set].bb.start()==bb.start()) 
		{
			ERROR_IF(v[lru].predecoded!=true,"BTB entry not predecoded");
			return;
		}
		if(v[set].LRU<v[lru].LRU)
			lru=set;
	}

	v[lru].bb=bb;
	v[lru].predecoded=true;
	v[lru].LRU=btb_cycle++;
}
