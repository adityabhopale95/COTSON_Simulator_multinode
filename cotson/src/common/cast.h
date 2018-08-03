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

// $Id: cast.h 11 2010-01-08 13:20:58Z paolofrb $
#ifndef CAST_H
#define CAST_H

#include "units.h"
#include <boost/lexical_cast.hpp>
#include <lua.hpp>

template<typename T> T cast(const std::string& v,const std::string& name)
{
	T t;
	try
	{
		t=boost::lexical_cast<T>(v);
	}
	catch(boost::bad_lexical_cast& e)
	{
		try
		{
			t=boost::lexical_cast<units<T> >(v);
		}
		catch(boost::bad_lexical_cast& e2)
		{
			throw std::invalid_argument((std::string("option ")+
				name+" has been casted incorrectly").c_str());
		}
		catch(std::invalid_argument& e3)
		{
			throw std::invalid_argument((std::string("option ")+
				name+" has been casted incorrectly: "+e3.what()).c_str());
		}
	}
	return t;
}

template<> bool cast(const std::string&,const std::string&);

std::string cast(luabind::iterator&);

#endif
