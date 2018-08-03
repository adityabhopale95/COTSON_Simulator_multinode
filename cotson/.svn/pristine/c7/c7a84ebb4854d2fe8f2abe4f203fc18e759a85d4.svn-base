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

#define BOOST_TEST_MODULE test_options
#include <boost/test/included/unit_test.hpp>

#include <iostream>
#include <map>
#include <set>
#include <boost/lexical_cast.hpp>
#include <lua.hpp>
#include "liboptions.h"

//#include "cast.cpp"
//#include "options.cpp"
//#include "parameters.cpp"
//#include "uint64_t_hash.cpp"
//#include "units.cpp"

using namespace std;

static void f() { }
luabind::scope definition()
{
	return luabind::def("f",f);
}

option o1("i","false","");
option o2("v","1","");
option o3("u","1k","");

BOOST_AUTO_TEST_CASE( my_test )
{
	Option::init("options.i=true options.v=2 options.u=\"2kB\"");
	
	BOOST_CHECK_EQUAL(Option::get<bool>("i"),true);
	BOOST_CHECK_EQUAL(Option::get<int>("v"),2);
	BOOST_CHECK_EQUAL(Option::get<int>("u"),2048);
}
