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

// $Id$

#define BOOST_TEST_MODULE test_mesi_2levels
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
#include "storage_two_way.h"
#include "storage_four_way.h"
#include "cache_timing_basic.h"
#include "cache_timing_l2.h"

#include "cache_impl.h"

#include "protocol.h"
#include "bus.h" 
#include "cache_impl.h"

#include "mesi.h"

//this next commented out include really includes the object
//file necessary for the testing (hence do not remove)
//#include "cache_timing_l2.cpp" 
//#include "main_memory.cpp" 
//#include "memory_interface.cpp" 
//#include "memory_access.cpp" 
//#include "debug_mem.cpp" 
//#include "mesi.cpp" 
//#include "protocol.cpp" 
//#include "cast.cpp"
//#include "options.cpp"
//#include "parameters.cpp"
//#include "uint64_t_hash.cpp"
//#include "units.cpp"

uint64_t Instruction::unique_id = 0; // static member

#define TIME_TO_STABLE 100

static void f() { }
luabind::scope definition()
{
	return luabind::def("f",f);
}

using namespace std;
using namespace boost;
using namespace Memory;


// With last level WRITE BACK
BOOST_AUTO_TEST_CASE( test_mesi_states_with_bus_2levels_WB_ )
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
	pb.set("protocol", "MESI");
	Interface* bus = new Bus<Memory::Protocol::MESI>(pb);
	
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

	//Initially, all caches have cache line NOT_FOUND
	uint64_t address=100;
	uint64_t time=10;
	Access m(address);
	Trace mt(0);
	BOOST_CHECK_EQUAL(l1a->state(m,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l1b->state(m,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l2->state(m,time),  NOT_FOUND);
	
	uint32_t lat;
	
	//First read from CPU_A (from NOT_FOUND to EXCLUSIVE in L1_A)
    BOOST_TEST_MESSAGE("***_WB_ 1st Read CPU A" );	
	mt.reset();
	lat=l1a->read(m,time,mt,INVALID).latency();
	BOOST_CHECK_EQUAL(lat,0+10+20+100u);
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), EXCLUSIVE);
	BOOST_CHECK_EQUAL(l1b->state(m,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l2->state(m,time),  EXCLUSIVE);
	
	//Second read from CPU_A (from EXCLUSIVE to EXCLUSIVE in L1_A)
    BOOST_TEST_MESSAGE("***_WB_ 2nd Read CPU A" );	
	mt.reset();
	lat=l1a->read(m,time,mt,EXCLUSIVE).latency();
	BOOST_CHECK_EQUAL(lat,0u); //hit
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), EXCLUSIVE);
	BOOST_CHECK_EQUAL(l1b->state(m,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l2->state(m,time),  EXCLUSIVE);

	//Write from CPU_A (from EXCLUSIVE to MODIFIED in L1_A)
    BOOST_TEST_MESSAGE("***_WB_ 1st Write CPU A" );	
	mt.reset();
	lat=l1a->write(m,time,mt,EXCLUSIVE).latency();
	BOOST_CHECK_EQUAL(lat,0u); //hit
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), MODIFIED);
	BOOST_CHECK_EQUAL(l1b->state(m,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l2->state(m,time),  EXCLUSIVE); //WRITE BACK; no change of state

	//Read from CPU_A (from MODIFIED to MODIFIED in L1_A)
    BOOST_TEST_MESSAGE("***_WB_ 3rd Read CPU A" );	
	mt.reset();
	lat=l1a->read(m,time,mt,MODIFIED).latency();
	BOOST_CHECK_EQUAL(lat,0u); //hit
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), MODIFIED);
	BOOST_CHECK_EQUAL(l1b->state(m,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l2->state(m,time),  EXCLUSIVE); 

	//Write from CPU_A (from MODIFIED to MODIFIED in L1_A)
    BOOST_TEST_MESSAGE("***_WB_ 2nd Write CPU A" );	
	mt.reset();
	lat=l1a->write(m,time,mt,MODIFIED).latency();
	BOOST_CHECK_EQUAL(lat,0u); //hit
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), MODIFIED);
	BOOST_CHECK_EQUAL(l1b->state(m,time), NOT_FOUND);	
	BOOST_CHECK_EQUAL(l2->state(m,time),  EXCLUSIVE); 

	//Read from CPU_B (from NOT_FOUND to SHARED in L1_B and
	//from MODIFIED to SHARED in L1_A )
    BOOST_TEST_MESSAGE("***_WB_ 1st Read CPU B" );	
	mt.reset();
	lat=l1b->read(m,time,mt,NOT_FOUND).latency();
	BOOST_CHECK_EQUAL(lat,0+10+0u); //miss L1_B, snoop from L1_A
	time+=lat;
	BOOST_CHECK_EQUAL(l2->state(m,time),  EXCLUSIVE); // Before writeback
	time+=TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), SHARED);
	BOOST_CHECK_EQUAL(l1b->state(m,time), SHARED);	
	BOOST_CHECK_EQUAL(l2->state(m,time),  MODIFIED); // After writeback
	MOESI_state last_l2=MODIFIED;

	//Read from CPU_A (from SHARED to SHARED in L1_A and
	//from SHARED to SHARED in L1_A )
    BOOST_TEST_MESSAGE("***_WB_ 4th Read CPU A" );	
	mt.reset();
	lat=l1a->read(m,time,mt,SHARED).latency();
	BOOST_CHECK_EQUAL(lat,0u); //hit L1_A
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), SHARED);
	BOOST_CHECK_EQUAL(l1b->state(m,time), SHARED);	
	BOOST_CHECK_EQUAL(l2->state(m,time),  last_l2); 

	//Read from CPU_B (from SHARED to SHARED in L1_B and
	//from SHARED to SHARED in L1_A )
    BOOST_TEST_MESSAGE("***_WB_ 2nd Read CPU B" );	
	mt.reset();
	lat=l1b->read(m,time,mt,SHARED).latency();
	BOOST_CHECK_EQUAL(lat,0u); //hit L1_B
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), SHARED);
	BOOST_CHECK_EQUAL(l1b->state(m,time), SHARED);	
	BOOST_CHECK_EQUAL(l2->state(m,time),  last_l2); 

	//Write from CPU_A (from SHARED to EXCLUSIVE in L1_A and
	//from SHARED to INVALID in L1_B )
    BOOST_TEST_MESSAGE("***_WB_ 3rd Write CPU A" );	
	mt.reset();
	lat=l1a->write(m,time,mt,SHARED).latency();
	BOOST_CHECK_EQUAL(lat,10u); //hit, but need to fetch line before writing (10 is bus latency)
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), EXCLUSIVE);
	BOOST_CHECK_EQUAL(l1b->state(m,time), INVALID);	
	BOOST_CHECK_EQUAL(l2->state(m,time),  last_l2); //WRITE BACK; no change of state

	//Read from CPU_B (from INVALID to SHARED in L1_B and
	//from EXCLUSIVE to SHARED in L1_A )
    BOOST_TEST_MESSAGE("***_WB_ 3rd Read CPU B" );	
	mt.reset();
	lat=l1b->read(m,time,mt,INVALID).latency();
	BOOST_CHECK_EQUAL(lat,0+10+0u); //miss L1_B, snoop from L1_A
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), SHARED);
	BOOST_CHECK_EQUAL(l1b->state(m,time), SHARED);	
	BOOST_CHECK_EQUAL(l2->state(m,time),  last_l2); 

	//Write from CPU_B (from SHARED to EXCLUSIVE in L1_B and
	//from SHARED to INVALID in L1_A )
    BOOST_TEST_MESSAGE("***_WB_ 1st Write CPU B" );	
	mt.reset();
	lat=l1b->write(m,time,mt,SHARED).latency();
	BOOST_CHECK_EQUAL(lat,0+10+0u); 
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), INVALID);
	BOOST_CHECK_EQUAL(l1b->state(m,time), EXCLUSIVE);	
	BOOST_CHECK_EQUAL(l2->state(m,time),  last_l2); //WRITE BACK; no change of state

	//Write from CPU_A (from INVALID to MODIFIED in L1_A and
	//from EXCLUSIVE to INVALID in L1_B )
    BOOST_TEST_MESSAGE("***_WB_ 4th Write CPU A" );	
	mt.reset();
	lat=l1a->write(m,time,mt,INVALID).latency();
	BOOST_CHECK_EQUAL(lat,10u); //write miss; Invalidate other copies
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), MODIFIED);
	BOOST_CHECK_EQUAL(l1b->state(m,time), INVALID);	
	BOOST_CHECK_EQUAL(l2->state(m,time),  last_l2); //WRITE BACK; no change of state
	
	
	//NEW MEMORY LOCATION
	
	address=800;
	Access m2(address);
	//Initially, all caches have this cache line NOT_FOUND
	BOOST_CHECK_EQUAL(l1a->state(m2,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l1b->state(m2,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l2->state(m2,time),  NOT_FOUND);
	
	//Read from CPU_A (from NOT_FOUND to EXCLUSIVE in L1_A and
	//from SHARED to SHARED in L1_A )
    BOOST_TEST_MESSAGE("***_WB_ 1st Read CPU A in 2nd memory location" );	
	mt.reset();
	lat=l1a->read(m2,time,mt,NOT_FOUND).latency();
	BOOST_CHECK_EQUAL(lat,0+10+20+100u); //miss 
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m2,time), EXCLUSIVE);
	BOOST_CHECK_EQUAL(l1b->state(m2,time), NOT_FOUND);	
	BOOST_CHECK_EQUAL(l2->state(m2,time),  EXCLUSIVE); 
	
	//Read from CPU_B (from NOT_FOUND to SHARED in L1_B and
	//from EXCLUSIVE to SHARED in L1_A )
    BOOST_TEST_MESSAGE("***_WB_ 3rd Read CPU B" );	
	mt.reset();
	lat=l1b->read(m2,time,mt,NOT_FOUND).latency();
	BOOST_CHECK_EQUAL(lat,0+10+0u); //miss L1_B, snoop from L1_A
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m2,time), SHARED);
	BOOST_CHECK_EQUAL(l1b->state(m2,time), SHARED);	
	BOOST_CHECK_EQUAL(l2->state(m2,time),  EXCLUSIVE); 
	
	//NEW MEMORY LOCATION
	
	address=1600;
	Access m3(address);
	//Initially, all caches have this cache line NOT_FOUND
	BOOST_CHECK_EQUAL(l1a->state(m3,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l1b->state(m3,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l2->state(m3,time),  NOT_FOUND);
	
	//Read from CPU_A (from NOT_FOUND to EXCLUSIVE in L1_A and
	//from SHARED to SHARED in L1_A )
    BOOST_TEST_MESSAGE("***_WB_ 1st Read CPU A in 3rd memory location" );	
	mt.reset();
	lat=l1a->read(m3,time,mt,NOT_FOUND).latency();
	BOOST_CHECK_EQUAL(lat,0+10+20+100u); //miss 
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m3,time), EXCLUSIVE);
	BOOST_CHECK_EQUAL(l1b->state(m3,time), NOT_FOUND);	
	BOOST_CHECK_EQUAL(l2->state(m3,time),  EXCLUSIVE); 
	
	//Write from CPU_B (from NOT_FOUND to MODIFIED in L1_B and
	//from EXCLUSIVE to INVALID in L1_A)
    BOOST_TEST_MESSAGE("***_WB_ 1st Write CPU B in 3rd memory location" );	
	mt.reset();
	lat=l1b->write(m3,time,mt,NOT_FOUND).latency();
	BOOST_CHECK_EQUAL(lat,10u); 
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m3,time), INVALID);
	BOOST_CHECK_EQUAL(l1b->state(m3,time), MODIFIED);	
	BOOST_CHECK_EQUAL(l2->state(m3,time),  EXCLUSIVE); 

	//NEW MEMORY LOCATION
	
	address=2400;
	Access m4(address);
	//Initially, all caches have this cache line NOT_FOUND
	BOOST_CHECK_EQUAL(l1a->state(m4,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l1b->state(m4,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l2->state(m4,time),  NOT_FOUND);
	
	//Write from CPU_A (from NOT_FOUND to MODIFIED in L1_A)
    BOOST_TEST_MESSAGE("***_WB_ 1st Write CPU A" );	
	mt.reset();
	lat=l1a->write(m4,time,mt,NOT_FOUND).latency();
	BOOST_CHECK_EQUAL(lat,0+10+20+100u); //miss 
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m4,time), MODIFIED);
	BOOST_CHECK_EQUAL(l1b->state(m4,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l2->state(m4,time),  EXCLUSIVE); //Line comes from memory

	//Write from CPU_B (from NOT_FOUND to MODIFIED in L1_B and
	//from MODIFIED to INVALID in L1_A)
    BOOST_TEST_MESSAGE("***_WB_ 1st Write CPU B in 4th memory location" );	
	mt.reset();
	lat=l1b->write(m4,time,mt,NOT_FOUND).latency();
	BOOST_CHECK_EQUAL(lat,10u); 
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m4,time), INVALID);
	BOOST_CHECK_EQUAL(l1b->state(m4,time), MODIFIED);	
	BOOST_CHECK_EQUAL(l2->state(m4,time),  EXCLUSIVE); 
	
}

