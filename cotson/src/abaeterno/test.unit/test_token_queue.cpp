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

// $Id: test_token_queue.cpp 11 2010-01-08 13:20:58Z paolofrb $

#define BOOST_TEST_MODULE test_token_queue
#include <boost/test/included/unit_test.hpp>

#include <stdint.h>

#include "token_queue.h"
#include <typedefs.h>
#include <CMonitor.h>

BOOST_AUTO_TEST_CASE( same_structure )
{
	TokenQueue ctk;
	EVENTQUEUEINFO eqi;

	BOOST_CHECK_EQUAL(sizeof(ctk),sizeof(eqi));
	BOOST_CHECK_EQUAL(sizeof(ctk.unused1),sizeof(eqi.pFptr));
	BOOST_CHECK_EQUAL(sizeof(ctk.first),sizeof(eqi.pFirst));
	BOOST_CHECK_EQUAL(sizeof(ctk.last),sizeof(eqi.pLast));
	BOOST_CHECK_EQUAL(sizeof(ctk.head),sizeof(eqi.pHead));
	BOOST_CHECK_EQUAL(sizeof(ctk.tail),sizeof(eqi.pTail));
	BOOST_CHECK_EQUAL(sizeof(ctk.devid),sizeof(eqi.nDeviceID));
	BOOST_CHECK_EQUAL(sizeof(ctk.type),sizeof(eqi.eqtType));
	BOOST_CHECK_EQUAL(sizeof(ctk.flags),sizeof(eqi.nDeviceFlags));
	BOOST_CHECK_EQUAL(sizeof(ctk.anything),sizeof(eqi.pConsumerData));

	uint64_t a_ctk=reinterpret_cast<uint64_t>(&ctk);
	uint64_t a_eqi=reinterpret_cast<uint64_t>(&eqi);

	uint64_t a_unused1=reinterpret_cast<uint64_t>(&ctk.unused1)-a_ctk;
	uint64_t a_pFptr=reinterpret_cast<uint64_t>(&eqi.pFptr)-a_eqi;

	BOOST_CHECK_EQUAL(a_unused1,a_pFptr);

	uint64_t a_first=reinterpret_cast<uint64_t>(&ctk.first)-a_ctk;
	uint64_t a_pFirst=reinterpret_cast<uint64_t>(&eqi.pFirst)-a_eqi;

	BOOST_CHECK_EQUAL(a_first,a_pFirst);

	uint64_t a_last=reinterpret_cast<uint64_t>(&ctk.last)-a_ctk;
	uint64_t a_pLast=reinterpret_cast<uint64_t>(&eqi.pLast)-a_eqi;

	BOOST_CHECK_EQUAL(a_last,a_pLast);

	uint64_t a_head=reinterpret_cast<uint64_t>(&ctk.head)-a_ctk;
	uint64_t a_pHead=reinterpret_cast<uint64_t>(&eqi.pHead)-a_eqi;

	BOOST_CHECK_EQUAL(a_head,a_pHead);

	uint64_t a_tail=reinterpret_cast<uint64_t>(&ctk.tail)-a_ctk;
	uint64_t a_pTail=reinterpret_cast<uint64_t>(&eqi.pTail)-a_eqi;

	BOOST_CHECK_EQUAL(a_tail,a_pTail);

	uint64_t a_devid=reinterpret_cast<uint64_t>(&ctk.devid)-a_ctk;
	uint64_t a_nDeviceID=reinterpret_cast<uint64_t>(&eqi.nDeviceID)-a_eqi;

	BOOST_CHECK_EQUAL(a_devid,a_nDeviceID);

	uint64_t a_unused2=reinterpret_cast<uint64_t>(&ctk.type)-a_ctk;
	uint64_t a_eqtType=reinterpret_cast<uint64_t>(&eqi.eqtType)-a_eqi;

	BOOST_CHECK_EQUAL(a_unused2,a_eqtType);

	uint64_t a_flags=reinterpret_cast<uint64_t>(&ctk.flags)-a_ctk;
	uint64_t a_nDeviceFlags=reinterpret_cast<uint64_t>(&eqi.nDeviceFlags)-a_eqi;

	BOOST_CHECK_EQUAL(a_flags,a_nDeviceFlags);

	uint64_t a_anything=reinterpret_cast<uint64_t>(&ctk.anything)-a_ctk;
	uint64_t a_pConsumerData=reinterpret_cast<uint64_t>(&eqi.pConsumerData)-a_eqi;

	BOOST_CHECK_EQUAL(a_anything,a_pConsumerData);
}

BOOST_AUTO_TEST_CASE( same_flags )
{
	BOOST_CHECK_EQUAL(TOKEN_QUEUE_CPU,EVENTQUEUEFLAGS_PROCESSOR);
	BOOST_CHECK_EQUAL(TOKEN_QUEUE_DISK,EVENTQUEUEFLAGS_BLOCKDEV);
	BOOST_CHECK_EQUAL(TOKEN_QUEUE_NIC,EVENTQUEUEFLAGS_PACKETDEV);
}

BOOST_AUTO_TEST_CASE( same_enum )
{
	BOOST_CHECK_EQUAL(static_cast<int>(Linear),static_cast<int>(eqtLinear));
	BOOST_CHECK_EQUAL(static_cast<int>(Circular),static_cast<int>(eqtCircular));
}
