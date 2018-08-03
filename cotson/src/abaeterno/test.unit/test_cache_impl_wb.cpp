// (C Copyright 2006-2009 Hewlett-Packard Development Company, L.P.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//

// $Id: test_cache_impl_wb.cpp 721 2015-06-03 02:29:33Z paolofrb $

// workaround for a g++ excess of zeal in static checks
#pragma GCC diagnostic ignored "-Warray-bounds"

#define BOOST_TEST_MODULE test_cache_impl_wb
#include <boost/test/included/unit_test.hpp>

#include <set>
#include <map>
#include <boost/lexical_cast.hpp>
#include <boost/ref.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/scoped_array.hpp>
#include <boost/io/ios_state.hpp>
#include <zlib.h>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/key_extractors.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>
#include <lua.hpp>

#include "abaeterno_config.h"

#include "main_memory.h"

#include "storage_one_way.h"
#include "storage_two_way.h"
#include "storage_four_way.h"
#include "storage_many.h"
#include "storage_basic.h"
#include "cache_timing_basic.h"
#include "cache_timing_l2.h"

#include "cache_impl.h"

//this next commented out include really includes the object
//file necessary for the testing (hence do not remove)
//#include "cache_timing_l2.cpp" 
//#include "main_memory.cpp" 
//#include "memory_interface.cpp" 
//#include "debug_mem.cpp" 
//#include "memory_access.cpp" 
//#include "cast.cpp"
//#include "options.cpp"
//#include "parameters.cpp"
//#include "uint64_t_hash.cpp"
//#include "units.cpp"

uint64_t Instruction::unique_id = 0; // static member

static void f() { }
luabind::scope definition()
{
	return luabind::def("f",f);
}


using namespace std;
using namespace boost;
using namespace Memory;

Interface* nCacheImpl(
    uint32_t size,
	uint32_t line_size,
	uint32_t num_sets,
	uint32_t latency,
	int mode=0)
{
	Parameters p;
	p.set("size",      lexical_cast<string>(size));
	p.set("line_size", lexical_cast<string>(line_size));
	p.set("num_sets",  lexical_cast<string>(num_sets));
	p.set("latency",   lexical_cast<string>(latency));
	p.set("name",      "name");
	p.set("type",      "...");
	p.set("write_policy","WB");
	if (mode==1)
	    return new CacheImpl<Storage::Many,Timing::Basic>(p);
	return new CacheImpl<Storage::Basic,Timing::Basic>(p);
}

Interface& nCacheImpl(Parameters &p, int mode=0)
{
	p.set("type",      "...");
	p.set("write_policy","WB");
	Interface *m;
	if (mode==1)
	    m = new CacheImpl<Storage::Many,Timing::Basic>(p);	
	else if (mode==2) {
		uint32_t sets = p.get<uint32_t>("num_sets");
	    if (sets==1)
	        m = new CacheImpl<Storage::OneWay,Timing::Basic>(p);
	    else if (sets==2)
	        m = new CacheImpl<Storage::TwoWay,Timing::Basic>(p);
	    else if (sets==4)
	        m = new CacheImpl<Storage::FourWay,Timing::Basic>(p);
	    else 
	        m = new CacheImpl<Storage::Basic,Timing::Basic>(p);
	}
	else
	    m = new CacheImpl<Storage::Basic,Timing::Basic>(p);
	return *m;
}

BOOST_AUTO_TEST_CASE( test_exception )
{
  for(int mode=0; mode <= 2; ++mode) {
	//from cache
	BOOST_CHECK_THROW(nCacheImpl(15,1,1,1,mode),invalid_argument);
	BOOST_CHECK_THROW(nCacheImpl(16,3,1,1,mode),invalid_argument);
	BOOST_CHECK_THROW(nCacheImpl(0,1,16,1,mode),invalid_argument);
	BOOST_CHECK_THROW(nCacheImpl(1,0,16,1,mode),invalid_argument);

	//num_sets
	BOOST_CHECK_NO_THROW(nCacheImpl(16,1,1,1,mode));
	BOOST_CHECK_NO_THROW(nCacheImpl(16,1,16,1,mode));
	BOOST_CHECK_THROW(nCacheImpl(16,1,32,1,mode),invalid_argument);
	BOOST_CHECK_THROW(nCacheImpl(16,1,0,1,mode),invalid_argument);
	BOOST_CHECK_THROW(nCacheImpl(16,1,5,1,mode),invalid_argument);
	BOOST_CHECK_NO_THROW(nCacheImpl(16*3,1,3,1,mode));
  }
}

