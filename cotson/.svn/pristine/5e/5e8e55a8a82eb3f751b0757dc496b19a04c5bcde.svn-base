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

#define BOOST_TEST_MODULE test_metric
#include <boost/test/included/unit_test.hpp>

#include <map>
#include <set>
#include <boost/ref.hpp>
#include "libmetric.h"

using namespace std;

BOOST_AUTO_TEST_CASE( test_1 )
{
	struct X : public metric
	{
		X() : a(1), b(1) 
		{
			add("a",a);
			add("b",b);
			add_ratio("c","a","b");
		}
		uint64_t a;
		uint64_t b;
	};
	
	X x;
	BOOST_CHECK_NO_THROW(x["b"]);
	BOOST_CHECK_EQUAL(x["b"],1LLU);
	BOOST_CHECK_EQUAL(x.ratio("c"),1.0);
	x.b=2;
	BOOST_CHECK_EQUAL(x["b"],2LLU);
	BOOST_CHECK_EQUAL(x.ratio("c"),.5);
	
	BOOST_CHECK_THROW(x["d"],runtime_error);
	BOOST_CHECK_THROW(x.ratio("d"),runtime_error);
	
	X::iterator i=x.begin();
	BOOST_CHECK_EQUAL(*i,"a");
	++i;
	BOOST_CHECK_EQUAL(*i,"b");
	++i;
	BOOST_CHECK_EQUAL(*i,"c");
	++i;
	BOOST_CHECK(i==x.end());

	BOOST_CHECK(x.begin()==x.names_begin());
	BOOST_CHECK(x.names_end()==x.ratios_begin());
	BOOST_CHECK(x.ratios_end()==x.end());
}

BOOST_AUTO_TEST_CASE( test_2 )
{
	struct X : public metric
	{
		X() 
		{
			BOOST_CHECK_NO_THROW(add("a",a));
			BOOST_CHECK_THROW(add("a",a),runtime_error);
			BOOST_CHECK_THROW(add_ratio("c","a","b"),runtime_error);
		}
		uint64_t a;
	};
	
	X x;
}

BOOST_AUTO_TEST_CASE( test_3 )
{
	struct X : public metric
	{
		X() 
		{
			add("a",a);
			add_ratio("c","a","a");
		}
		uint64_t a;
	};

	struct Y : public metric
	{
		Y() 
		{
			add("a",a);
			add_ratio("c","a","a");
			BOOST_CHECK_THROW(add("",x),runtime_error);
			add("X.",x);
		}
		uint64_t a;
		X x;
	};

	Y y;
}

BOOST_AUTO_TEST_CASE( test_4 )
{
	struct E : public metric
	{
		uint64_t v1;
		uint64_t v2;
	
		E(uint64_t i) : v1(i), v2(i+1) { add("v1",v1); }
		void do_then() { add("v2",v2); }
		void do_add(string p,E& other) { add(p,other); } 
	};

	E a(1),b(10),c(100);
	
	BOOST_CHECK_NO_THROW(a["v1"]);
	BOOST_CHECK_THROW(a["v2"],runtime_error);
	BOOST_CHECK_NO_THROW(b["v1"]);
	BOOST_CHECK_THROW(b["v2"],runtime_error);
	BOOST_CHECK_NO_THROW(c["v1"]);
	BOOST_CHECK_THROW(c["v2"],runtime_error);

	BOOST_CHECK_EQUAL(a["v1"],1LLU);
	BOOST_CHECK_EQUAL(b["v1"],10LLU);
	BOOST_CHECK_EQUAL(c["v1"],100LLU);

	a.do_add("b.",b);
	BOOST_CHECK_NO_THROW(a["b.v1"]);
	BOOST_CHECK_EQUAL(a["b.v1"],10LLU);
	BOOST_CHECK_EQUAL(&a["b.v1"],&b["v1"]);

	BOOST_CHECK_THROW(a["b.v2"],runtime_error);
	b.do_then();
	BOOST_CHECK_NO_THROW(b["v2"]);
	BOOST_CHECK_EQUAL(a["b.v2"],b["v2"]);
	BOOST_CHECK_EQUAL(&a["b.v2"],&b["v2"]);
	
	BOOST_CHECK_THROW(a["b.c.v1"],runtime_error);
	BOOST_CHECK_THROW(b["c.v1"],runtime_error);
	b.do_add("c.",c);
	BOOST_CHECK_NO_THROW(a["b.c.v1"]);
	BOOST_CHECK_NO_THROW(b["c.v1"]);
	BOOST_CHECK_EQUAL(a["b.c.v1"],b["c.v1"]);
	BOOST_CHECK_EQUAL(&a["b.c.v1"],&b["c.v1"]);
	BOOST_CHECK_EQUAL(a["b.c.v1"],c["v1"]);
	BOOST_CHECK_EQUAL(&a["b.c.v1"],&c["v1"]);

	
	BOOST_CHECK_THROW(a["b.c.v2"],runtime_error);
	BOOST_CHECK_THROW(b["c.v2"],runtime_error);
	c.do_then();
	BOOST_CHECK_NO_THROW(a["b.c.v2"]);
	BOOST_CHECK_NO_THROW(b["c.v2"]);
	BOOST_CHECK_EQUAL(a["b.c.v2"],b["c.v2"]);
	BOOST_CHECK_EQUAL(&a["b.c.v2"],&b["c.v2"]);
	BOOST_CHECK_EQUAL(a["b.c.v2"],c["v2"]);
	BOOST_CHECK_EQUAL(&a["b.c.v2"],&c["v2"]);
}


