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
#ifndef BTB_H_INCLUDED
#define BTB_H_INCLUDED

#include "cache.h"
#include "bb.h"
#include "libmetric.h"

class btbT : public metric
{	
	public:
	btbT(uint32_t entries, uint32_t sets);

	bool Lookup(uint64_t vaddr,BB& up);	
	void Update(const BB& up);
	void Predecode(const BB& up);
		
	private:
	struct Entry 
	{
		BB bb;
		uint64_t LRU;
		bool predecoded; 
		Entry(): LRU(0), predecoded(0) {}
	};

	boost::scoped_ptr<Memory::Cache< std::vector<Entry> > > cache;

	uint64_t lookup;
	uint64_t update;
	uint64_t miss;
	uint64_t pseudohit;
		
	uint64_t btb_cycle;
};

#endif
