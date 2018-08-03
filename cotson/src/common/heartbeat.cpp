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

// $Id: heartbeat.cpp 745 2016-11-25 18:35:57Z robgiorgi $
#include "heartbeat.h"
#include "liboptions.h"

using namespace std;
using namespace boost;

// This should be enough for a while...
namespace {
optionR o1("heartbeat1",  "1st nested heartbeat driver");
optionR o2("heartbeat2",  "2nd nested heartbeat driver");
optionR o3("heartbeat3",  "3rd nested heartbeat driver");
optionR o4("heartbeat4",  "4th nested heartbeat driver");
optionR o5("heartbeat5",  "5th nested heartbeat driver");
optionR o6("heartbeat6",  "6th nested heartbeat driver");
optionR o7("heartbeat7",  "7th nested heartbeat driver");
}

vector< boost::shared_ptr<HeartBeat> > HeartBeat::hearts;

void HeartBeat::new_heartbeat(const string& hbs)
{
	set<string> required;
	required.insert("type");
	Parameters p=Option::from_named_option(hbs,required,"HeartBeat");
	p.set("name", hbs);
	p.track();
	boost::shared_ptr<HeartBeat> hb(Factory<HeartBeat>::create(p));
	hearts.push_back(hb);
}

void HeartBeat::init()
{
	if(hearts.empty())
	{
		string hbs("heartbeat");
		new_heartbeat(hbs); // "heartbeat" is required

		// look for "heartbeat1", "heartbeat2", ...
		int n = 1;
		bool found = true;
		while(found) 
		{
			string opt_hbs = hbs + lexical_cast<string>(n++);
			if (Option::has(opt_hbs.c_str()))
				new_heartbeat(opt_hbs);
			else 
				found=false;
		}
	}
}

void HeartBeat::add(metric& m) 
{
	init();
	for(size_t i=0; i<hearts.size(); ++i)
		hearts[i]->mets.push_back(&m); 
}

void HeartBeat::add_aggregable(metric& m) 
{
	init();
	Parameters p;
	p.set("name", "aggregator");
	p.set("type", "sum");

	for(size_t i=0; i<hearts.size(); ++i) 
	{
		boost::shared_ptr<Aggregator> a(Factory<Aggregator>::create(p));
		a->init(&m);
		hearts[i]->aggr.push_back(a);
	}
}

void HeartBeat::beat() 
{
	init();
	for(size_t i=0; i<hearts.size(); ++i) 
	{
	    HeartBeat& hb = *hearts[i];
	    for(size_t a=0; a < hb.aggr.size(); ++a)
		    hb.aggr[a]->beat();
	    hb.do_beat(); 
    }
}

void HeartBeat::last_beat() 
{
	init();
	for(size_t i=0; i<hearts.size(); ++i) 
	{
	    HeartBeat& hb = *hearts[i];
	    for(size_t a=0; a < hb.aggr.size(); ++a)
		    hb.aggr[a]->last_beat();
	    hb.do_last_beat(); 
    }
}
