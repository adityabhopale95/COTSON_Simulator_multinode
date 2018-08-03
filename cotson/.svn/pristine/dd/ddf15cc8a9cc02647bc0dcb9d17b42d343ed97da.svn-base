// (C) Copyright 2006-2010 Hewlett-Packard Development Company, L.P.
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
#include "fileutil.h"

#include <boost/lexical_cast.hpp>
#include <sqlite3.h>

using namespace std;
using namespace boost;

class SqliteWriter 
{
private:
    sqlite3 *db;
    bool db_open;
    vector<string> vcol;
    vector<string> vdata;

public:
    SqliteWriter(const string& dbfile):db_open(false) 
    {
        if (sqlite3_open(dbfile.c_str(),&db))
        {
            string errdesc("Can't open sqlite db: "+string(sqlite3_errmsg(db)));
            sqlite3_close(db);
			ERROR(errdesc);
        }
        db_open=true;
    }
    ~SqliteWriter() { if (db && db_open) sqlite3_close(db); }

    int exec(const string& sql) 
	{
		if (!db_open)
		    return 0;
		// cout << "SQLITE: " << sql << endl;
        char **result;
		char *errmsg;
        int nrow,ncol;
        vcol.clear();
		vdata.clear();
		// FIXME: primitive lock checking - should use the unlock_notify feature
		int rc = 0;
		do 
		    rc = sqlite3_get_table(db,sql.c_str(),&result,&nrow,&ncol,&errmsg);
		while (rc==SQLITE_BUSY || rc==SQLITE_LOCKED);
        if(rc == SQLITE_OK)
	    {
            for(int i=0;i<ncol;++i)
                vcol.push_back(result[i]);   // column names
            for(int i=0;i<ncol*nrow;++i) // flat data
  	            vdata.push_back(result[ncol+i]);
            sqlite3_free_table(result);
        }
		else 
			ERROR(errmsg);
        return rc;
    }
	void begin_transaction() { exec("BEGIN TRANSACTION;"); }
	void end_transaction() { exec("END TRANSACTION;"); }

	inline bool has_table(const string& t)
	{
	    this->exec("SELECT name FROM sqlite_master WHERE name='"+t+"';");
		return !empty();
    }

	inline const string& data(int row, int col) const
	{
	    return vdata[row * vcol.size() + col];
	}
	inline const string& column(int col) const
	{
	    return vcol[col];
	}
	inline const bool empty() const { return vdata.empty(); }
};

static const char *create_db = "\
CREATE TABLE experiments (\
    experiment_id  integer PRIMARY KEY, \
	description    text\
); \
CREATE TABLE parameters (\
    experiment_id integer REFERENCES experiments, \
	machine_id    integer NOT NULL, \
	name          varchar(256) NOT NULL,\
	value         varchar(256) NOT NULL,\
	CONSTRAINT non_repeated_options UNIQUE(experiment_id,machine_id,name)\
); \
CREATE TABLE heartbeats (\
	heartbeat_id  integer PRIMARY KEY,\
	heartbeat_seq integer NOT NULL,\
	machine_id    integer NOT NULL,\
	experiment_id integer REFERENCES experiments,\
	nanos         int8,\
	date          DATETIME DEFAULT CURRENT_TIMESTAMP, \
	CONSTRAINT non_repeated_heart_per_machine UNIQUE(heartbeat_seq,machine_id,experiment_id)\
); \
CREATE INDEX heartbeats_nanos_idx ON heartbeats(nanos);\
CREATE TABLE metric_names (\
	metric_id  integer PRIMARY KEY,\
	name       varchar(256) NOT NULL,\
	CONSTRAINT non_repeated_names UNIQUE(name)\
); \
CREATE TABLE metrics (\
	heartbeat_id integer REFERENCES heartbeats,\
	metric_id    integer REFERENCES metric_names,\
	value        float8,\
	CONSTRAINT non_repeated_metric_per_heart UNIQUE(heartbeat_id,metric_id)\
); \
CREATE INDEX metrics_metid_idx ON metrics(metric_id); \
CREATE INDEX metric_names_name_idx ON metric_names(name); \
";

class SqliteHB : public HeartBeat
{
public:
	SqliteHB(Parameters&);

protected:
	void do_beat();
	void do_last_beat();

private:
    void add(const map<string,string>&);
    void add_metric(const string&);
    string get_metric_id(const string&);
    void subscribe(metric*);
    void start_heartbeat(int);
    void beat(metric*);
	SqliteWriter db;
	string experiment_id;
	string machine_id;
	uint64_t count;
    typedef map<string,string> MetricIds;
	MetricIds ids;
	string current_heartbeat_id;
};

