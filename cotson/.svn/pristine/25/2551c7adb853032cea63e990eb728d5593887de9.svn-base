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

#include "lua_helpers.h"
#include "liboptions.h"

using namespace std;

namespace Memory {

void lua::next(lua& other)
{
	pointer->setNext(other.pointer);
	Option::track(pointer->Name()+".next",other.pointer->Name());
}

namespace detail {

luaDestroy::~luaDestroy()
{
	for(vector<Memory::lua*>::iterator i=luas.begin();i!=luas.end();++i)
		delete *i;
	luas.clear();
}

void luaDestroy::add(Memory::lua* l) 
{ 
	luas.push_back(l); 
}

}

}
