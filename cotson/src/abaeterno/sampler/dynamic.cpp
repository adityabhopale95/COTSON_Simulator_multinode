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

// $Id: dynamic.cpp 745 2016-11-25 18:35:57Z robgiorgi $
// #define _DEBUG_THIS_
#include "abaeterno_config.h"

#include "error.h"
#include "machine.h"
#include "logger.h"
#include "cotson.h"
#include "sampler.h"
#include "liboptions.h"
#include "series.h"
#include "libmetric.h"

#include "regex.h" // C library

using namespace std;
using namespace boost;

class Dynamic : public Sampler
{
public:
	Dynamic(Parameters&);
	~Dynamic();

	StateChange changeState(SimState);

private:
	bool updateAndCheck();
	void initialize();
	void findMetrics(metric&,const string&);

	const uint64_t functional;
	const uint64_t warming;
	const uint64_t simulation;
	const double sensitivity;
	const uint32_t maxfunctional;
	const uint32_t windowsize;
	uint64_t firstfunctional;
	const vector<string> variables;

	struct MetInfo 
	{
		vector<const uint64_t*> metrics;
		vector< Series<double>* > values;
		vector<double> vlast;
		double tlast;
		string name;
		double diffnormUpdate(double);
	};

	typedef vector<MetInfo> Dmetrics;
	Dmetrics dmetrics;

	typedef map<uint,string> VarMap;
	VarMap found;

	bool flatten;
	double sigma;
	uint32_t nfunctional;
};


registerClass<Sampler,Dynamic> dynamic_c("dynamic");

Dynamic::Dynamic(Parameters&p) : 
	Sampler(SIMULATION),
	functional   (p.get<uint64_t>("functional")),
	warming      (p.get<uint64_t>("warming")),
	simulation   (p.get<uint64_t>("simulation")),
	sensitivity  (p.get<double>("sensitivity") / 100.0),
	maxfunctional(p.get<uint32_t>("maxfunctional")),
	windowsize   (p.get<uint32_t>("window","10",false)), // no track
	variables    (p.getV<string>("variable")),
	flatten      (p.get<bool>("flatten","false",false)), // no track
	sigma        (p.get<double>("sigma","1.0",false)), // no track
	nfunctional(0)
{
    firstfunctional =   (functional > simulation+warming)
			          ? functional-simulation-warming
					  : functional;
    for (uint v = 0; v < variables.size(); ++v)
	    found[v] = string("");

	if (flatten)
	    findMetrics(Machine::get(),"");
	else
	{
		for(int i=0;i<Machine::cpus();i++)
		{
			Cpu* c=Machine::cpu(i);
			findMetrics(*c,c->name()+".");
		}
		for(int i=0;i<Machine::disks();i++)
		{
			Disk* d=Machine::disk(i);
			findMetrics(*d,d->name()+".");
		}
		for(int i=0;i<Machine::nics();i++)
		{
			Nic* n=Machine::nic(i);
			findMetrics(*n,n->name()+".");
		}
	}
	
	bool errors = false;
    for (uint v = 0; v < variables.size(); ++v)
	{
        if (found[v] == "") 
		{
			WARNING("No match for dynamic metric '"+variables[v]+"'");
			errors = true;
		}
        else LOG("Match: ", variables[v],":", found[v]);
	}
    if (errors)
        ERROR("dymamic sampler");
}

