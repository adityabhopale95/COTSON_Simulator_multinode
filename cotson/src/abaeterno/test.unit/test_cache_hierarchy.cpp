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

// $Id: test_cache_hierarchy.cpp 721 2015-06-03 02:29:33Z paolofrb $

// workaround for a g++ excess of zeal in static checks
#pragma GCC diagnostic ignored "-Warray-bounds"

#define BOOST_TEST_MODULE test_cache_hierarchy
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

#include "storage_basic.h"
#include "cache_timing_basic.h"
#include "cache_timing_l2.h"

#include "cache_impl.h"

#include "protocol.h"
#include "bus.h" 
#include "cache_impl.h"
#include "simple_protocol.h"

//this next commented out include really includes the object
//file necessary for the testing (hence do not remove)
//#include "cache_timing_l2.cpp" 
//#include "main_memory.cpp" 
//#include "memory_interface.cpp" 
//#include "memory_access.cpp" 
//#include "debug_mem.cpp" 
//#include "simple_protocol.cpp" 
//#include "protocol.cpp" 
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


// NO CACHE COHERENCE IN THIS TEST (NO BUS)!
BOOST_AUTO_TEST_CASE( test_hierarchy_simple_things_without_bus )
{
	//CPU A - L1
	Parameters p1a;
	p1a.set("size",      "16");
	p1a.set("line_size", "1");
	p1a.set("num_sets",  "1");
	p1a.set("latency",   "0");
	p1a.set("type",      "...");
	p1a.set("write_policy","WB");
	p1a.set("name",      "L1-CPU_A");
	Interface* l1a = new CacheImpl<Storage::Basic,Timing::Basic>(p1a);
 
	//CPU B - L1
	Parameters p1b;
	p1b.set("size",      "16");
	p1b.set("line_size", "1");
	p1b.set("num_sets",  "1");
	p1b.set("latency",   "0");
	p1b.set("type",      "...");
	p1b.set("write_policy","WB");
	p1b.set("name",      "L1-CPU_B");
	Interface* l1b = new CacheImpl<Storage::Basic,Timing::Basic>(p1b);
 
	//L2
	Parameters p2;
	p2.set("size",      "16");
	p2.set("line_size", "1");
	p2.set("num_sets",  "1");
	p2.set("latency",   "20");
	p2.set("type",      "...");
	p2.set("write_policy","WB");
	p2.set("name",      "L2");
	Interface* l2 = new CacheImpl<Storage::Basic,Timing::Basic>(p2);
	
	// Link caches and main memory
	l1a->setNext(Interface::Shared(l2));
	l1b->setNext(Interface::Shared(l2));
 
	Parameters pm;
	pm.set("latency", "100");
	pm.set("name",    "main_memory");
	pm.set("type",    "...");
	l2->setNext(Interface::Shared(new Main<Timing::Basic>(pm)));

	StateObserver::transition(SIMULATION);

	// First access
	uint64_t address=255;
	uint64_t time=10;
	Access m(address);
	Trace mt(0);
	// Initial state should be NOT_FOUND in all cache levels
	BOOST_CHECK_EQUAL(l1a->state(m,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l1b->state(m,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l2->state(m,time), NOT_FOUND);
	
	// First miss takes total miss roundtrip
	uint32_t lat=l1a->read(m,time,mt,INVALID).latency();
	BOOST_CHECK_EQUAL(lat,0+20+100u);

	// Half miss in L2 from CPU_B; total roundtrip should be the same of CPU_A
	lat=l1b->read(m,time,mt,INVALID).latency();
	BOOST_CHECK_EQUAL(lat,0+20+100u);
	
	// Second access from CPU_A (some time after) should hit L1
	time+=1000;
	lat=l1a->read(m,time,mt,INVALID).latency();
	BOOST_CHECK_EQUAL(lat,0u);
	BOOST_CHECK(l1a->state(m,time)!=INVALID);

	// Same test for CPU_B
	time+=1000;
	lat=l1b->read(m,time,mt,INVALID).latency();
	BOOST_CHECK_EQUAL(lat,0u);
	BOOST_CHECK(l1a->state(m,time)!=INVALID);
	
	// Same test for L2
	time+=1000;
	lat=l2->read(m,time,mt,INVALID).latency();
	BOOST_CHECK_EQUAL(lat,20u);
	BOOST_CHECK(l1a->state(m,time)!=INVALID);
}

// NOW WITH CACHE COHERENCE; CHECK ONLY VALID-INVALID RESULTS
BOOST_AUTO_TEST_CASE( test_moesi_states_with_bus )
{
	//CPU A - L1
	Parameters p1a;
	p1a.set("size",      "16");
	p1a.set("line_size", "1");
	p1a.set("num_sets",  "1");
	p1a.set("latency",   "0");
	p1a.set("type",      "...");
	p1a.set("write_policy","WB");
	p1a.set("name",      "L1-CPU_A");
	Interface* l1a = new CacheImpl<Storage::Basic,Timing::Basic>(p1a);
 
	//CPU B - L1
	Parameters p1b;
	p1b.set("size",      "16");
	p1b.set("line_size", "1");
	p1b.set("num_sets",  "1");
	p1b.set("latency",   "0");
	p1b.set("type",      "...");
	p1b.set("write_policy","WB");
	p1b.set("name",      "L1-CPU_B");
	Interface* l1b = new CacheImpl<Storage::Basic,Timing::Basic>(p1b);
 
	//BUS
 	Parameters pb;
	pb.set("name",     "BUS");
	pb.set("latency",  "10");
	pb.set("bandwidth",  "1");
	pb.set("protocol", "SIMPLE");
	Interface* bus = new Bus<Memory::Protocol::SIMPLE>(pb);
	
	//L2
	Parameters p2;
	p2.set("size",      "16");
	p2.set("line_size", "1");
	p2.set("num_sets",  "1");
	p2.set("latency",   "20");
	p2.set("type",      "...");
	p2.set("write_policy","WB");
	p2.set("name",      "L2");
	Interface* l2 = new CacheImpl<Storage::Basic,Timing::Basic>(p2);
 
	
	// Link caches and bus
	l1a->setNext(Interface::Shared(bus));
	l1b->setNext(Interface::Shared(bus));
	bus->setNext(Interface::Shared(l2));
 
 	// Link bus with main memory
	Parameters pm;
	pm.set("latency", "100");
	pm.set("name",    "main_memory");
	pm.set("type",    "...");
	l2->setNext(Interface::Shared(new Main<Timing::Basic>(pm)));

	StateObserver::transition(SIMULATION);

	// First access
	uint64_t address=255;
	uint64_t time=10;
	Access m(address);
	Trace mt(0);
	// Initial state should be NOT_FOUND in all cache levels
	BOOST_CHECK_EQUAL(l1a->state(m,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l1b->state(m,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l2->state(m,time), NOT_FOUND);
	
	// First miss takes total miss roundtrip
	uint32_t lat=l1a->read(m,time,mt,NOT_FOUND).latency();
	BOOST_CHECK_EQUAL(lat,0+10+20+100u);

	// Half miss in L2 from CPU_B; total roundtrip should be the same of CPU_A plus bus contention (=10)
	lat=l1b->read(m,time,mt,INVALID).latency();
	BOOST_CHECK_EQUAL(lat,0+10+20+10+100u);
	
	// Second access from CPU_A (some time after) should hit L1
	time+=1000;
	lat=l1a->read(m,time,mt,INVALID).latency();
	BOOST_CHECK_EQUAL(lat,0u);
	BOOST_CHECK(l1a->state(m,time)!=INVALID);

	// Same test for CPU_B
	time+=1000;
	lat=l1b->read(m,time,mt,INVALID).latency();
	BOOST_CHECK_EQUAL(lat,0u);
	BOOST_CHECK(l1a->state(m,time)!=INVALID);
	
	// Same test for L2
	time+=1000;
	lat=l2->read(m,time,mt,INVALID).latency();
	BOOST_CHECK_EQUAL(lat,20u);
	BOOST_CHECK(l1a->state(m,time)!=INVALID);

}

BOOST_AUTO_TEST_CASE( test_inclusivity )
{
	//CPU A - L1
	Parameters p1a;
	p1a.set("size",      "16");
	p1a.set("line_size", "1");
	p1a.set("num_sets",  "1");
	p1a.set("latency",   "0");
	p1a.set("type",      "...");
	p1a.set("write_policy","WB");
	p1a.set("write_allocate","true");
	p1a.set("name",      "L1-A");
	Interface* l1a = new CacheImpl<Storage::Basic,Timing::Basic>(p1a);
 
	//CPU B - L1
	Parameters p1b;
	p1b.set("size",      "16");
	p1b.set("line_size", "1");
	p1b.set("num_sets",  "1");
	p1b.set("latency",   "0");
	p1b.set("type",      "...");
	p1b.set("write_policy","WB");
	p1a.set("write_allocate","true");
	p1b.set("name",      "L1-B");
	Interface* l1b = new CacheImpl<Storage::Basic,Timing::Basic>(p1b);
 
	//L2
	Parameters p2;
	p2.set("size",      "32");
	p2.set("line_size", "1");
	p2.set("num_sets",  "2");
	p2.set("latency",   "20");
	p2.set("type",      "...");
	p2.set("write_policy","WT");
	p2.set("name",      "L2");
	Interface* l2 = new CacheImpl<Storage::Basic,Timing::Basic>(p2);
	
	// Link caches and main memory
	l1a->setNext(Interface::Shared(l2));
	l1b->setNext(Interface::Shared(l2));
 
	Parameters pm;
	pm.set("latency", "100");
	pm.set("name",    "main_memory");
	pm.set("type",    "...");
	l2->setNext(Interface::Shared(new Main<Timing::Basic>(pm)));

	StateObserver::transition(SIMULATION);
	uint64_t time=0;
	{
        Access ma(0x103); Trace ta(0);
	    uint32_t la=l1a->read(ma,time,ta,INVALID).latency();
	    BOOST_CHECK_EQUAL(la,0+20+100u); // miss

	    Access mb(0x203); Trace tb(0);
	    uint32_t lb=l1b->read(mb,time,tb,INVALID).latency();
	    BOOST_CHECK_EQUAL(lb,0+20+100u); // miss
	}
	time+=1000;
	{
        Access ma(0x103); Trace ta(0);
	    uint32_t la=l1a->read(ma,time,ta,INVALID).latency();
	    BOOST_CHECK_EQUAL(la,0u); // hit

	    Access mb(0x203); Trace tb(0);
	    uint32_t lb=l1b->read(mb,time,tb,INVALID).latency();
	    BOOST_CHECK_EQUAL(lb,0u); // hit
	}
	time+=1000;
	{
	    // Read two more lines at position 3 from L1A,
	    // This generates an inclusion invalidation to L1B
	    Access ma1(0x303); Trace ta1(0);
	    Access ma2(0x403); Trace ta2(0);
	    uint32_t la1=l1a->read(ma1,time,ta1,INVALID).latency();
	    uint32_t la2=l1a->read(ma2,time,ta2,INVALID).latency();
	    BOOST_CHECK_EQUAL(la1,0+20+100u); // miss
	    BOOST_CHECK_EQUAL(la2,0+20+100u); // miss
    }
	time+=1000;
	{
	    Access mb(0x203); Trace tb(0);
	    uint32_t lb2=l1b->read(mb,time,tb,INVALID).latency();
	    BOOST_CHECK_EQUAL(lb2,0+20+100u); // miss
	}
}

BOOST_AUTO_TEST_CASE( test_write_wb1_wt2 )
{
	//CPU A - L1 (writeback, write-allocate)
	Parameters p1a;
	p1a.set("size",      "16");
	p1a.set("line_size", "2");
	p1a.set("num_sets",  "1");
	p1a.set("latency",   "0");
	p1a.set("type",      "...");
	p1a.set("write_policy","WB");
	p1a.set("write_allocate","true");
	p1a.set("name",      "L1-A");
	Interface* l1a = new CacheImpl<Storage::Basic,Timing::Basic>(p1a);
 
	//CPU A - L2 (writethrough, no-write-allocate)
	Parameters p2;
	p2.set("size",      "32");
	p2.set("line_size", "2");
	p2.set("num_sets",  "1");
	p2.set("latency",   "20");
	p2.set("type",      "...");
	p2.set("write_policy","WT");
	p2.set("write_allocate","false");
	p2.set("name",      "L2");
	Interface* l2 = new CacheImpl<Storage::Basic,Timing::Basic>(p2);
	
	// Link caches and main memory
	l1a->setNext(Interface::Shared(l2));
 
	Parameters pm;
	pm.set("latency", "100");
	pm.set("name",    "main_memory");
	pm.set("type",    "...");
	l2->setNext(Interface::Shared(new Main<Timing::Basic>(pm)));

	StateObserver::transition(SIMULATION);
	uint64_t time=0;
	{ // write to a line
        Access r(0x1000); Trace t(0);
	    uint32_t la=l1a->write(r,time,t,INVALID).latency();
	    BOOST_CHECK_EQUAL(la,0+20+100u); // miss (back at t+120)
	}
	time+=100; 
	{ // write to the same line while it's coming
        Access w(0x1001); Trace t(0);
	    uint32_t la=l1a->write(w,time,t,INVALID).latency();
	    BOOST_CHECK_EQUAL(la,120-100u); // half miss
	}
	time+=1000;
	{ // read it back; because it's write allocated it's a hit
        Access r(0x1000); Trace t(0);
	    uint32_t la=l1a->read(r,time,t,INVALID).latency();
	    BOOST_CHECK_EQUAL(la,0u); // hit
	}
}

BOOST_AUTO_TEST_CASE( test_write_wt1_wt2 )
{
	//CPU A - L1 (writethrough, no-write-allocate)
	Parameters p1a;
	p1a.set("size",      "16");
	p1a.set("line_size", "2");
	p1a.set("num_sets",  "1");
	p1a.set("latency",   "0");
	p1a.set("type",      "...");
	p1a.set("write_policy","WT");
	p1a.set("write_allocate","false");
	p1a.set("name",      "L1-A");
	Interface* l1a = new CacheImpl<Storage::Basic,Timing::Basic>(p1a);
 
	//CPU A - L2 (writethrough, no-write-allocate)
	Parameters p2;
	p2.set("size",      "32");
	p2.set("line_size", "2");
	p2.set("num_sets",  "1");
	p2.set("latency",   "20");
	p2.set("type",      "...");
	p2.set("write_policy","WT");
	p2.set("write_allocate","false");
	p2.set("name",      "L2");
	Interface* l2 = new CacheImpl<Storage::Basic,Timing::Basic>(p2);
	
	// Link caches and main memory
	l1a->setNext(Interface::Shared(l2));
 
	Parameters pm;
	pm.set("latency", "100");
	pm.set("name",    "main_memory");
	pm.set("type",    "...");
	l2->setNext(Interface::Shared(new Main<Timing::Basic>(pm)));

	StateObserver::transition(SIMULATION);
	uint64_t time=0;
	{ // write to a line
        Access r(0x1000); Trace t(0);
	    uint32_t la=l1a->write(r,time,t,INVALID).latency();
	    BOOST_CHECK_EQUAL(la,0+20+100u); // miss (back at t+120)
	}
	time+=100; 
	{ // write to the same line while it's coming
	  // because it's a no-write-allocate, it's another miss
        Access w(0x1001); Trace t(0);
	    uint32_t la=l1a->write(w,time,t,INVALID).latency();
	    BOOST_CHECK_EQUAL(la,0+20+100u); // miss
	}
	time+=1000;
	{ // read it back; because it's no-write-allocate it's a miss
        Access r(0x1000); Trace t(0);
	    uint32_t la=l1a->read(r,time,t,INVALID).latency();
	    BOOST_CHECK_EQUAL(la,0+20+100u); // miss
	}
}

BOOST_AUTO_TEST_CASE( test_write_wt1_wb2 )
{
	//CPU A - L1 (writethrough, no-write-allocate)
	Parameters p1a;
	p1a.set("size",      "16");
	p1a.set("line_size", "2");
	p1a.set("num_sets",  "1");
	p1a.set("latency",   "0");
	p1a.set("type",      "...");
	p1a.set("write_policy","WT");
	p1a.set("write_allocate","false");
	p1a.set("name",      "L1-A");
	Interface* l1a = new CacheImpl<Storage::Basic,Timing::Basic>(p1a);
 
	//CPU A - L2 (writeback, write-allocate)
	Parameters p2;
	p2.set("size",      "32");
	p2.set("line_size", "2");
	p2.set("num_sets",  "1");
	p2.set("latency",   "20");
	p2.set("type",      "...");
	p2.set("write_policy","WB");
	p2.set("name",      "L2");
	Interface* l2 = new CacheImpl<Storage::Basic,Timing::Basic>(p2);
	
	// Link caches and main memory
	l1a->setNext(Interface::Shared(l2));
 
	Parameters pm;
	pm.set("latency", "100");
	pm.set("name",    "main_memory");
	pm.set("type",    "...");
	l2->setNext(Interface::Shared(new Main<Timing::Basic>(pm)));

	StateObserver::transition(SIMULATION);
	uint64_t time=0;
	{ // write to a line
        Access r(0x1000); Trace t(0);
	    uint32_t la=l1a->write(r,time,t,INVALID).latency();
	    BOOST_CHECK_EQUAL(la,0+20+100u); // miss (back at t+120)
	}
	time+=100; 
	{ // write to the same line while it's coming
	  // L1 no-alloc, L2 write-alloc, it's an L1 miss, L2 half miss
        Access w(0x1001); Trace t(0);
	    uint32_t la=l1a->write(w,time,t,INVALID).latency();
	    BOOST_CHECK_EQUAL(la,20+(120-100u));
	}
	time+=1000;
	{ // read it back, L1 no-alloc, L2 write-alloc, it's an L1 miss, L2 hit
        Access r(0x1000); Trace t(0);
	    uint32_t la=l1a->read(r,time,t,INVALID).latency();
	    BOOST_CHECK_EQUAL(la,0+20u); // miss L1/hit L2
	}
}
