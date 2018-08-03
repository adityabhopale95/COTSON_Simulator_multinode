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

// workaround for a g++ excess of zeal in static checks
#pragma GCC diagnostic ignored "-Warray-bounds"

#define BOOST_TEST_MODULE test_cache_impl
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
#include "protocol.h"
#include "bus.h"
#include "simple_protocol.h"

//this next commented out include really includes the object
//file necessary for the testing (hence do not remove)
//#include "cache_timing_l2.cpp" 
//#include "main_memory.cpp" 
//#include "memory_interface.cpp" 
//#include "debug_mem.cpp" 
//#include "protocol.cpp"
//#include "simple_protocol.cpp"
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
	p.set("name",      "cache mode="+lexical_cast<string>(mode));
	p.set("type",      "...");
	p.set("write_policy","WT");

	if (mode==1)
	    return new CacheImpl<Storage::Many,Timing::Basic>(p);
	return new CacheImpl<Storage::Basic,Timing::Basic>(p);
}

Interface& nCacheImpl(Parameters &p, int mode=0)
{
	p.set("type",      "...");
	p.set("write_policy","WT");
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

Interface& nCacheImplL2(Parameters &p)
{
	p.set("type",      "...");
	p.set("write_policy","WB");
	Interface *m;
	m = new CacheImpl<Storage::Basic,Timing::L2>(p);
	return *m;
}

Memory::Trace& T()
{
   static Memory::Trace mt(0);
   mt.reset();
   return mt;
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
    string modestr=" mode"+lexical_cast<string>(mode);
	Parameters p;
	p.set("size",      "16");
	p.set("line_size", "1");
	p.set("num_sets",  "1");
	p.set("latency",   "0");
	p.set("name",      "l1"+modestr);
	Interface& c = nCacheImpl(p,mode);

 	Parameters pb;
	pb.set("name",     "BUS"+modestr);
	pb.set("latency",  "10");
	pb.set("bandwidth",  "1");
	pb.set("protocol", "SIMPLE");
	Interface* bus = new Bus<Memory::Protocol::SIMPLE>(pb);
	c.setNext(Interface::Shared(bus));

	Parameters pm;
	pm.set("latency", "100");
	pm.set("name",    "main"+modestr);
	pm.set("type",    "...");
	bus->setNext(Interface::Shared(new Main<Timing::Basic>(pm)));

	for(int i=0;i<16;i++)
	{
		Access m(i);
		uint32_t lat=c.read(m,200*i,T(),INVALID).latency();
		BOOST_CHECK_EQUAL(lat,0+10+100u);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]);
  }
}

BOOST_AUTO_TEST_CASE( test_no_hit_on_emtpy_wo_bus )
{
	//our first implementation of cache had an error
	//that made it hit when accessed for tags completely 0
//  for(int mode=0; mode <= 2; ++mode) {}
for(int mode=0; mode <= 0; ++mode) {
    string modestr=" mode"+lexical_cast<string>(mode);
	Parameters p;
	p.set("size",      "16");
	p.set("line_size", "1");
	p.set("num_sets",  "1");
	p.set("latency",   "0");
	p.set("name",      "cache"+modestr);
	Interface& c = nCacheImpl(p,mode);

	Parameters pb;
	pb.set("latency", "100");
	pb.set("name",    "main"+modestr);
	pb.set("type",    "...");
	c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));

	for(int i=0;i<16;i++)
	{
		Access m(i);
		uint32_t lat=c.read(m,0,T(),INVALID).latency();
		BOOST_CHECK_EQUAL(lat,0+100u);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]);
  }
}

BOOST_AUTO_TEST_CASE( test_fill_and_check_direct_map )
{
  for(int mode=0; mode <= 2; ++mode) {
    string modestr=" mode"+lexical_cast<string>(mode);
	Parameters p;
	p.set("size",      "16");
	p.set("line_size", "1");
	p.set("num_sets",  "1");
	p.set("latency",   "1");
	p.set("name",      "cache"+modestr);
	Interface& c = nCacheImpl(p,mode);

	Parameters pb;
	pb.set("latency", "100");
	pb.set("name",    "main"+modestr);
	pb.set("type",    "...");
	c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));

	for(int i=0;i<16;i++)
	{
		Access m(i);
		uint32_t lat=c.read(m,0,T(),INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1+100u);
		c.write(m,0,T(),INVALID);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]);
	StateObserver::transition(FULL_WARMING);
	StateObserver::transition(SIMULATION);
	for(int i=0;i<16;i++)
	{
		Access m(i);
		uint32_t lat=c.read(m,200,T(),INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1u);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]*2);
  }
}