void Dynamic::findMetrics(metric& met, const string& pfx)
{
    MetInfo metinfo;
	LOG("Find Metrics",pfx);
	regex_t redata;
	string ms;
    for (uint v = 0; v < variables.size(); ++v) 
    {
		regex_t* re = &redata;
		if (::regcomp(re,variables[v].c_str(),REG_ICASE) != 0)
            ERROR("Can't parse regular expression '"+variables[v]+"'");
		for (metric::iterator m=met.begin(); m!=met.end(); ++m)
		{
			string mname = *m;
			string fullname = pfx + mname;
			if (::regexec(re,fullname.c_str(),0,0,0)==0)
			{
                const uint64_t *metptr = &(met[mname]);
				Series<double>* vs = new Series<double>(windowsize,0.0);
                metinfo.metrics.push_back(metptr);
                metinfo.values.push_back(vs);
                metinfo.vlast.push_back(0.0);
                metinfo.tlast = 0.0;
				found[v] += fullname + ' ';
				ms += fullname + ' ';
		    }
        }
		::regfree(re);
    }
    if (! metinfo.metrics.empty()) 
	{
		LOG("Add Metric",ms);
		metinfo.name = ms;
        dmetrics.push_back(metinfo);
    }
}

Dynamic::~Dynamic() 
{
    for(Dmetrics::iterator mi=dmetrics.begin();mi!=dmetrics.end(); ++mi) 
        for (uint32_t i = 0; i < mi->values.size(); ++i) 
		    delete mi->values[i];
}

Sampler::StateChange Dynamic::changeState(SimState curr)
{
	switch(curr)
	{
		case FUNCTIONAL:
			return   updateAndCheck()
				   ? StateChange(FULL_WARMING,warming)
				   : StateChange(FUNCTIONAL,functional);
	
		case FULL_WARMING:
			return StateChange(SIMULATION,simulation);
		
		case SIMULATION: 
			initialize();
			return StateChange(FUNCTIONAL,firstfunctional);
		
		default: ERROR("sim state not handled here");
	}
	return StateChange(END,0);
}

double Dynamic::MetInfo::diffnormUpdate(double sigma)
{
	LOG("Update Metric",name);
    double norm2 = 0;
	double sigma2 = 4 * sigma * sigma;
	const double now = Cotson::nanos(); // absolute time
	double dt = now - tlast; // differential time
	tlast = now; // update last value
	if (dt==0) { // First time around
            for (uint32_t i = 0; i < metrics.size(); ++i)
                vlast[i] = static_cast<double>(*(metrics[i])); // current value
	    return 0;
	}
    for (uint32_t i = 0; i < metrics.size(); ++i) 
	{
		Series<double>* vs = values[i]; // series for this metric
        const double av = static_cast<double>(*(metrics[i])); // current value
        double dv = av - vlast[i]; // differential value
		vlast[i] = av; // update last value

		double x = round(dv/(dt*1e-6));  // rounded metric/us (e.g. MIPS for insts)
		double mavg = vs->mavg();        // moving average for the metric
		double mvar2 = vs->mvar2();      // moving variance^2 for the metric
		double vscale2 = mvar2 > 0 ? mvar2*sigma2 : (mavg > 0 ? mavg * mavg : 1);
		double rel2 = ((x-mavg) * (x-mavg)) / vscale2;
		norm2 += rel2;
		vs->push_back(x); // update series
		LOG(now*1e-3,i,"x",x,
		    "avg",mavg,"var",sqrt(vscale2),"b%",
			round(sqrt(rel2)*100));
	}
	return norm2 > 0 ? sqrt(norm2) : 0;
}

void Dynamic::initialize()
{
	nfunctional=0;
    for(Dmetrics::iterator mi=dmetrics.begin();mi!=dmetrics.end(); ++mi) 
	    (void)mi->diffnormUpdate(sigma);
}

bool Dynamic::updateAndCheck()
{
	bool resample=false;
	// Exceeded max functional time triggers sampling
	if (nfunctional++ >= maxfunctional) 
	{
		LOG("Maxfunctional reached");
	    resample=true;
	}
	
	// Change in metric over sensitivity for any device triggers sampling
	for(Dmetrics::iterator mi=dmetrics.begin();mi!=dmetrics.end(); ++mi)
	    if (mi->diffnormUpdate(sigma) > sensitivity)
		    resample=true;

	LOG(resample ? "Resample" : "Continue functional", nfunctional, "time", Cotson::nanos());
	return resample;
}

