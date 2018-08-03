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
#include "debug_mem.h"

#include "cache_line.h"
#include "storage_two_way.h"
#include "storage_four_way.h"

using namespace std;

namespace Debug {

void dump(const Memory::Line& tl)
{
	cout << "tag=" << hex << tl.tag 
		<< ", moesi="  << tl.moesi <<  dec
		<< ", lru="  << tl.lru << endl;
}

void dump(const Memory::Storage::TwoLines& tl)
{
	cout << "line0: tag=" << hex << tl.e0.tag 
		<< ", moesi="  << tl.e0.moesi <<  dec
		<< ", lru="  << tl.e0.lru << endl;
	cout << "line1: tag=" << hex << tl.e1.tag 
		<< ", moesi="  << tl.e1.moesi <<  dec
		<< ", lru="  << tl.e1.lru << endl;
}

void dump(const Memory::Storage::FourLines& tl)
{
	cout << "line0: tag=" << hex << tl.e0.tag 
		<< ", moesi="  << tl.e0.moesi <<  dec
		<< ", lru="  << tl.e0.lru << endl;
	cout << "line1: tag=" << hex << tl.e1.tag 
		<< ", moesi="  << tl.e1.moesi <<  dec
		<< ", lru="  << tl.e1.lru << endl;
	cout << "line2: tag=" << hex << tl.e2.tag 
		<< ", moesi="  << tl.e2.moesi <<  dec
		<< ", lru="  << tl.e2.lru << endl;
	cout << "line3: tag=" << hex << tl.e3.tag 
		<< ", moesi="  << tl.e3.moesi <<  dec
		<< ", lru="  << tl.e3.lru << endl;
}

void dump(const Memory::Storage::CacheLineMap& m)
{
	int j=0;
	for(Memory::Storage::CacheLineMap::const_iterator i=m.begin(); i != m.end(); ++i,++j) 
		cout << "line" << j << ": tag=" << hex << i->first 
			<< ", moesi="  << i->second->moesi <<  dec
			<< ", lru="  << i->second->lru << endl;
}

void dump(const Memory::Storage::CacheLineVec& v)
{
	int j=0;
	for(Memory::Storage::CacheLineVec::const_iterator i=v.begin(); i != v.end(); ++i,++j) 
		cout << "line" << j << ": tag=" << hex << i->tag
			<< ", moesi="  << i->moesi <<  dec
			<< ", lru="  << i->lru << endl;
}

}