BOOST_AUTO_TEST_CASE( test_fill_and_check_2_way )
{
  for(int mode=0; mode <= 2; ++mode) {
    string modestr=" mode"+lexical_cast<string>(mode);
	Parameters p;
	p.set("size",      "16");
	p.set("line_size", "1");
	p.set("num_sets",  "2");
	p.set("latency",   "1");
	p.set("name",      "cache"+modestr);
	Interface& c = nCacheImpl(p,mode);

	Parameters pb;
	pb.set("latency", "100");
	pb.set("name",    "main"+modestr);
	pb.set("type",    "...");
	c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));

	for(int i=0;i<16;i++)
	{
		Access m(i);
		uint32_t lat=c.read(m,0,T(),INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1+100u);
		c.write(m,0,T(),INVALID);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]);
	StateObserver::transition(FULL_WARMING);
	StateObserver::transition(SIMULATION);
	for(int i=0;i<16;i++)
	{
		Access m(i);
		uint32_t lat=c.read(m,200,T(),INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1u);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]*2);
 	}
}

BOOST_AUTO_TEST_CASE( test_fill_and_check_assoc )
{
  for(int mode=0; mode <= 2; ++mode) {
    string modestr=" mode"+lexical_cast<string>(mode);
	Parameters p;
	p.set("size",      "16");
	p.set("line_size", "1");
	p.set("num_sets",  "16");
	p.set("latency",   "1");
	p.set("name",      "cache"+modestr);
	Interface& c = nCacheImpl(p,mode);

	Parameters pb;
	pb.set("latency", "100");
	pb.set("name",    "main"+modestr);
	pb.set("type",    "...");
	c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));

	for(int i=0;i<16;i++)
	{
		Access m(i);
		uint32_t lat=c.read(m,0,T(),INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1+100u);
		c.write(m,0,T(),INVALID);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]);
	StateObserver::transition(FULL_WARMING);
	StateObserver::transition(SIMULATION);
	for(int i=0;i<16;i++)
	{
		Access m(i);
		uint32_t lat=c.read(m,200,T(),INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1u);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]*2);
  }
}

BOOST_AUTO_TEST_CASE( test_assoc_pathological_case )
{
  for(int mode=0; mode <= 2; ++mode) {
    string modestr=" mode"+lexical_cast<string>(mode);
	Parameters p;
	p.set("size",      "16");
	p.set("line_size", "1");
	p.set("num_sets",  "16");
	p.set("latency",   "1");
	p.set("name",      "cache"+modestr);
	Interface& c = nCacheImpl(p,mode);

	Parameters pb;
	pb.set("latency", "100");
	pb.set("name",    "main"+modestr);
	pb.set("type",    "...");
	c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));
	uint time=0;
	for(int j=0;j<5;j++)
		for(int i=0;i<17;i++)
		{
			Access m(i);
			uint32_t lat=c.read(m,time,T(),INVALID).latency();
			time+=lat+1;
			StateObserver::transition(FULL_WARMING);
			StateObserver::transition(SIMULATION);
			BOOST_CHECK_EQUAL(lat,1+100u);
			c.write(m,time,T(),INVALID);
			time+=lat+1;
		}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]);
  }
}

BOOST_AUTO_TEST_CASE( test_fill_and_check_4_way )
{
  for(int mode=0; mode <= 2; ++mode) {
    string modestr=" mode"+lexical_cast<string>(mode);
	Parameters p;
	p.set("size",      "64");
	p.set("line_size", "1");
	p.set("num_sets",  "4");
	p.set("latency",   "1");
	p.set("name",      "cache"+modestr);
	Interface& c = nCacheImpl(p,mode);

	Parameters pb;
	pb.set("latency", "100");
	pb.set("name",    "main"+modestr);
	pb.set("type",    "...");
	c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));

	for(int i=0;i<64;i++)
	{
		Access m(i);
		uint32_t lat=c.read(m,0,T(),INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1+100u);
		c.write(m,0,T(),INVALID);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]);
	StateObserver::transition(FULL_WARMING);
	StateObserver::transition(SIMULATION);
	for(int i=0;i<64;i++)
	{
		Access m(i);
		uint32_t lat=c.read(m,200,T(),INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1u);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]*2);
  }
}

