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

// $Id: memgate.h 231 2011-11-22 11:09:59Z paolofrb $
#ifndef MEMGATE_H
#define MEMGATE_H

#include <stdint.h>
#include <map>
#include <vector>

#ifdef USE_MEMGATE

#define MEMGATE_HOOKS \
void* operator new  (std::size_t size) { return MemGate::Bag::get().malloc(size, false); } \
void* operator new[](std::size_t size) { return MemGate::Bag::get().malloc(size, true);  } \
void  operator delete  (void *p) { MemGate::Bag::get().free(p, false); }              \
void  operator delete[](void *p) { MemGate::Bag::get().free(p, true);  }

#else

#define MEMGATE_HOOKS 

#endif

namespace MemGate {
extern int depth;
extern bool check_base;

class Element
{
public:
	static uint64_t base_addr;
	std::size_t size;
	bool array;
	std::vector<uint64_t> stack;

	Element();
	Element(std::size_t size_,bool array_);
};

class Bag 
{
	public:
	static Bag & get(void)
	{
		static Bag singleton;
		return singleton;
	}

	void start();
	void stop(); 

	void* malloc(std::size_t size, bool array);
	void free(void *p, bool array);

	virtual ~Bag(); 
		
private:
	bool monitor;
	bool enable;

	Bag() : monitor(false), enable(true) {}

	std::map<void*,Element> active;
 	std::map<void*,std::pair<Element,Element> > array_errors;
};

void start(); 
void stop(); 

}

struct use_static_memgate
{
	use_static_memgate() { MemGate::start(); }
};

#endif