// With last level WRITE THROUGH
BOOST_AUTO_TEST_CASE( test_mesi_states_with_bus_2levels_WT_ )
{
	
	//CPU A - L1
	Parameters p1a;
	p1a.set("size",      "16");
	p1a.set("line_size", "1");
	p1a.set("num_sets",  "1");
	p1a.set("latency",   "0");
	p1a.set("type",      "...");
	p1a.set("write_policy","WT");
	p1a.set("write_allocate","true");
	p1a.set("name",      "L1-CPU_A");
	Interface* l1a = new CacheImpl<Storage::Basic,Timing::Basic>(p1a);
 
	//CPU B - L1
	Parameters p1b;
	p1b.set("size",      "16");
	p1b.set("line_size", "1");
	p1b.set("num_sets",  "1");
	p1b.set("latency",   "0");
	p1b.set("type",      "...");
	p1b.set("write_policy","WT");
	p1b.set("write_allocate","true");
	p1b.set("name",      "L1-CPU_B");
	Interface* l1b = new CacheImpl<Storage::Basic,Timing::Basic>(p1b);
 
	//BUS
 	Parameters pb;
	pb.set("name",     "BUS");
	pb.set("latency",  "10");
	pb.set("bandwidth",  "1");
	pb.set("protocol", "MESI");
	Interface* bus = new Bus<Memory::Protocol::MESI>(pb);
	
	//L2
	Parameters p2;
	p2.set("size",      "16");
	p2.set("line_size", "1");
	p2.set("num_sets",  "1");
	p2.set("latency",   "20");
	p2.set("type",      "...");
	p2.set("write_policy","WT");
	p2.set("write_allocate","true");
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

	//Initially, all caches have cache line NOT_FOUND
	uint64_t address=100;
	uint64_t time=10;
	Access m(address);
	Trace mt(0);
	BOOST_CHECK_EQUAL(l1a->state(m,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l1b->state(m,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l2->state(m,time),  NOT_FOUND);
	
	uint32_t lat;
	
	//First read from CPU_A (from INVALID to EXCLUSIVE in L1_A & L2)
    BOOST_TEST_MESSAGE("***_WT_ 1st Read CPU A" );	
	mt.reset();
	lat=l1a->read(m,time,mt,INVALID).latency();
	BOOST_CHECK_EQUAL(lat,0+10+20+100u);
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), EXCLUSIVE);
	BOOST_CHECK_EQUAL(l1b->state(m,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l2->state(m,time),  EXCLUSIVE);

	//Second read from CPU_A (from EXCLUSIVE to EXCLUSIVE in L1_A)
    BOOST_TEST_MESSAGE("***_WT_ 2nd Read CPU A" );	
	mt.reset();
	lat=l1a->read(m,time,mt,EXCLUSIVE).latency();
	BOOST_CHECK_EQUAL(lat,0u); //hit
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), EXCLUSIVE);
	BOOST_CHECK_EQUAL(l1b->state(m,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l2->state(m,time),  EXCLUSIVE);

	//Write from CPU_A (from EXCLUSIVE to MODIFIED in L1_A & L2)
    BOOST_TEST_MESSAGE("***_WT_ 1st Write CPU A" );	
	mt.reset();
	lat=l1a->write(m,time,mt,EXCLUSIVE).latency();
	BOOST_CHECK_EQUAL(lat,10+20+100u); //total write-through latency
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), MODIFIED);
	BOOST_CHECK_EQUAL(l1b->state(m,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l2->state(m,time),  MODIFIED); //WRITE THU

	//Read from CPU_A (from MODIFIED to MODIFIED in L1_A)
    BOOST_TEST_MESSAGE("***_WT_ 3rd Read CPU A" );	
	mt.reset();
	lat=l1a->read(m,time,mt,MODIFIED).latency();
	BOOST_CHECK_EQUAL(lat,0u); //hit
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), MODIFIED);
	BOOST_CHECK_EQUAL(l1b->state(m,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l2->state(m,time),  MODIFIED); 

	//Write from CPU_A (from MODIFIED to MODIFIED in L1_A)
    BOOST_TEST_MESSAGE("***_WT_ 2nd Write CPU A" );	
	mt.reset();
	lat=l1a->write(m,time,mt,MODIFIED).latency();
	BOOST_CHECK_EQUAL(lat,10+20+100u); //total write-through latency
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), MODIFIED);
	BOOST_CHECK_EQUAL(l1b->state(m,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l2->state(m,time),  MODIFIED); 

	//Read from CPU_B (from NOT_FOUND to SHARED in L1_B and
	//from MODIFIED to SHARED in L1_A )
    BOOST_TEST_MESSAGE("***_WT_ 1st Read CPU B" );	
	mt.reset();
	lat=l1b->read(m,time,mt,NOT_FOUND).latency();
	BOOST_CHECK_EQUAL(lat,0+10+0u); //miss L1_B, snoop from L1_A
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), SHARED);
	BOOST_CHECK_EQUAL(l1b->state(m,time), SHARED);	
	BOOST_CHECK_EQUAL(l2->state(m,time),  MODIFIED); 

	//Read from CPU_A (from SHARED to SHARED in L1_A and
	//from SHARED to SHARED in L1_A )
    BOOST_TEST_MESSAGE("***_WT_ 4th Read CPU A" );	
	mt.reset();
	lat=l1a->read(m,time,mt,OWNER).latency();
	BOOST_CHECK_EQUAL(lat,0u); //hit L1_A
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), SHARED);
	BOOST_CHECK_EQUAL(l1b->state(m,time), SHARED);	
	BOOST_CHECK_EQUAL(l2->state(m,time),  MODIFIED); 

	//Read from CPU_B (from SHARED to SHARED in L1_B and
	//from SHARED to SHARED in L1_A )
    BOOST_TEST_MESSAGE("***_WT_ 2nd Read CPU B" );	
	mt.reset();
	lat=l1b->read(m,time,mt,SHARED).latency();
	BOOST_CHECK_EQUAL(lat,0u); //hit L1_B
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), SHARED);
	BOOST_CHECK_EQUAL(l1b->state(m,time), SHARED);	
	BOOST_CHECK_EQUAL(l2->state(m,time),  MODIFIED); 

	//Write from CPU_A (from SHARED to EXCLUSIVE in L1_A and
	//from SHARED to INVALID in L1_B )
    BOOST_TEST_MESSAGE("***_WT_ 3rd Write CPU A" );	
	mt.reset();
	lat=l1a->write(m,time,mt,OWNER).latency();
	BOOST_CHECK_EQUAL(lat,10+20+100u); //total write-through latency
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), EXCLUSIVE);
	BOOST_CHECK_EQUAL(l1b->state(m,time), INVALID);	
	BOOST_CHECK_EQUAL(l2->state(m,time),  MODIFIED); //WRITETHRU

	//Read from CPU_B (from INVALID to SHARED in L1_B and
	//from EXCLUSIVE to SHARED in L1_A )
    BOOST_TEST_MESSAGE("***_WT_ 3rd Read CPU B" );	
	mt.reset();
	lat=l1b->read(m,time,mt,INVALID).latency();
	BOOST_CHECK_EQUAL(lat,0+10+0u); //miss L1_B, snoop from L1_A
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), SHARED);
	BOOST_CHECK_EQUAL(l1b->state(m,time), SHARED);	
	BOOST_CHECK_EQUAL(l2->state(m,time),  MODIFIED); 

	//Write from CPU_B (from SHARED to EXCLUSIVE in L1_B and
	//from SHARED to INVALID in L1_A )
    BOOST_TEST_MESSAGE("***_WT_ 1st Write CPU B" );	
	mt.reset();
	lat=l1b->write(m,time,mt,SHARED).latency();
	BOOST_CHECK_EQUAL(lat,10+20+100u); //total write-through latency
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), INVALID);
	BOOST_CHECK_EQUAL(l1b->state(m,time), EXCLUSIVE);	
	BOOST_CHECK_EQUAL(l2->state(m,time),  MODIFIED); //WRITETHRU

	//Write from CPU_A (from INVALID to MODIFIED in L1_A and
	//from EXCLUSIVE to INVALID in L1_B )
    BOOST_TEST_MESSAGE("***_WT_ 4th Write CPU A" );	
	mt.reset();
	lat=l1a->write(m,time,mt,INVALID).latency();
	BOOST_CHECK_EQUAL(lat,10+20+100+10u); //total radx+write-through latency
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m,time), MODIFIED);
	BOOST_CHECK_EQUAL(l1b->state(m,time), INVALID);	
	BOOST_CHECK_EQUAL(l2->state(m,time),  MODIFIED);


	//NEW MEMORY LOCATION
	
	address=800;
	Access m2(address);
	//Initially, all caches have this cache line NOT_FOUND
	BOOST_CHECK_EQUAL(l1a->state(m2,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l1b->state(m2,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l2->state(m2,time),  NOT_FOUND);
	
	//Read from CPU_A (from NOT_FOUND to EXCLUSIVE in L1_A and
	//from SHARED to SHARED in L1_A )
    BOOST_TEST_MESSAGE("***_WT_ 1st Read CPU A in 2nd memory location" );	
	mt.reset();
	lat=l1a->read(m2,time,mt,NOT_FOUND).latency();
	BOOST_CHECK_EQUAL(lat,0+10+20+100u); //miss 
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m2,time), EXCLUSIVE);
	BOOST_CHECK_EQUAL(l1b->state(m2,time), NOT_FOUND);	
	BOOST_CHECK_EQUAL(l2->state(m2,time),  EXCLUSIVE); 
	
	//Read from CPU_B (from NOT_FOUND to SHARED in L1_B and
	//from EXCLUSIVE to SHARED in L1_A )
    BOOST_TEST_MESSAGE("***_WT_ 1st Read CPU B in 2nd memory location" );	
	mt.reset();
	lat=l1b->read(m2,time,mt,NOT_FOUND).latency();
	BOOST_CHECK_EQUAL(lat,0+10+0u); //miss L1_B, snoop from L1_A
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m2,time), SHARED);
	BOOST_CHECK_EQUAL(l1b->state(m2,time), SHARED);	
	BOOST_CHECK_EQUAL(l2->state(m2,time),  EXCLUSIVE); 

	//NEW MEMORY LOCATION
	
	address=1600;
	Access m3(address);
	//Initially, all caches have this cache line NOT_FOUND
	BOOST_CHECK_EQUAL(l1a->state(m3,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l1b->state(m3,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l2->state(m3,time),  NOT_FOUND);
	
	//Read from CPU_A (from NOT_FOUND to EXCLUSIVE in L1_A)
    BOOST_TEST_MESSAGE("***_WT_ 1st Read CPU A in 3rd memory location" );	
	mt.reset();
	lat=l1a->read(m3,time,mt,NOT_FOUND).latency();
	BOOST_CHECK_EQUAL(lat,0+10+20+100u); //miss 
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m3,time), EXCLUSIVE);
	BOOST_CHECK_EQUAL(l1b->state(m3,time), NOT_FOUND);	
	BOOST_CHECK_EQUAL(l2->state(m3,time),  EXCLUSIVE); 
	
	//Write from CPU_B (from NOT_FOUND to MODIFIED in L1_B and
	//from EXCLUSIVE to INVALID in L1_A)
    BOOST_TEST_MESSAGE("***_WT_ 1st Write CPU B in 3rd memory location" );	
	mt.reset();
	lat=l1b->write(m3,time,mt,NOT_FOUND).latency();
	BOOST_CHECK_EQUAL(lat,10+20+100+10u); //total write-through latency
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m3,time), INVALID);
	BOOST_CHECK_EQUAL(l1b->state(m3,time), MODIFIED);	
	BOOST_CHECK_EQUAL(l2->state(m3,time),  MODIFIED); //WRITETHRU


	//NEW MEMORY LOCATION
	
	address=2400;
	Access m4(address);
	//Initially, all caches have this cache line NOT_FOUND
	BOOST_CHECK_EQUAL(l1a->state(m4,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l1b->state(m4,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l2->state(m4,time),  NOT_FOUND);
	
	//Write from CPU_A (from NOT_FOUND to MODIFIED in L1_A)
    BOOST_TEST_MESSAGE("***_WT_ 1st Write CPU A in 4th memory location" );	
	mt.reset();
	lat=l1a->write(m4,time,mt,NOT_FOUND).latency();
	BOOST_CHECK_EQUAL(lat,0+10+20+100+10+20u); //miss 
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m4,time), MODIFIED);
	BOOST_CHECK_EQUAL(l1b->state(m4,time), NOT_FOUND);
	BOOST_CHECK_EQUAL(l2->state(m4,time),  MODIFIED); 

	//Write from CPU_B (from NOT_FOUND to MODIFIED in L1_B and
	//from MODIFIED to INVALID in L1_A)
    BOOST_TEST_MESSAGE("***_WB_ 1st Write CPU B in 4th memory location" );	
	mt.reset();
	lat=l1b->write(m4,time,mt,NOT_FOUND).latency();
	BOOST_CHECK_EQUAL(lat,10+20+100+10u); //total readx+write-through latency
	time+=lat+TIME_TO_STABLE;
	BOOST_CHECK_EQUAL(l1a->state(m4,time), INVALID);
	BOOST_CHECK_EQUAL(l1b->state(m4,time), MODIFIED);	
	BOOST_CHECK_EQUAL(l2->state(m4,time),  MODIFIED); 
	
}