BOOST_AUTO_TEST_CASE( test_4_way_large_tags )
{
  for(int mode=0; mode <= 2; ++mode) {
    string modestr=" mode"+lexical_cast<string>(mode);
	Parameters p;
	p.set("size",      "64");
	p.set("line_size", "1");
	p.set("num_sets",  "4");
	p.set("latency",   "1");
	p.set("name",      "cache"+modestr);
	Interface& c = nCacheImpl(p,mode);

	Parameters pb;
	pb.set("latency", "100");
	pb.set("name",    "main"+modestr);
	pb.set("type",    "...");
	c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));

	for(long long i=0;i<64;i++)
	{
		Access m(((i+1)<<55) | i);
		uint32_t lat=c.read(m,0,T(),INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1+100u);
		c.write(m,0,T(),INVALID);
	}
	// Check that we hit the right tag
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]);
	StateObserver::transition(FULL_WARMING);
	StateObserver::transition(SIMULATION);
	for(long long i=0;i<64;i++)
	{
		Access m(((i+1)<<55) | i);
		uint32_t lat=c.read(m,200,T(),INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1u);
	}
	BOOST_CHECK_EQUAL(c["read"],c["read_miss"]*2);
	// Check that we miss with the wrong tag
	for(long long i=0;i<64;i++)
	{
		Access m(i);
		uint32_t lat=c.read(m,400,T(),INVALID).latency();
		BOOST_CHECK_EQUAL(lat,1+100u);
	}
	BOOST_CHECK_EQUAL(c["read"],(c["read_miss"]*3)/2);
  }
}

BOOST_AUTO_TEST_CASE( test_lru_2_ways )
{
  Parameters p;
  p.set("size",      "64");
  p.set("line_size", "1");
  p.set("latency",   "1");

  Parameters pb;
  pb.set("latency", "100");
  pb.set("type",    "...");

  for(int mode=0; mode <= 2; ++mode) {
      string modestr=" mode"+lexical_cast<string>(mode);
      p.set("name",      "cache"+modestr);
      pb.set("name",    "main"+modestr);
      // Two ways
      p.set("num_sets",  "2");
      Interface& c = nCacheImpl(p,mode);
      c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));
      StateObserver::transition(FULL_WARMING);
      StateObserver::transition(SIMULATION);
      // Same pos, 3 different tags
      Access m1((0x1ULL << 40) | 0x7ULL); 
      Access m2((0x2ULL << 40) | 0x7ULL);
      Access m3((0x3ULL << 40) | 0x7ULL);

      // miss 1, 2
      BOOST_CHECK_EQUAL(c.read(m1,0,T(),INVALID).latency(),101u);
      BOOST_CHECK_EQUAL(c.read(m2,0,T(),INVALID).latency(),101u);

      // hit 1, 2 (LRU=1)
      BOOST_CHECK_EQUAL(c.read(m1,200,T(),INVALID).latency(),1u);
      BOOST_CHECK_EQUAL(c.read(m2,200,T(),INVALID).latency(),1u);

      // miss 3, replace 1
      BOOST_CHECK_EQUAL(c.read(m3,300,T(),INVALID).latency(),101u);

      // half miss 3
      BOOST_CHECK_EQUAL(c.read(m3,360,T(),INVALID).latency(),41u);

      // hit 2 (LRU=3)
      BOOST_CHECK_EQUAL(c.read(m2,500,T(),INVALID).latency(),1u);

      // miss 1, replace 3
      BOOST_CHECK_EQUAL(c.read(m1,500,T(),INVALID).latency(),101u);

      // hit 1,2
      BOOST_CHECK_EQUAL(c.read(m1,700,T(),INVALID).latency(),1u);
      BOOST_CHECK_EQUAL(c.read(m2,700,T(),INVALID).latency(),1u);
  }
}


