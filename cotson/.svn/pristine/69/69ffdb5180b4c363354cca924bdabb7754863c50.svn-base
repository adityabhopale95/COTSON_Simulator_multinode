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
#include "error.h"
#include "libmetric.h"
#include "liboptions.h"

#include <boost/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <pqxx/pqxx>
 
using namespace pqxx;
using namespace boost;
using namespace std;

class PgsqlWriter
{
public:
	static int new_experiment(const string& dbconn,const string& description);

	PgsqlWriter(const string& cst,int exp_id,int mach_id);
	void add(const map<string,string>& opts);
	void subscribe(metric* met);
	void start_heartbeat(int heartbeat_seq);
	void beat(metric* met);

private:
    typedef map<string,string> MetricIds;

	void add_metric(const string& name);
	string get_metric_id(transaction<>& t,const string& name);

	pqxx::connection C;
	string experiment_id;
	string machine_id;
	string current_heartbeat_id;
	MetricIds ids;
};

PgsqlWriter::PgsqlWriter(const string& cst,int exp_id,int mach_id) : 
	C(cst),
	experiment_id(lexical_cast<string>(exp_id)),
	machine_id(lexical_cast<string>(mach_id))
{
}

void PgsqlWriter::add(const map<string,string>& opts)
{
	transaction<> t(C, "adding options "+experiment_id+" "+machine_id);
	for(map<string,string>::const_iterator i=opts.begin(); i!=opts.end();++i)
	{
		t.exec("INSERT INTO parameters(experiment_id,machine_id,name,value) VALUES("+
			experiment_id+","+
			machine_id+",'"+
			t.esc(i->first)+"','"+
			t.esc(i->second)+"');");
	}
	t.commit();
}

void PgsqlWriter::add_metric(const string& m)
{
	try 
	{
	    transaction<> t(C,"adding new metric");
	    if (t.exec("SELECT metric_id FROM metric_names WHERE name='"+t.esc(m)+"'").empty())
	        // This insert can fail in case of a concurrent insertion
	        t.exec("INSERT INTO metric_names(name) VALUES('"+ t.esc(m)+"');");
	    t.commit(); 
	} 
	catch(...)
	{
	    WARNING("DB insertion of metric ", m, " failed: using previous");
	}
}

string PgsqlWriter::get_metric_id(transaction<>& t,const string& name)
{
	result r=t.exec("SELECT metric_id FROM metric_names WHERE name='"+t.esc(name)+"'");
	string value;
	if(!r[0][0].to(value))
		throw runtime_error("metric id from "+name+" cannot be casted");
	return value;
}

void PgsqlWriter::subscribe(metric* met)
{
	// First, insert metrics (if needed)
	for(metric::iterator i=met->names_begin();i!=met->names_end();i++)
	    add_metric(*i);
	for(metric::iterator i=met->ratios_begin();i!=met->ratios_end();i++)
	    add_metric(*i);

	// Then get the ids
	transaction<> t(C,"getting metric ids");
	for(metric::iterator i=met->names_begin();i!=met->names_end();i++)
		ids[*i]=get_metric_id(t,*i);
	for(metric::iterator i=met->ratios_begin();i!=met->ratios_end();i++)
		ids[*i]=get_metric_id(t,*i);
	t.commit();
}

void PgsqlWriter::start_heartbeat(int heartbeat_seq)
{
	transaction<> t(C, "starting heartbeat");
	result r=t.exec(
	    "INSERT INTO heartbeats(heartbeat_seq,machine_id,experiment_id) VALUES("+
		lexical_cast<string>(heartbeat_seq)+","+
		machine_id+","+experiment_id+"); SELECT lastval()");
	t.commit();
	if(!r[0][0].to(current_heartbeat_id))
		throw runtime_error("could not get a heartbeat_id");
}

void PgsqlWriter::beat(metric* met)
{
	transaction<> t(C, "beat");
	for(metric::iterator i=met->names_begin();i!=met->names_end();i++)
	{
		string val = lexical_cast<string>((*met)[*i]);
		if (val != "nan" && val != "0")
		    t.exec("INSERT INTO metrics(heartbeat_id,metric_id,value) VALUES("+
			    current_heartbeat_id+","+ ids[*i]+",'"+val+"');"); 
	}
	for(metric::iterator i=met->ratios_begin();i!=met->ratios_end();i++)
	{
		string val = lexical_cast<string>(met->ratio(*i));
		if (val != "nan" && val != "0")
		    t.exec("INSERT INTO metrics(heartbeat_id,metric_id,value) VALUES("+
			    current_heartbeat_id+","+ids[*i]+",'"+val+"');"); 
	}

	// Small optimization, might go away at some point
	t.exec("UPDATE heartbeats SET nanos="+
	    lexical_cast<string>((*met)["nanos"])+
	    " WHERE heartbeat_id="+current_heartbeat_id+
	    " AND machine_id="+machine_id+";");

	t.commit();
}

int PgsqlWriter::new_experiment(const string& dbconn,const string& description)
{
	connection nc(dbconn);
	transaction<> t(nc, "creating a new experiment");
	result r=t.exec("INSERT INTO experiments(description) VALUES('"+
		t.esc(description)+"'); SELECT lastval()");
	t.commit();
	int value;
	if(!r[0][0].to(value))
		throw runtime_error("experiment id cant be casted");
	return value;
}

class PostgresHB : public HeartBeat
{
public:
	PostgresHB(Parameters&);

protected:
	void do_beat();
	void do_last_beat();

private:
	string dbconn;
	int experiment_id;
	int machine_id;
	uint64_t count;
	scoped_ptr<PgsqlWriter> writer;
};

PostgresHB::PostgresHB(Parameters&p) :
    dbconn(p.get<string>("dbconn")),
	machine_id(0),
	count(0)
{
	if(Option::has("network.mediator_nodeid")) 
		machine_id=Option::get<int>("network.mediator_nodeid"); 
	
	if(p.has("experiment_id"))
		experiment_id=p.get<int>("experiment_id");
	else if(p.has("experiment_description"))
		experiment_id=
			PgsqlWriter::new_experiment(dbconn,p.get<string>("experiment_description"));
	else
		throw invalid_argument("either experiment_id or experiment_description is needed for pgsql heartbeat");
	writer.reset(new PgsqlWriter(dbconn,experiment_id,machine_id));
}

void PostgresHB::do_beat()
{
	if(count==0)
	{
		writer->add(Option::requested());
		for(vector<metric*>::iterator mi=mets.begin(); mi!=mets.end(); mi++)
		    writer->subscribe(*mi);
	}
	count++;
	writer->start_heartbeat(count);
	for(vector<metric*>::iterator mi=mets.begin(); mi!=mets.end(); mi++)
	    writer->beat(*mi);
}

void PostgresHB::do_last_beat()
{
	do_beat();
}

////////////////////////////////////////////////////
// Register class for COTSon use
////////////////////////////////////////////////////
registerClass<HeartBeat,PostgresHB> pgsql_c("pgsql");
