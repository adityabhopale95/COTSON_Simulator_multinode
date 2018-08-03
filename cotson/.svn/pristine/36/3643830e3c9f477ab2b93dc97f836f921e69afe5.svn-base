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

#include "cast.h"

using namespace std;
using namespace boost;

template<>
bool cast(const std::string& v,const std::string& name)
{
	if(v=="true")
		return true;
	else if(v=="false")
		return false;

	bool t;
	try
	{
		t=boost::lexical_cast<bool>(v);
	}
	catch(boost::bad_lexical_cast& e)
	{
		throw std::invalid_argument((std::string("option ")+
			name+" was cast incorrectly").c_str());
	}
	return t;
}

string cast(luabind::iterator& i)
{
	int type=luabind::type(*i);
	if(type==LUA_TBOOLEAN)
		return (*i==true)?"1":"0";

	if(type==LUA_TNUMBER || type==LUA_TSTRING)
		return lexical_cast<string>(*i);

	throw invalid_argument(lexical_cast<string>(i.key())+
		" has unacceptable syntactic type");
}

