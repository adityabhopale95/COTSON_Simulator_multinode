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

// $Id: test_ras.cpp 11 2010-01-08 13:20:58Z paolofrb $

#define BOOST_TEST_MODULE test_ras
#include <boost/test/included/unit_test.hpp>

#include <set>
#include <boost/lexical_cast.hpp>
#include "ras.h"

using namespace std;
#define RAS_SIZE 16
#define PC 6000

BOOST_AUTO_TEST_CASE( test_group_id )
{
	rasT ras(RAS_SIZE);
	int i;
	
	// 
	// Check initial state
	// 
	uint64_t initialIndex = ras.BackupIndex();
	BOOST_CHECK(initialIndex == 0);
	
	// 
	// Check normal RAS behaviour
	// 
	for (i=0; i<RAS_SIZE; i++)
		ras.Push(PC+i);
	for (i=0; i<RAS_SIZE; i++)
		BOOST_CHECK(ras.Pop() == static_cast<uint64_t>(PC+(RAS_SIZE-1)-i));
	
	// 
	//Check overflow
	// 
	uint64_t firstPC = PC+PC;
	for (i=0; i<RAS_SIZE+1; i++)
		ras.Push(firstPC+i); // RAS ptr overflow!
			
	BOOST_CHECK(ras.Pop() == firstPC+RAS_SIZE);
	BOOST_CHECK(ras.Pop() == firstPC+RAS_SIZE-1);
	
	// 
	//Check underflow
	//
	ras.RestoreIndex(initialIndex); // Reset to initial state
	BOOST_CHECK(ras.BackupIndex() == 0);
	ras.Pop(); // Force underflow
	ras.Push(PC*PC);
	BOOST_CHECK(ras.BackupIndex() == 0);
	BOOST_CHECK(ras.Pop() == PC*PC);
}
