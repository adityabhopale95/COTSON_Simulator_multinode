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
#ifndef STATIC_SET_H
#define STATIC_SET_H

#include <boost/noncopyable.hpp>
#include <set>

template<typename Param>
class StaticSet : public boost::noncopyable
{
	std::set<Param> sset;

	StaticSet() {}

	// avoid destructor ordering issues
	~StaticSet() { singleton().sset.clear(); } 

	static StaticSet &singleton()
	{
		static StaticSet a;
		return a;
	}
	
	public:
				
	typedef typename std::set<Param>::iterator iterator;
	static void add(const Param& p)
	{
// 		std::set<Param>& s=singleton().sset;
// 		if(s.find(p)!=s.end())
// 			throw std::invalid_argument("duplicate");
		singleton().sset.insert(p);
	}

	static void remove(const Param& p)
	{
// 		std::set<Param>& s=singleton().sset;
// 		if(s.find(p)==s.end())
// 			throw std::invalid_argument("not found");
	    singleton().sset.erase(p);
	}

	static iterator begin() { return singleton().sset.begin(); }
	static iterator end()   { return singleton().sset.end(); }
};

template <typename Param>
struct registerSetParam
{
	registerSetParam(const Param& p)
	{
		StaticSet<Param>::add(p);
	}
};

template <typename Param>
struct unregisterSetParam
{
	unregisterSetParam(const Param& p)
	{
		StaticSet<Param>::remove(p);
	}
};

#endif

