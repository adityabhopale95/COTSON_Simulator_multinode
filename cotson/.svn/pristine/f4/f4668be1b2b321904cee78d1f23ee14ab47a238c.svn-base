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

#define BOOST_TEST_MODULE test_timewheel
#include <boost/test/included/unit_test.hpp>

#include <zlib.h>
#include <set>
#include <map>
#include <boost/assign.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/key_extractors.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>
#include "timewheel.h"

using namespace std;
using namespace Memory;

static void up_to(TimeWheel& w, uint64_t t)
{
	while (!w.empty() && w.top()->when <= t)
	    w.pop();
}

static void insert(TimeWheel& w, uint64_t c,uint64_t t,uint64_t a,MOESI_state ms,Future::Type tp)
{
	w.insert(Future(c,t,a,ms,Trace(0),tp,0));
}

BOOST_AUTO_TEST_CASE( test_same_tag )
{
    TimeWheel w;
    insert(w,200,1,10,OWNER,Future::UPDATE);
    insert(w,1,2,0,EXCLUSIVE,Future::UPDATE); // noise
    insert(w,2,3,0,EXCLUSIVE,Future::UPDATE); // noise
    BOOST_CHECK(w.size() == 3);
    BOOST_CHECK(w.has(1,Future::UPDATE)->addr == 10);
    BOOST_CHECK(w.has(1,Future::UPDATE)->when == 200);

    insert(w,100,1,20,EXCLUSIVE,Future::UPDATE);
    insert(w,99,4,0,EXCLUSIVE,Future::UPDATE); // noise
    insert(w,100,5,0,EXCLUSIVE,Future::UPDATE); // noise
    insert(w,101,6,0,EXCLUSIVE,Future::UPDATE); // noise
	// first elem with tag=1 is now 100
    BOOST_CHECK(w.size() == 7);
    BOOST_CHECK(w.has(1,Future::UPDATE)->addr == 20);
    BOOST_CHECK(w.has(1,Future::UPDATE)->when == 100);

    insert(w,300,1,30,NOT_FOUND,Future::UPDATE);
	// first elem with tag=1 remains 100 
    BOOST_CHECK(w.size() == 8);
    BOOST_CHECK(w.has(1,Future::UPDATE)->addr == 20);
    BOOST_CHECK(w.has(1,Future::UPDATE)->when == 100);

	up_to(w,200);
	// first elem with tag=1 is now 300
	const Future* f=w.has(1,Future::UPDATE);
    BOOST_CHECK(w.size() == 1);
	BOOST_CHECK(f && f->addr==30 && f->when==300);

    insert(w,299,1,40,OWNER,Future::UPDATE);
	// first elem with tag=1 is now 299
    BOOST_CHECK(w.size() == 2);
	f=w.has(1,Future::UPDATE);
    BOOST_CHECK(f && f->addr == 40 && f->when==299);
}

BOOST_AUTO_TEST_CASE( test_ordering )
{
	TimeWheel w;
	BOOST_CHECK(w.size() == 0);

	insert(w,20,0x102,2,OWNER,Future::UPDATE);
	insert(w,10,0x101,1,OWNER,Future::UPDATE);
	insert(w,40,0x104,4,OWNER,Future::UPDATE);
	insert(w,30,0x103,3,OWNER,Future::UPDATE);
	BOOST_CHECK(w.size() == 4);

	// check that entries are retrieved in time order
	BOOST_CHECK(w.top()->addr == 1 && w.top()->when == 10); w.pop();
	BOOST_CHECK(w.top()->addr == 2 && w.top()->when == 20); w.pop();
	BOOST_CHECK(w.top()->addr == 3 && w.top()->when == 30); w.pop();
	BOOST_CHECK(w.top()->addr == 4 && w.top()->when == 40); w.pop();
}

BOOST_AUTO_TEST_CASE( test_erase )
{
	TimeWheel w;
	BOOST_CHECK(w.size() == 0);

	insert(w,200,0x20,0x20,OWNER,Future::UPDATE);
	insert(w,100,0x10,0x10,OWNER,Future::UPDATE);
	insert(w,100,0x11,0x11,OWNER,Future::UPDATE);
	insert(w,400,0x40,0x40,OWNER,Future::UPDATE);
	insert(w,300,0x30,0x30,OWNER,Future::UPDATE);
	BOOST_CHECK(w.size() == 5);

	// nothing gets erased
	up_to(w,0);
	BOOST_CHECK(w.size() == 5);

	// remove the two elems at 10
	up_to(w,100);
	BOOST_CHECK(w.size() == 3);
	BOOST_CHECK(!w.has(0x10,Future::UPDATE));
	BOOST_CHECK(!w.has(0x11,Future::UPDATE));

	// add another elem with tag 10 and check
	insert(w,110,0x10,0x12,OWNER,Future::UPDATE);
	BOOST_CHECK(w.has(0x10,Future::UPDATE)->addr == 0x12);
}

BOOST_AUTO_TEST_CASE( test_type )
{
	TimeWheel w;
	// Check type retrieval
	insert(w,10,100,101,OWNER,Future::UPDATE);
	insert(w,10,100,102,OWNER,Future::INVALIDATE);
	BOOST_CHECK(w.has(100,Future::UPDATE)->addr == 101);
	BOOST_CHECK(w.has(100,Future::INVALIDATE)->addr == 102);

	// Check retrieval of earliest time
	insert(w, 5,100,103,OWNER,Future::UPDATE);
	insert(w,20,100,104,OWNER,Future::UPDATE);
	insert(w, 8,100,105,OWNER,Future::INVALIDATE);
	insert(w,80,100,106,OWNER,Future::INVALIDATE);
	BOOST_CHECK(w.has(100,Future::UPDATE)->when == 5);
	BOOST_CHECK(w.has(100,Future::INVALIDATE)->when == 8);
}