// With last level WRITE BACK
BOOST_AUTO_TEST_CASE( test_mesi_states_with_bus_2levels_inv_WB_ )
{
	//CPU A - L1
	Parameters p1a;
	p1a.set("size",      "16");
	p1a.set("line_size", "1");
	p1a.set("num_sets",  "2");
	p1a.set("latency",   "0");
	p1a.set("type",      "...");
	p1a.set("write_policy","WB");
	p1a.set("name",      "L1-CPU_A");
	Interface* l1a = new CacheImpl<Storage::TwoWay,Timing::Basic>(p1a);

	//CPU B - L1
	Parameters p1b;
	p1b.set("size",      "16");
	p1b.set("line_size", "1");
	p1b.set("num_sets",  "2");
	p1b.set("latency",   "0");
	p1b.set("type",      "...");
	p1b.set("write_policy","WB");
	p1b.set("name",      "L1-CPU_B");
	Interface* l1b = new CacheImpl<Storage::TwoWay,Timing::Basic>(p1b);

	//BUS
 	Parameters pb;
	pb.set("name",     "BUS");
	pb.set("latency",  "10");
	pb.set("bandwidth",  "1");
	pb.set("protocol", "MESI");
	Interface* bus = new Bus<Memory::Protocol::MESI>(pb);

	//L2
	Parameters p2;
	p2.set("size",      "16");
	p2.set("line_size", "1");
	p2.set("num_sets",  "4");
	p2.set("latency",   "20");
	p2.set("type",      "...");
	p2.set("write_policy","WB");
	p2.set("name",      "L2");
	Interface* l2 = new CacheImpl<Storage::FourWay,Timing::Basic>(p2);

	// Link caches and bus
	l1a->setNext(Interface::Shared(bus));
	l1b->setNext(Interface::Shared(bus));
	bus->setNext(Interface::Shared(l2));
}

