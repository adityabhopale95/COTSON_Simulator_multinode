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

// $Id: debug_mem.h 11 2010-01-08 13:20:58Z paolofrb $
#ifndef DEBUG_MEM_H
#define DEBUG_MEM_H
#include "inline.h"
#include "hashmap.h"

namespace Memory
{
	struct Line;
	namespace Storage
	{
		struct TwoLines;
		struct FourLines;
        typedef HashMap<uint64_t,Line*> CacheLineMap;
		typedef std::vector<Memory::Line> CacheLineVec;
	}
}

namespace Debug 
{
	void dump(const Memory::Line&);
	void dump(const Memory::Storage::TwoLines&);
	void dump(const Memory::Storage::FourLines&);
	void dump(const Memory::Storage::CacheLineMap&);
	void dump(const Memory::Storage::CacheLineVec&);

	template<typename T>
	void dump(const T&) { std::cout << "TODO: dump not done" << std::endl; }
}

#endif

