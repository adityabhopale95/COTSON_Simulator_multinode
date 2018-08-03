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
// derived from Abstract Factory, Template Style from Hyslop
// see orig_factory_template.h for differences and copyright
// eliminated auto_ptr
// and changed to suit my tastes (smaller template names,
// other function names ...)
// changed the singleton pattern slightly

#ifndef FACTORY_H
#define FACTORY_H

#include <boost/noncopyable.hpp>
#include "parameters.h"
#include "kill.h"

template <class Base,typename Key=std::string>
class Factory : public boost::noncopyable
{
	typedef Base* (*createFun)(Parameters&);
	std::map<Key, createFun> registry;

	Factory() {}
	static Factory& singleton()
	{
		static Factory a;
		return a;
	}

public:

	static void add(const Key& name, createFun fun)
	{
		singleton().registry[name]=fun;
	}
  
	static Base* create(Parameters p)
	{
		std::string type=p.get<std::string>("type");
		typename std::map<Key, createFun>::const_iterator 
			i=singleton().registry.find(type);
		if(i != singleton().registry.end())
			return i->second(p);

        std::cerr << "**ERROR: Unknown element: " << type << std::endl;
        std::cerr << " known ones are: " << std::endl;
        for(iterator i=begin();i!=end(); ++i)
            std::cerr << "    " << i->first << std::endl;
        std::cerr << kill; 
		return 0;
	}

	typedef typename std::map<Key, createFun>::iterator iterator;
	static iterator begin() { return singleton().registry.begin(); }
	static iterator end()   { return singleton().registry.end(); }
};

template <class Base, class Derived, typename Key=std::string>
struct registerClass
{
	static Base* createInstance(Parameters& p)
	{
		return new Derived(p);
	}

	registerClass(const Key &id)
	{
		Factory<Base,Key>::add(id, createInstance);
	}
};

#endif
