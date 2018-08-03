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

// $Id: test_tagged_metrics.cpp 11 2010-01-08 13:20:58Z paolofrb $

#define BOOST_TEST_MODULE test_tagged_metrics
#include <boost/test/included/unit_test.hpp>

#include <map>
#include <set>
#include <boost/ref.hpp>

#include "libmetric.h"
//#include "tagged_metric.cpp"

using namespace std;

BOOST_AUTO_TEST_CASE( basic_tag_mechanism )
{
	struct X : public metric
	{
		X() : a(1) 
		{
			add("a",a);
		}
		uint64_t a;
	};

	X x;
	TaggedMetrics tm=TaggedMetrics::get();
	tm.add(&x,"hola caracola");

	BOOST_CHECK_EQUAL(tm.end("hola")-tm.begin("hola"),1);
	BOOST_CHECK_EQUAL(tm.end("caracola")-tm.begin("caracola"),1);
	BOOST_CHECK_EQUAL(tm.end("unknown")-tm.begin("unknown"),0);

	for(TaggedMetrics::iterator i=tm.begin("hola");i!=tm.end("hola");++i)
	{
		BOOST_CHECK_NO_THROW((**i)["a"]);
		BOOST_CHECK_EQUAL((**i)["a"],1LLU);
	}
}

BOOST_AUTO_TEST_CASE( add_does_not_add_twice )
{
	struct X : public metric
	{
		X() : a(1) 
		{
			add("a",a);
		}
		uint64_t a;
	};

	X x;
	TaggedMetrics tm=TaggedMetrics::get();
	tm.add(&x,"hola");
	BOOST_CHECK_EQUAL(tm.end("hola")-tm.begin("hola"),1);
	tm.add(&x,"hola");
	BOOST_CHECK_EQUAL(tm.end("hola")-tm.begin("hola"),1);
}
