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

// $Id: lua_helpers.h 11 2010-01-08 13:20:58Z paolofrb $
#ifndef HELPERS_H
#define HELPERS_H

#include <lua.hpp>
#include "memory_interface.h"

namespace Memory { 

struct lua;

namespace detail {

struct luaDestroy
{
	std::vector<Memory::lua*> luas;
	~luaDestroy();
	void add(Memory::lua* l); 
};

}

struct lua 
{
	static detail::luaDestroy& destroy()
	{
		static detail::luaDestroy singleton;
		return singleton;
	}

	lua(Interface::Shared p) : pointer(p) {}
	void next(lua&);

	Interface::Shared pointer;
};

}
#endif
