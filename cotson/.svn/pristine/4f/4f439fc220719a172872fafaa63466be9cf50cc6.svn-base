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
#ifndef STORAGE_H
#define STORAGE_H

#include "cache.h"
#include "cache_line.h"
#include "liboptions.h"

namespace Memory { 
namespace Storage { 
namespace detail // only our storages will use it!
{
	template <typename T>
	class base
	{
		public:

		typedef Cache<T> CacheType;

		base(const Parameters&p):
			size(p.get<uint32_t>("size")),
			num_sets(p.get<uint32_t>("num_sets")),
			line_size(p.get<uint32_t>("line_size")),
			last_tag(~0ULL),
			last_line(0),
			cache(cachesize(size,num_sets),line_size)
		{}

		protected:

		const uint32_t size;
		const uint32_t num_sets;
		const uint32_t line_size;

		// fast links to last hit
		uint64_t last_tag; 
		Line* last_line;

		public:

		CacheType cache;
		void dump() 
		{ 
			std::cout << "dumping storage" << std::endl; 
			std::cout << "size=" << size << std::endl;
			std::cout << "num_sets=" << num_sets << std::endl;
			std::cout << "line_size=" << line_size << std::endl;
			cache.dump();
		} 

		private:

		uint32_t cachesize(uint32_t size, uint32_t num_sets)
		{
			if(num_sets==0)
				throw std::invalid_argument("num of sets can not be zero");
			if(size%num_sets!=0)
				throw std::invalid_argument("size must be divisible by num of sets");
			return size/num_sets;
		}
	};
}}}
#endif
