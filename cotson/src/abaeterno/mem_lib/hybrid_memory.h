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

// $Id: hybrid_memory.h 721 2015-06-03 02:29:33Z paolofrb $
#ifndef HYBRID_MEMORY_H
#define HYBRID_MEMORY_H

#include "memory_interface.h"
#include "liboptions.h"

namespace Memory {

class Hybrid : public Interface
{   
    public:
    Hybrid(Parameters&);

    MemState read(const Access&,uint64_t,Trace&,MOESI_state);
    MemState readx(const Access&,uint64_t,Trace&,MOESI_state);
	MemState write(const Access&,uint64_t,Trace&,MOESI_state);
    MOESI_state state(const Access&,uint64_t);
    void invalidate(const Access&,uint64_t,const Trace&,MOESI_state);
    
    ~Hybrid();

	uint32_t item_size() const { return linesize_; }

    private:
    uint64_t read_;  
    uint64_t write_; 
    uint64_t access_;
    virtual uint32_t get_latency(const Access& m, uint64_t tstamp, bool write)=0;

    protected:
    uint64_t hit_;
    uint64_t miss_;
    uint64_t flash_write_;
    uint64_t flash_read_;
    uint64_t outbounds_read_;
    uint64_t outbounds_write_;

    uint32_t dram_latency;
    uint64_t dram_size;

    //Flash
    uint32_t flash_read_latency;
    uint32_t flash_write_latency;
    uint32_t flash_erase_latency;
    
    uint64_t page_size;
    uint32_t pages_per_block;
    uint32_t blocks;
    uint64_t flash_size;

    uint64_t hit_id;
    
    uint64_t pages_used;
    
    boost::scoped_array<uint32_t> page_accesses;
	uint32_t linesize_;
}; 

}
#endif