BOOST_AUTO_TEST_CASE( test_no_hit_on_emtpy_w_bus )
{
	//our first implementation of cache had an error
	//that made it hit when accessed for tags completely 0
	
  for(int mode=0; mode <= 2; ++mode) {
	Parameters p;
	p.set("size",      "16");
	p.set("line_size", "1");
	p.set("num_sets",  "1");
	p.set("latency",   "0");
	p.set("name",      "name");
	Interface& c = nCacheImpl(p,mode);

	Parameters pb;
	pb.set("latency", "110");
	pb.set("name",    "name");
	pb.set("type",    "...");
	c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));

	for(int i=0;i<16;i++)
	{
		Access m(i);
		Trace mt(0);
		uint32_t lat=c.read(m,0,mt,INVALID).latency();
		BOOST_CHECK_EQUAL(lat,0+10+100u);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]);
  }
}

BOOST_AUTO_TEST_CASE( test_no_hit_on_emtpy_wo_bus )
{
	//our first implementation of cache had an error
	//that made it hit when accessed for tags completely 0
//  for(int mode=0; mode <= 2; ++mode) {
for(int mode=0; mode <= 0; ++mode) {
	Parameters p;
	p.set("size",      "16");
	p.set("line_size", "1");
	p.set("num_sets",  "1");
	p.set("latency",   "0");
	p.set("name",      "name");
	Interface& c = nCacheImpl(p,mode);

	Parameters pb;
	pb.set("latency", "100");
	pb.set("name",    "name");
	pb.set("type",    "...");
	c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));

	for(int i=0;i<16;i++)
	{
		Access m(i);
		Trace mt(0);
		uint32_t lat=c.read(m,0,mt,INVALID).latency();
		BOOST_CHECK_EQUAL(lat,0+100u);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]);
  }
}

BOOST_AUTO_TEST_CASE( test_fill_and_check_direct_map )
{
  for(int mode=0; mode <= 2; ++mode) {
	Parameters p;
	p.set("size",      "16");
	p.set("line_size", "1");
	p.set("num_sets",  "1");
	p.set("latency",   "1");
	p.set("name",      "name");
	Interface& c = nCacheImpl(p,mode);

	Parameters pb;
	pb.set("latency", "100");
	pb.set("name",    "name");
	pb.set("type",    "...");
	c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));

	for(int i=0;i<16;i++)
	{
		Access m(i);
		Trace mt(0);
		mt.add(&c,READ);
		uint32_t lat=c.read(m,0,mt,INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1+100u);
		mt.add(&c,WRITE);
		c.write(m,0,mt,INVALID);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]);
	StateObserver::transition(FULL_WARMING);
	StateObserver::transition(SIMULATION);
	for(int i=0;i<16;i++)
	{
		Access m(i);
		Trace mt(0);
		mt.add(&c,READ);
		uint32_t lat=c.read(m,200,mt,INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1u);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]*2);
  }
}

BOOST_AUTO_TEST_CASE( test_fill_and_check_2_way )
{
	for(int mode=0; mode <= 2; ++mode) {
	Parameters p;
	p.set("size",      "16");
	p.set("line_size", "1");
	p.set("num_sets",  "2");
	p.set("latency",   "1");
	p.set("name",      "name");
	Interface& c = nCacheImpl(p,mode);

	Parameters pb;
	pb.set("latency", "100");
	pb.set("name",    "name");
	pb.set("type",    "...");
	c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));

	for(int i=0;i<16;i++)
	{
		Access m(i);
		Trace mt(0);
		mt.add(&c,READ);
		uint32_t lat=c.read(m,0,mt,INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1+100u);
		mt.add(&c,WRITE);
		c.write(m,0,mt,INVALID);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]);
	StateObserver::transition(FULL_WARMING);
	StateObserver::transition(SIMULATION);
	for(int i=0;i<16;i++)
	{
		Access m(i);
		Trace mt(0);
		mt.add(&c,READ);
		uint32_t lat=c.read(m,200,mt,INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1u);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]*2);
 	}
}

