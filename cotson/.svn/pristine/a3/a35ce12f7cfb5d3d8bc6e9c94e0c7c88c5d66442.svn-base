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
#ifndef HYBRID_MEMORY_CACHE_H
#define HYBRID_MEMORY_CACHE_H

#include "memory_interface.h"
#include "liboptions.h"
#include "cache.h"
#include "hybrid_memory.h"

namespace Memory {

class HybridCache : public Hybrid
{   
public:
    HybridCache(Parameters&);
    ~HybridCache();

private:
    struct CachePage
    {
	    CachePage():modified(false),group_id(~0) {};
	    bool modified;
	    uint64_t group_id;
    };
    boost::scoped_ptr<Memory::Cache<CachePage> > cache;
    uint32_t get_latency(const Access& m, uint64_t tstamp, bool write);
}; 

}
#endif
