// (C) Copyright 2006 Hewlett-Packard Development Company, L.P.  HP
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// $Id: twolev.h 11 2010-01-08 13:20:58Z paolofrb $
#ifndef TWOLEV_H_INCLUDED 
#define TWOLEV_H_INCLUDED

#include "cache.h"
#include "libmetric.h"
#include "error.h"

struct TwolevUpdate
{
	uint64_t addr;
	bool pred;
	char *count;
};

class twolevT : public metric
{
public:
	twolevT(uint32_t, uint32_t, uint32_t, bool);

	void ResetHistory();

    inline bool Lookup(uint64_t addr,TwolevUpdate& up,bool real_taken)
    {
	    up.addr = addr;
	    up.count = Get2bitCounter (addr);
	    up.pred = *(up.count) > 1;
    
	    if (up.pred!=real_taken)
		        pred_miss++;
	    lookup++;
	    return up.pred!=real_taken;
    }

    inline void Update(uint64_t addr, uint32_t taken, char *counter)
    {
    	update++;
    
    	ERROR_IF(!counter,"No counter!");
    	char cv = *counter;
    	*counter = taken ? (cv<3 ? cv+1 : cv) : (cv>0 ? cv-1 : cv);
    	updateEntry(update_bhr[addr],taken);
    }

	inline void SpecUpdate(uint64_t addr, bool taken)
	{
	    updateEntry(lookup_bhr[addr],taken);
	}

	inline char* Get2bitCounter(uint64_t addr)
	{
	    uint32_t index=lookup_bhr[addr];
	    if (use_xor) 
		    index = (index ^ addr) & ((1 << hlength) - 1);
	    index = (addr << hlength) | index;
	    return (&pht[index]);
	}
		
private:
	const uint32_t hlength;
	const bool use_xor;

	inline void updateEntry(uint64_t& entry,bool taken)
	{
	    entry = ((entry << 1) | (uint32_t)taken) & ((1 << hlength) - 1); 
	}

	Memory::Cache<uint64_t> lookup_bhr;
	Memory::Cache<uint64_t> update_bhr;
	Memory::Cache<char>     pht;

	uint64_t lookup;
	uint64_t pred_miss;
	uint64_t update;
	uint64_t reset;
};

#endif