BOOST_AUTO_TEST_CASE( test_fill_and_check_assoc )
{
  for(int mode=0; mode <= 2; ++mode) {
	Parameters p;
	p.set("size",      "16");
	p.set("line_size", "1");
	p.set("num_sets",  "16");
	p.set("latency",   "1");
	p.set("name",      "name");
	Interface& c = nCacheImpl(p,mode);

	Parameters pb;
	pb.set("latency", "100");
	pb.set("name",    "name");
	pb.set("type",    "...");
	c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));

	for(int i=0;i<16;i++)
	{
		Access m(i);
		Trace mt(0);
		mt.add(&c,READ);
		uint32_t lat=c.read(m,0,mt,INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1+100u);
		mt.add(&c,WRITE);
		c.write(m,0,mt,INVALID);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]);
	StateObserver::transition(FULL_WARMING);
	StateObserver::transition(SIMULATION);
	for(int i=0;i<16;i++)
	{
		Access m(i);
		Trace mt(0);
		mt.add(&c,READ);
		uint32_t lat=c.read(m,200,mt,INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1u);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]*2);
  }
}

BOOST_AUTO_TEST_CASE( test_assoc_pathological_case )
{
  for(int mode=0; mode <= 2; ++mode) {
	Parameters p;
	p.set("size",      "16");
	p.set("line_size", "1");
	p.set("num_sets",  "16");
	p.set("latency",   "1");
	p.set("name",      "name");
	Interface& c = nCacheImpl(p,mode);

	Parameters pb;
	pb.set("latency", "100");
	pb.set("name",    "name");
	pb.set("type",    "...");
	c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));
	uint time=0;
	for(int j=0;j<5;j++)
		for(int i=0;i<17;i++)
		{
			Access m(i);
			Trace mt(0);
			mt.add(&c,READ);
			uint32_t lat=c.read(m,time,mt,INVALID).latency();
			time+=lat+1;
			StateObserver::transition(FULL_WARMING);
			StateObserver::transition(SIMULATION);
			BOOST_CHECK_EQUAL(lat,1+100u);
			mt.add(&c,WRITE);
			c.write(m,time,mt,INVALID);
			time+=lat+1;
		}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]);
  }
}

BOOST_AUTO_TEST_CASE( test_fill_and_check_4_way )
{
  for(int mode=0; mode <= 2; ++mode) {
	Parameters p;
	p.set("size",      "64");
	p.set("line_size", "1");
	p.set("num_sets",  "4");
	p.set("latency",   "1");
	p.set("name",      "name");
	Interface& c = nCacheImpl(p,mode);

	Parameters pb;
	pb.set("latency", "100");
	pb.set("name",    "name");
	pb.set("type",    "...");
	c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));

	for(int i=0;i<64;i++)
	{
		Access m(i);
		Trace mt(0);
		mt.add(&c,READ);
		uint32_t lat=c.read(m,0,mt,INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1+100u);
		mt.add(&c,WRITE);
		c.write(m,0,mt,INVALID);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]);
	StateObserver::transition(FULL_WARMING);
	StateObserver::transition(SIMULATION);
	for(int i=0;i<64;i++)
	{
		Access m(i);
		Trace mt(0);
		mt.add(&c,READ);
		uint32_t lat=c.read(m,200,mt,INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1u);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]*2);
  }
}

BOOST_AUTO_TEST_CASE( test_4_way_large_tags )
{
  for(int mode=0; mode <= 2; ++mode) {
	Parameters p;
	p.set("size",      "64");
	p.set("line_size", "1");
	p.set("num_sets",  "4");
	p.set("latency",   "1");
	p.set("name",      "name");
	Interface& c = nCacheImpl(p,mode);

	Parameters pb;
	pb.set("latency", "100");
	pb.set("name",    "name");
	pb.set("type",    "...");
	c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));

	for(long long i=0;i<64;i++)
	{
		Access m(((i+1)<<55) | i);
		Trace mt(0);
		mt.add(&c,READ);
		uint32_t lat=c.read(m,0,mt,INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1+100u);
		mt.add(&c,WRITE);
		c.write(m,0,mt,INVALID);
	}
	// Check that we hit the right tag
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]);
	StateObserver::transition(FULL_WARMING);
	StateObserver::transition(SIMULATION);
	for(long long i=0;i<64;i++)
	{
		Access m(((i+1)<<55) | i);
		Trace mt(0);
		mt.add(&c,READ);
		uint32_t lat=c.read(m,200,mt,INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1u);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]*2);
	// Check that we miss with the wrong tag
	for(long long i=0;i<64;i++)
	{
		Access m(i);
		Trace mt(0);
		mt.add(&c,READ);
		uint32_t lat=c.read(m,400,mt,INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1+100u);
	}
	BOOST_CHECK_EQUAL(c["read"],(c["read_miss"]*3)/2);
  }
}
