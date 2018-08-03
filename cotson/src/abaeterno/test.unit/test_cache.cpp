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

// $Id: test_cache.cpp 11 2010-01-08 13:20:58Z paolofrb $

#define BOOST_TEST_MODULE test_cache
#include <boost/test/included/unit_test.hpp>

#include <iostream>
#include <map>
#include <boost/scoped_array.hpp>
#include "cache.h"

using namespace std;

BOOST_AUTO_TEST_CASE( test_group_id )
{
	Memory::Cache<uint> a(16,1);
	BOOST_CHECK(a.group_id(0)!=a.group_id(1));

	Memory::Cache<uint> b(16,2);
	BOOST_CHECK(b.group_id(0)!=b.group_id(2));
	BOOST_CHECK_EQUAL(b.group_id(0),b.group_id(1));

	Memory::Cache<uint> c(16,4);
	BOOST_CHECK(c.group_id(0)!=c.group_id(4));
	BOOST_CHECK_EQUAL(c.group_id(0),c.group_id(1));
	BOOST_CHECK_EQUAL(c.group_id(0),c.group_id(2));
	BOOST_CHECK_EQUAL(c.group_id(0),c.group_id(3));
}

BOOST_AUTO_TEST_CASE( test_exception )
{
	BOOST_CHECK_THROW(Memory::Cache<uint>(15,1),invalid_argument);
	BOOST_CHECK_THROW(Memory::Cache<uint>(16,3),invalid_argument);
	BOOST_CHECK_NO_THROW(Memory::Cache<uint>(16,1));
	BOOST_CHECK_THROW(Memory::Cache<uint>(0,1),invalid_argument);
	BOOST_CHECK_THROW(Memory::Cache<uint>(1,0),invalid_argument);
}

BOOST_AUTO_TEST_CASE( test_access )
{
	Memory::Cache<uint> a(16,1);
	for(uint i=0;i<16;i++)
		a[i]=i;
	for(uint i=0;i<16;i++)
		BOOST_CHECK_EQUAL(a[i],i);

	Memory::Cache<uint> b(16,2);
	for(uint i=0;i<16;i++)
		b[i]=i;
	for(uint i=0;i<16;i++)
	{
		if(i%2==0)
			BOOST_CHECK(b[i]!=i);
		else
			BOOST_CHECK_EQUAL(b[i],i);
	}
}
