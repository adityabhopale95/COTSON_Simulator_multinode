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

// $Id: test_series.cpp 11 2010-01-08 13:20:58Z paolofrb $

#define BOOST_TEST_MODULE test_series
#include <boost/test/included/unit_test.hpp>

#include "series.h"

using namespace std;

BOOST_AUTO_TEST_CASE( test_init )
{
	for (int s=1; s<100; ++s) {
	    Series<int> buf(s,1234);
	    for (int i=0; i<s; ++i)
 	        BOOST_CHECK_EQUAL(buf[i],1234);
    }
}

BOOST_AUTO_TEST_CASE( test_mavg )
{
    Series<double> buf(4,0);

	buf.push_back(100);
	double avg = buf.mavg();
 	BOOST_CHECK_EQUAL(avg,100);

	buf.push_back(101);
	avg = buf.mavg();
 	BOOST_CHECK_EQUAL(avg,(100+101)/2.0);

	buf.push_back(102);
	avg = buf.mavg();
 	BOOST_CHECK_EQUAL(avg,(100+101+102)/3.0);

	buf.push_back(103);
	avg = buf.mavg();
 	BOOST_CHECK_EQUAL(buf[1],103);
 	BOOST_CHECK_EQUAL(buf[2],102);
 	BOOST_CHECK_EQUAL(buf[3],101);
 	BOOST_CHECK_EQUAL(buf[0],100);
 	BOOST_CHECK_EQUAL(avg,(100+101+102+103)/4.0);

	buf.push_back(40);
	avg = buf.mavg();
 	BOOST_CHECK_EQUAL(buf[1],40);
 	BOOST_CHECK_EQUAL(buf[2],103);
 	BOOST_CHECK_EQUAL(buf[3],102);
 	BOOST_CHECK_EQUAL(buf[0],101);
 	BOOST_CHECK_EQUAL(avg,(101+102+103+40)/4.0);

	buf.push_back(50);
	avg = buf.mavg();
 	BOOST_CHECK_EQUAL(buf[1],50);
 	BOOST_CHECK_EQUAL(buf[2],40);
 	BOOST_CHECK_EQUAL(buf[3],103);
 	BOOST_CHECK_EQUAL(buf[0],102);
 	BOOST_CHECK_EQUAL(avg,(102+103+40+50)/4.0);
}

BOOST_AUTO_TEST_CASE( test_arma22 )
{
    Series<double> obs(4,0);
    Series<double> pred(4,0);

	// Test trivial case (next step predictor)
	obs.push_back(100);
	double p = obs.arma22(pred,1,0,0,0);
	pred.push_back(100);
 	BOOST_CHECK_EQUAL(p,100);

	obs.push_back(200);
	p = obs.arma22(pred,1,0,0,0);
	pred.push_back(200);
 	BOOST_CHECK_EQUAL(p,200);

	obs.push_back(300);
	p = obs.arma22(pred,1,0,0,0);
	pred.push_back(300);
 	BOOST_CHECK_EQUAL(p,300);

	// Test full ARMA
	obs.push_back(400);
	p = obs.arma22(pred,1,0.5,0.25,-0.25);
	pred.push_back(p);
 	BOOST_CHECK_EQUAL(p,550);

	obs.push_back(300);
	p = obs.arma22(pred,1,0.5,0.25,-0.25);
	pred.push_back(p);
 	BOOST_CHECK_EQUAL(p,412.5);

	obs.push_back(200);
	p = obs.arma22(pred,1,0.5,0.25,-0.25);
	pred.push_back(p);
 	BOOST_CHECK_EQUAL(p,359.375);
}