BOOST_AUTO_TEST_CASE( test_5 )
{
	struct X : public metric
	{
		X() : a(1), b(2) 
		{
			add("a",a);
			add("b",b);
		}
		uint64_t a;
		uint64_t b;
		void modify()
		{
			a=3;
			b=4;
		}
	};

	X x;
	BOOST_CHECK_NO_THROW(x["a"]);
	BOOST_CHECK_EQUAL(x["a"],1LLU);
	BOOST_CHECK_NO_THROW(x["b"]);
	BOOST_CHECK_EQUAL(x["b"],2LLU);

	metric_storage st=x.checkpoint();

	BOOST_CHECK_NO_THROW(x["a"]);
	BOOST_CHECK_EQUAL(x["a"],1LLU);
	BOOST_CHECK_NO_THROW(x["b"]);
	BOOST_CHECK_EQUAL(x["b"],2LLU);

	x.modify();

	BOOST_CHECK_NO_THROW(x["a"]);
	BOOST_CHECK_EQUAL(x["a"],3LLU);
	BOOST_CHECK_NO_THROW(x["b"]);
	BOOST_CHECK_EQUAL(x["b"],4LLU);

	x.restore(st);

	BOOST_CHECK_NO_THROW(x["a"]);
	BOOST_CHECK_EQUAL(x["a"],1LLU);
	BOOST_CHECK_NO_THROW(x["b"]);
	BOOST_CHECK_EQUAL(x["b"],2LLU);
}

BOOST_AUTO_TEST_CASE( test_6 )
{
	struct F : public metric
	{
		uint64_t v1;
		uint64_t v2;
	
		F(uint64_t i) : v1(i) { add("v1",v1); }
		void do_add(string p,F& other) { add(p,other); } 
	};

	F a(1),b(10),c(100);
	a.do_add("b.",b);
	b.do_add("c.",c);
	
	BOOST_CHECK_EQUAL(a["v1"],1LLU);
	BOOST_CHECK_EQUAL(a["b.v1"],10LLU);
	BOOST_CHECK_EQUAL(a["b.c.v1"],100LLU);

	metric_storage st=a.checkpoint();

	BOOST_CHECK_EQUAL(a["v1"],1LLU);
	BOOST_CHECK_EQUAL(a["b.v1"],10LLU);
	BOOST_CHECK_EQUAL(a["b.c.v1"],100LLU);

	a.v1*=2;
	b.v1*=3;
	c.v1*=4;

	BOOST_CHECK_EQUAL(a["v1"],2LLU);
	BOOST_CHECK_EQUAL(a["b.v1"],30LLU);
	BOOST_CHECK_EQUAL(a["b.c.v1"],400LLU);

	a.restore(st);

	BOOST_CHECK_EQUAL(a["v1"],1LLU);
	BOOST_CHECK_EQUAL(a["b.v1"],10LLU);
	BOOST_CHECK_EQUAL(a["b.c.v1"],100LLU);
}

BOOST_AUTO_TEST_CASE( test_7 )
{
	struct X : public metric
	{
		X() : a(1) { add("a",a); }
		uint64_t a;
	};

	X x;
	BOOST_CHECK_EQUAL(x["a"],1LLU);

	metric_storage st1=x.checkpoint();
	metric_storage st2=x.checkpoint();

	st1+=st2;
	x.restore(st1);
	BOOST_CHECK_EQUAL(x["a"],2LLU);

	st1-=st2;
	x.restore(st1);
	BOOST_CHECK_EQUAL(x["a"],1LLU);

	st1*=4;
	x.restore(st1);
	BOOST_CHECK_EQUAL(x["a"],4LLU);

	st1/=3;
	x.restore(st1);
	x.restore(st1);
	BOOST_CHECK_EQUAL(x["a"],1LLU);
}

BOOST_AUTO_TEST_CASE( test_8 )
{
	struct X : public metric
	{
		X() : b()
		{
			add("b",b);
		}
		difference b;
	};

	X x;
	BOOST_CHECK_EQUAL(x.b.base,0ULL);
	BOOST_CHECK_EQUAL(x.b.v,0ULL);

	x.b.set(2);
	BOOST_CHECK_EQUAL(x.b.base,0ULL);
	BOOST_CHECK_EQUAL(x.b.v,2ULL);

	x.b.reset(4);
	BOOST_CHECK_EQUAL(x.b.base,4ULL);
	BOOST_CHECK_EQUAL(x.b.v,0ULL);

	x.b.set(6);
	BOOST_CHECK_EQUAL(x.b.base,4ULL);
	BOOST_CHECK_EQUAL(x.b.v,2ULL);

	x.clear_metrics();
	BOOST_CHECK_EQUAL(x.b.base,6ULL);
	BOOST_CHECK_EQUAL(x.b.v,0ULL);
}