BOOST_AUTO_TEST_CASE( test_lru_4_ways )
{
  Parameters p;
  p.set("size",      "64");
  p.set("line_size", "1");
  p.set("latency",   "1");

  Parameters pb;
  pb.set("latency", "100");
  pb.set("type",    "...");

  for(int mode=0; mode <= 2; ++mode) {
      string modestr=" mode"+lexical_cast<string>(mode);
      p.set("name",      "cache"+modestr);
      pb.set("name",    "main"+modestr);
      // Two ways
      p.set("num_sets",  "4");
      Interface& c = nCacheImpl(p,mode);
      c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));
      StateObserver::transition(FULL_WARMING);
      StateObserver::transition(SIMULATION);
      // Same pos, 3 different tags
      Access m1((0x1ULL << 40) | 0x7ULL); 
      Access m2((0x2ULL << 40) | 0x7ULL);
      Access m3((0x3ULL << 40) | 0x7ULL);
      Access m4((0x4ULL << 40) | 0x7ULL);
      Access m5((0x5ULL << 40) | 0x7ULL);

      // miss 1, 2, 3, 4
      BOOST_CHECK_EQUAL(c.read(m1,0,T(),INVALID).latency(),101u);
      BOOST_CHECK_EQUAL(c.read(m2,0,T(),INVALID).latency(),101u);
      BOOST_CHECK_EQUAL(c.read(m3,0,T(),INVALID).latency(),101u);
      BOOST_CHECK_EQUAL(c.read(m4,0,T(),INVALID).latency(),101u);

      // hit 3, 1, 2, 4 (LRU=3)
      BOOST_CHECK_EQUAL(c.read(m3,200,T(),INVALID).latency(),1u);
      BOOST_CHECK_EQUAL(c.read(m1,200,T(),INVALID).latency(),1u);
      BOOST_CHECK_EQUAL(c.read(m2,200,T(),INVALID).latency(),1u);
      BOOST_CHECK_EQUAL(c.read(m4,200,T(),INVALID).latency(),1u);

      // miss 5, replace 3
      BOOST_CHECK_EQUAL(c.read(m5,300,T(),INVALID).latency(),101u);

      // half miss 5
      BOOST_CHECK_EQUAL(c.read(m5,360,T(),INVALID).latency(),41u);

      // hit 1,2,4 (LRU=5)
      BOOST_CHECK_EQUAL(c.read(m1,500,T(),INVALID).latency(),1u);
      BOOST_CHECK_EQUAL(c.read(m2,500,T(),INVALID).latency(),1u);
      BOOST_CHECK_EQUAL(c.read(m4,500,T(),INVALID).latency(),1u);

      // miss 3, replace 5
      BOOST_CHECK_EQUAL(c.read(m3,500,T(),INVALID).latency(),101u);

      // hit 1,2,3,4
      BOOST_CHECK_EQUAL(c.read(m1,700,T(),INVALID).latency(),1u);
      BOOST_CHECK_EQUAL(c.read(m2,700,T(),INVALID).latency(),1u);
      BOOST_CHECK_EQUAL(c.read(m3,700,T(),INVALID).latency(),1u);
      BOOST_CHECK_EQUAL(c.read(m4,700,T(),INVALID).latency(),1u);
  }
}

BOOST_AUTO_TEST_CASE( test_full_assoc )
{
  Parameters p;
  p.set("name",      "cache");
  p.set("size",      "64");
  p.set("line_size", "1");
  p.set("latency",   "1");
  p.set("num_sets",  "64");

  Parameters pb;
  pb.set("latency", "100");
  pb.set("name",    "main");
  pb.set("type",    "...");

  for(int mode=0; mode <= 2; ++mode) {

    Interface& c = nCacheImpl(p,mode);
    c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));

    StateObserver::transition(FULL_WARMING);
    StateObserver::transition(SIMULATION);

	// All misses
    for(uint64_t i=1;i<=64;i++) {
        Access a((i << 40));
        BOOST_CHECK_EQUAL(c.read(a,0,T(),INVALID).latency(),101u);
    }
	// All hits
    for(uint64_t i=1;i<=64;i++) {
        Access a(((65-i) << 40));
        BOOST_CHECK_EQUAL(c.read(a,200,T(),INVALID).latency(),1u);
    }
	// Replace 10 elements, (LRUs are 63,62,61,60,...54)
    for(uint64_t i=1;i<=10;i++) {
        Access a((i << 30));
        BOOST_CHECK_EQUAL(c.read(a,400,T(),INVALID).latency(),101u);
    }

	// All hits
    for(uint64_t i=1;i<=10;i++) {
        Access a((i << 30));
        BOOST_CHECK_EQUAL(c.read(a,600,T(),INVALID).latency(),1u);
    }
    for(uint64_t i=1;i<=54;i++) {
        Access a((i << 40));
        BOOST_CHECK_EQUAL(c.read(a,600,T(),INVALID).latency(),1u);
    }
  }
}

