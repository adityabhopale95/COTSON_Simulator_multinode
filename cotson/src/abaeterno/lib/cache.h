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

// $Id: cache.h 418 2012-11-21 15:09:57Z paolofrb $
#ifndef CACHE_H
#define CACHE_H

#include "liboptions.h"
#include "debug_mem.h"

namespace Memory
{

template<typename Addr> 
inline static size_t ilog2(Addr x)
{
#if defined(__GNUC__)
    // Use optimized gcc builtin
	uint64_t n = static_cast<uint64_t>(x);
    return 8*sizeof(uint64_t)-__builtin_clzll((n))-1;
#else
	size_t i=-1;
	for(uint64_t n=static_cast<uint64_t>(x); n>0; n>>=1,i++);
	return i;
#endif
}


// needed for the template friend function!
template<typename Elem,typename Addr>  class Cache;
 
template<typename Elem,typename Addr=uint64_t>
class Cache 
{
	public:
	Cache(Addr sz, Addr gs) :
		size(sz),       // size       : quantity of elements
		group_size_(gs), // group_size : size of each group of elements
		group_shift(ilog2(group_size_)),
		cache(0)
	{
		if(size&(size-1))
			throw std::invalid_argument(std::string("size must be power of two"));
		if(group_size_&(group_size_-1))
			throw std::invalid_argument(std::string("group size must be power of two"));
		if(size==0)
			throw std::invalid_argument(std::string("size can not be zero"));
		if(group_size_==0)
			throw std::invalid_argument(std::string("group size can not be zero"));
		cache = new Elem[size/group_size_];
	}

	~Cache() { delete[] cache; }

	void init(Elem e)
	{
		for(uint i=0;i<size/group_size_;i++)
			cache[i]=e;
	}

	// void is not a mistake here
	void operator=(const Cache<Elem,Addr>& other)
	{
		if(this == &other)
			return;
		if(size != other.size || group_size_ != other.group_size_)
			throw std::runtime_error(std::string("copying caches only between identical sizes!"));

		Elem* ncache =new Elem[size/group_size_];
		for(uint i=0;i<size/group_size_;i++)
			ncache[i]=other.cache[i];

		delete[] cache;
	    cache=ncache;
	}

	INLINE Addr group_id(Addr n) const { return n >> group_shift; }
	INLINE Addr addr_from_group_id(Addr n) const { return n << group_shift; }
	INLINE Addr group_size() const { return group_size_; } 

	INLINE Elem& operator[](Addr addr)
	{
		return cache[group_id(addr&(size-1))];
	}

	void dump()
	{
		std::cout << "CACHE-BEGIN" << std::endl;
		std::cout << "size=" << size << std::endl;
		std::cout << "group_size=" << group_size_ << std::endl;
		for(uint i=0;i<size/group_size_;i++)
		{
			std::cout << "ELEM-" << i << "-BEGIN" << std::endl;
			Debug::dump(cache[i]);
		}
		std::cout << "ELEMS-END" << std::endl;
	}
	
	protected:

	const Addr size;
	const Addr group_size_;
	const Addr group_shift;

	Elem* cache;
}; 

}

#endif