SqliteHB::SqliteHB(Parameters&p) :
    db(p.get<string>("dbfile")),
    experiment_id(lexical_cast<string>(p.get<int>("experiment_id"))),
	machine_id("0"),
	count(0)
{
	const char* id_opt="network.mediator_nodeid";
	// int node_id=Option::has(id_opt) ? Option::get<int>(id_opt) : 1;
	int node_id=Option::get<int>(id_opt);
	machine_id=lexical_cast<string>(node_id);
    string desc("");
	if(p.has("experiment_description"))
	    desc=p.get<string>("experiment_description");

	// If the db already exists we add to the existing one
	// This will only work if machine_id or experiment_id are different
	if (node_id == 0 || node_id==1) // only node 0 (mediator) or 1 can create the DB and experiment 
	{
		if (!db.has_table("metrics")) 
		{
	        db.begin_transaction();
            db.exec(create_db);
			db.end_transaction();
		}

	    db.begin_transaction();
	    db.exec("SELECT description FROM experiments WHERE experiment_id='" 
		    + experiment_id +"'");
	    if(!db.empty())
	        cout << "Using experiment " << experiment_id << ": " << db.data(0,0) << endl;
		else {
	        cout << "Creating experiment " << experiment_id << ": " << desc << endl;
	        db.exec("INSERT INTO experiments(experiment_id,description) VALUES('"
	            + experiment_id + "','" + desc +"')");
		}
		db.end_transaction();
	}
	else // wait for node 1 to do the job
	{
		while(!db.has_table("metrics"));
		do 
		    db.exec("SELECT description FROM experiments WHERE experiment_id='" + experiment_id +"'");
	    while(db.empty()); 
	    cout << "Using experiment " << experiment_id << ": " << db.data(0,0) << endl;
	}
}

void SqliteHB::do_beat()
{
	if(count==0)
	{
		add(Option::requested());
		for(vector<metric*>::iterator mi=mets.begin(); mi!=mets.end(); mi++)
		    subscribe(*mi);
	}
	count++;
	start_heartbeat(count);
	for(vector<metric*>::iterator mi=mets.begin(); mi!=mets.end(); mi++)
	    beat(*mi);
}

void SqliteHB::do_last_beat()
{
	do_beat();
}

void SqliteHB::add(const map<string,string>& opts)
{
	db.begin_transaction();
	for(map<string,string>::const_iterator i=opts.begin(); i!=opts.end();++i)
	{
		db.exec("INSERT INTO parameters(experiment_id,machine_id,name,value) VALUES('"+
		    experiment_id+"','"+
		    machine_id+"','"+
			i->first+"','"+
			i->second+"');");
	}
	db.end_transaction();
}

void SqliteHB::add_metric(const string& m)
{
	db.exec("SELECT metric_id FROM metric_names WHERE name='"+m+"';");
	if (db.empty())
	    db.exec("INSERT INTO metric_names(name) VALUES('"+m+"');");
}

string SqliteHB::get_metric_id(const string& name)
{
	db.exec("SELECT metric_id FROM metric_names WHERE name='"+name+"';");
	if (!db.empty())
	    return db.data(0,0);
    else
		throw runtime_error("could not get a metric_id");
    return "";
}

void SqliteHB::subscribe(metric* met)
{
	// First, insert metrics (if needed)
	db.begin_transaction();
	for(metric::iterator i=met->names_begin();i!=met->names_end();i++)
	    add_metric(*i);
	for(metric::iterator i=met->ratios_begin();i!=met->ratios_end();i++)
	    add_metric(*i);
	db.end_transaction();

	// Then get the ids
	db.begin_transaction();
	for(metric::iterator i=met->names_begin();i!=met->names_end();i++)
		ids[*i]=get_metric_id(*i);
	for(metric::iterator i=met->ratios_begin();i!=met->ratios_end();i++)
		ids[*i]=get_metric_id(*i);
	db.end_transaction();
}

void SqliteHB::start_heartbeat(int heartbeat_seq)
{
	db.begin_transaction();
	db.exec("INSERT INTO heartbeats(heartbeat_seq,experiment_id,machine_id) VALUES("+
		lexical_cast<string>(heartbeat_seq)+","+
		experiment_id+","+machine_id+"); SELECT last_insert_rowid()");
	if (!db.empty())
	    current_heartbeat_id=db.data(0,0);
	else
		throw runtime_error("could not get a heartbeat_id");
	db.end_transaction();
}

void SqliteHB::beat(metric* met)
{
	db.begin_transaction();
	for(metric::iterator i=met->names_begin();i!=met->names_end();i++)
	{
		string val = lexical_cast<string>((*met)[*i]);
		if (val != "nan" && val != "0")
		    db.exec("INSERT INTO metrics(heartbeat_id,metric_id,value) VALUES("+
			    current_heartbeat_id+","+ ids[*i]+",'"+val+"');"); 
	}
	for(metric::iterator i=met->ratios_begin();i!=met->ratios_end();i++)
	{
		string val = lexical_cast<string>(met->ratio(*i));
		if (val != "nan" && val != "0")
		   db.exec("INSERT INTO metrics(heartbeat_id,metric_id,value) VALUES("+
			    current_heartbeat_id+","+ids[*i]+",'"+val+"');"); 
	}
	// Small optimization, might go away at some point
	db.exec("UPDATE heartbeats SET nanos="+
	    lexical_cast<string>((*met)["nanos"])+
	    " WHERE heartbeat_id="+current_heartbeat_id+
	    " AND machine_id="+machine_id+";");
	db.end_transaction();
}

////////////////////////////////////////////////////
// Register class for COTSon use
////////////////////////////////////////////////////
registerClass<HeartBeat,SqliteHB> sqlite_c("sqlite");

