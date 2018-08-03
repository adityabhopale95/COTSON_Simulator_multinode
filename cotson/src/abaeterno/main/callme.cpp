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

// $Id: callme.cpp 11 2010-01-08 13:20:58Z paolofrb $
#include "abaeterno_config.h"
#include "callme.h"
#include "logger.h"
#include "cotson.h"

using namespace boost;
using namespace std;

namespace {
	inline void call_second_with_first(pair<const uint64_t,CallMe::Callback> &i)
	{
		i.second(i.first);
	} 
}


void CallMe::hey(uint64_t now)
{
	LOG("hey",now);
	Wheel& wheel=get().wheel;
	
	if(wheel.empty())
		throw runtime_error("No callback found");

	Wheel::iterator ub=wheel.upper_bound(now);
	Wheel toCall(wheel.begin(),ub);
	wheel.erase(wheel.begin(),ub);
	
	subscribe();
	for_each(toCall.begin(),toCall.end(),call_second_with_first);
}

void CallMe::later(uint64_t after, Callback func)
{
	uint64_t now = Cotson::nanos();
	uint64_t when = now + after;
	LOG("CallMe::later",now,after);

	Wheel& wheel=get().wheel;

	wheel.insert(make_pair(when,func));
	subscribe();
}

void CallMe::subscribe()
{
	Wheel& wheel=get().wheel;
	if (!wheel.empty()) 
	{
		uint64_t when=wheel.begin()->first;
		LOG("subscribing at", when);
		Cotson::callback(when-Cotson::nanos());
	}
}
