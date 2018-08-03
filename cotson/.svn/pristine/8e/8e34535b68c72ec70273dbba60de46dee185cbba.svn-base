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
#include "memgate.h"
#include "liboptions.h"
#include "error.h"

#include <stdexcept>
#include <fstream>
#include <iostream>
#include <iterator>
#include <boost/io/ios_state.hpp>

#include <dlfcn.h>

using namespace std;
using namespace boost;

#ifdef USE_MEMGATE
namespace {
option  o1("memgate_depth", "16", "amount of stack frames to store");
option  o2("memgate_check_base", "true", "do we check for base !");
}
#endif

namespace MemGate {

int depth=16;
bool check_base=true;
bool subtract_base=true;

uint64_t Element::base_addr;

Element::Element() : size(0),array(false) {}

Element::Element(size_t s,bool a) : size(s), array(a)
{
	#define UI(X) reinterpret_cast<uint64_t>(X)
	#define PRINT_THIS(v) { \
		cout << endl;\
		cout << "starting           " << v << endl; \
		cout << "frame address      " << hex << UI(__builtin_frame_address(v)) << endl; \
		const void *a=__builtin_return_address(v); \
		cout << "return address     " << hex << UI(a) << endl; \
		Dl_info dli; dladdr(a,&dli); \
		cout << "base address       " << hex << UI(dli.dli_fbase) << endl; \
		cout << "abs base address   " << hex << UI(base_addr) << endl; }

	#define GET_THIS(v) { \
		if(MemGate::depth==v) return;\
		if(!__builtin_frame_address(v)) return; \
		const void* a=__builtin_return_address(v);\
		if(!a) return;\
		if(check_base) {\
		Dl_info dli; dladdr(a,&dli);\
		if(UI(dli.dli_fbase)!=base_addr) return; } \
		if(subtract_base) stack.push_back(UI(a)-base_addr); \
		else stack.push_back(UI(a)); }


	GET_THIS(0); // this is memgate:xxx
	GET_THIS(1); // this is memgate:xxx
	GET_THIS(2);
	GET_THIS(3);
	GET_THIS(4);
	GET_THIS(5);
	GET_THIS(6);
	GET_THIS(7);
	GET_THIS(8);
	GET_THIS(9);
	GET_THIS(10);
	GET_THIS(11);
	GET_THIS(12);
	GET_THIS(13);
	GET_THIS(14);
	GET_THIS(15);
	GET_THIS(16);
}

ostream& operator<<(ostream& out,const Element& info)
{
	io::ios_all_saver ias(out);
	out << info.size << " " << boolalpha << info.array;
	out << hex;
	for(vector<uint64_t>::const_iterator i=info.stack.begin();i!=info.stack.end();++i)
		out << " " << *i;
	return out;
}

Bag::~Bag() 
{ 
	enable = false;

	for(map<void*,Element>::iterator it=active.begin();it!=active.end();++it)
		WARNING("memgate::active ", it->second);

	for(map<void*,pair<Element,Element> >::iterator it=array_errors.begin();it!=array_errors.end();++it)
		WARNING("memgate::array ", it->second.first, ":", it->second.second);
}

void Bag::start()
{
	if(!monitor)
	{
		active.clear();
		Dl_info dli;
		dladdr(__builtin_return_address(0),&dli);
		Element::base_addr=reinterpret_cast<uint64_t>(dli.dli_fbase);
		if(string(dli.dli_fname) == "aaInjector")
			subtract_base=false;
		monitor=true;
	}
}

void Bag::stop() 
{
	monitor=false; 
}

void* Bag::malloc(size_t size, bool array)
{
	void* p=::malloc(size); 
	if(!enable || !monitor)
		return p;

	enable=false;
	if(p) 
		active[p]=Element(size,array);

	enable=true;
	return p;
}

void Bag::free(void *p, bool array)
{
	if(!enable)
	{
		::free(p);
		return;
	}	
	
	enable=false;
	if(p) 
	{
		map<void*,Element>::iterator i=active.find(p);
		if(i!=active.end())
		{
			if (i->second.array != array) 
				array_errors[i->first]=make_pair(i->second,Element(0,0));
			active.erase(i);
		}
		else
		{
			// too much noise
			WARNING("memgate::non-alloc-delete ", p, " ", Element(0,0));
		}
	}
	enable=true;
	::free(p);
}

void start() { Bag::get().start(); }
void stop() { Bag::get().stop(); }

}

#ifdef USE_MEMGATE
void* operator new  (size_t size) { return MemGate::Bag::get().malloc(size, false); } 
void* operator new[](size_t size) { return MemGate::Bag::get().malloc(size, true);  }
void  operator delete  (void *p) { MemGate::Bag::get().free(p, false); }
void  operator delete[](void *p) { MemGate::Bag::get().free(p, true);  }
#endif