BOOST_AUTO_TEST_CASE( test_write_allocate )
{

  for(int mode=0; mode <= 2; ++mode) {
    Parameters p;
    p.set("name",      "cache");
    p.set("size",      "64");
    p.set("line_size", "1");
    p.set("latency",   "1");
    p.set("num_sets",  "1");
    p.set("write_policy",  "WB");
    p.set("write_allocate", "true");

    Parameters pb;
    pb.set("latency", "100");
    pb.set("name",    "main");
    pb.set("type",    "...");

    Interface& c = nCacheImpl(p,mode);
    c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));

    StateObserver::transition(FULL_WARMING);
    StateObserver::transition(SIMULATION);

	uint64_t x=1234;
	// Write allocate all lines
    for(uint64_t i=x;i<x+64;i++) {
        Access a(i);
        BOOST_CHECK_EQUAL(c.write(a,0,T(),INVALID).latency(),101u);
    }
	// All hits
    for(uint64_t i=x;i<x+64;i++) {
        Access a(i);
        BOOST_CHECK_EQUAL(c.read(a,200,T(),INVALID).latency(),1u);
    }
  }

  for(int mode=0; mode <= 2; ++mode) {
    Parameters p;
    p.set("name",      "cache");
    p.set("size",      "64");
    p.set("line_size", "1");
    p.set("latency",   "1");
    p.set("num_sets",  "1");
    p.set("write_policy", "WT");
    p.set("write_allocate", "true");
    Parameters pb;
    pb.set("latency", "100");
    pb.set("name",    "main");
    pb.set("type",    "...");
    Interface& c = nCacheImpl(p,mode);
    c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));

    StateObserver::transition(FULL_WARMING);
    StateObserver::transition(SIMULATION);

	uint64_t x=3456;
	// Write allocate all lines
    for(uint64_t i=x;i<x+64;i++)
        BOOST_CHECK_EQUAL(c.write(Access(i),0,T(),INVALID).latency(),101u);
	// All hits
    for(uint64_t i=x;i<x+64;i++) 
        BOOST_CHECK_EQUAL(c.read(Access(i),200,T(),INVALID).latency(),1u);
  }

  for(int mode=0; mode <= 2; ++mode) {
    Parameters p;
    p.set("name",      "cache");
    p.set("size",      "64");
    p.set("line_size", "1");
    p.set("latency",   "1");
    p.set("num_sets",  "1");
    p.set("write_policy",  "WT");
    p.set("write_allocate",  "false");
    Parameters pb;
    pb.set("latency", "100");
    pb.set("name",    "main");
    pb.set("type",    "...");
    Interface& c = nCacheImpl(p,mode);
    c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));

    StateObserver::transition(FULL_WARMING);
    StateObserver::transition(SIMULATION);

	uint64_t x=5678;
	// Do not allocate any line
    for(uint64_t i=x;i<x+64;i++)
        BOOST_CHECK_EQUAL(c.write(Access(i),0,T(),INVALID).latency(),101u);
	// All misses
    for(uint64_t i=x;i<x+64;i++) 
        BOOST_CHECK_EQUAL(c.read(Access(i),200,T(),INVALID).latency(),101u);
  }
}

#if 0
BOOST_AUTO_TEST_CASE( test_l2_timing )
{   
    Parameters p;
    p.set("size",      "64");
    p.set("line_size", "1");
    p.set("num_sets",  "4");
    p.set("latency",   "1");
    p.set("name",      "l2_timing");
    p.set("timing",      "l2");
    p.set("banks",      "16");
    p.set("ports",      "4");
    p.set("interleaving_bits",      "0");
    p.set("distance",      "0");
    p.set("clock_multiplier",      "2");
    p.set("port_accesses_per_cycle",      "1");
    p.set("bank_accesses_per_cycle",      "1");
    p.set("bank_cycle",      "4");


    //Interface& c = nCacheImpl(p,mode);
    //Interface* tmp;
    //tmp= new CacheImpl<Storage::Basic,Timing::L2,WritePolicy::WriteBack>(p);
    Interface& c = nCacheImplL2(p);

    LOG("test_l2_timing");
    //Interface& c = dynamic_cast<> new CacheImpl<Storage::Basic,Timing::L2,WritePolicy::WriteBack>(p);

    Parameters pb;
    pb.set("latency", "100");
    pb.set("name",    "name");
    pb.set("type",    "...");
    c.setNext(Interface::Shared(new Main<Timing::Basic>(pb)));
    uint64_t tstamp = 1;
    for(int i=0;i<64;i++)
    {
        Access m(i);
        LOG("read to",i);
        LOG("tstamp=",tstamp);
        uint32_t lat=c.read(m,tstamp,T(),INVALID).first;
        LOG("latency= ",lat);
        //BOOST_CHECK_EQUAL(lat,1+100u);
        //c.write(m,0,T(),INVALID);
        tstamp+=2;
    }
    LOG("stressing bank 0");
    for(int i=0;i<64;i++)
    {
        Access m(0);
        LOG("read to",0);
        LOG("tstamp=",tstamp);
        uint32_t lat=c.read(m,tstamp,T(),INVALID).first;
        LOG("latency= ",lat);
        //BOOST_CHECK_EQUAL(lat,1+100u);
        //c.write(m,0,T(),INVALID);
        tstamp+=2;
    }


}
#endif
