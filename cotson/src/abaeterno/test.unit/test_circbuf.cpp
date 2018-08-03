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

// $Id: test_circbuf.cpp 11 2010-01-08 13:20:58Z paolofrb $

#define BOOST_TEST_MODULE test_circbuf
#include <boost/test/included/unit_test.hpp>

#include "circbuf.h"

using namespace std;

BOOST_AUTO_TEST_CASE( test_init )
{
	for (int s=1; s<100; ++s) {
	    CircBuf<int> buf1(s);
	    for (int i=0; i<s; ++i)
 	        BOOST_CHECK_EQUAL(buf1[i],0);

	    CircBuf<unsigned long long *> buf2(s);
	    for (int i=0; i<s; ++i)
 	        BOOST_CHECK_EQUAL(buf2[i],(unsigned long long*)0);
    }
}

BOOST_AUTO_TEST_CASE( test_access )
{
    CircBuf<int> buf(4);
	buf.push_back(100);
	buf.push_back(101);
	buf.push_back(102);
	buf.push_back(103);
 	BOOST_CHECK_EQUAL(buf[1],103);
 	BOOST_CHECK_EQUAL(buf[2],102);
 	BOOST_CHECK_EQUAL(buf[3],101);
 	BOOST_CHECK_EQUAL(buf[0],100);

	buf.push_back(104);
 	BOOST_CHECK_EQUAL(buf[1],104);
 	BOOST_CHECK_EQUAL(buf[2],103);
 	BOOST_CHECK_EQUAL(buf[3],102);
 	BOOST_CHECK_EQUAL(buf[0],101);

	buf.push_back(105);
 	BOOST_CHECK_EQUAL(buf[1],105);
 	BOOST_CHECK_EQUAL(buf[2],104);
 	BOOST_CHECK_EQUAL(buf[3],103);
 	BOOST_CHECK_EQUAL(buf[0],102);
}

BOOST_AUTO_TEST_CASE( test_corner )
{
    CircBuf<int> buf1(1);
    CircBuf<int> buf2(2);
	buf1.push_back(100);
 	BOOST_CHECK_EQUAL(buf1[0],100);
	buf1.push_back(101);
 	BOOST_CHECK_EQUAL(buf1[1],101);

	buf2.push_back(100);
	buf2.push_back(101);
 	BOOST_CHECK_EQUAL(buf2[1],101);
 	BOOST_CHECK_EQUAL(buf2[0],100);
	buf2.push_back(102);
 	BOOST_CHECK_EQUAL(buf2[1],102);
 	BOOST_CHECK_EQUAL(buf2[0],101);
}

