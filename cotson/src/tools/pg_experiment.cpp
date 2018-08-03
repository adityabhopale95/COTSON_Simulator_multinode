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

// $Id: pg_experiment.cpp 745 2016-11-25 18:35:57Z robgiorgi $
#include <pqxx/pqxx>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <stdexcept>

using namespace pqxx;
using namespace boost;
using namespace std;

int usage(const string &self)
{
    cerr << "Usage: " << self << " [host|0] command [options]" << endl;
    cerr << "Available commands are:" << endl;
    cerr << "    list " << endl;
    cerr << "    new 'description'" << endl;
    cerr << "    delete id" << endl;
    cerr << "    redo id 'description'" << endl;
    cerr << "    metrics experiment" << endl;
    throw invalid_argument("usage");
    return -1;
}

int main(int argc,char*argv[])
{
    const string self(argv[0]);
    if(argc<3)
        return usage(self);
    string host(*argv[1]=='0' ? "" : "host="+string(argv[1]));
    string command(argv[2]);
    string dbconn(host+" dbname=cotson user=cotson");
    if(command=="new") {
        if(argc!=4)
		    return usage(self);
        string description(argv[3]);
        connection n(dbconn);
        transaction<> t(n, "creating a new experiment");
        result r=t.exec("INSERT INTO experiments(description) VALUES('"+
            t.esc(description)+"'); SELECT lastval()");
        t.commit();
        int value;
        if(!r[0][0].to(value))
            throw runtime_error("experiment id cant be casted");
        cout << "created experiment " << value << endl;
    }
    else if (command=="redo") {
        if(argc!=5)
			return usage(self);
        string id(argv[3]);
        string description(argv[4]);
        connection n(dbconn);
        transaction<> t(n, "replace an experiment");
        t.exec("DELETE from experiments WHERE experiment_id="+t.esc(id)+";");
        t.exec("INSERT INTO experiments VALUES("+ t.esc(id)+",'"+t.esc(description)+"');");
        t.commit();
    }
    else if (command=="list") {
        if(argc!=3)
		    return usage(self);
        connection n(dbconn);
        transaction<> t(n, "list experiments");
        result r = t.exec("SELECT * from experiments ORDER BY experiment_id;");
        t.commit();
        for (result::const_iterator i = r.begin(); i != r.end(); ++i) {
            cout << i[0] << " '" << i[1] << "'" << endl;
        }
    }
    else if (command=="metrics") {
        if(argc!=4)
		    return usage(self);
        string expid(argv[3]);
        connection n(dbconn);
        transaction<> t(n, "list metrics");
        string q = "SELECT metric_id, name from metric_names where metric_id in "
                   " (select metric_id from heartbeats natural join metrics "
                   " where experiment_id=" + t.esc(expid) + string(") order by name");
        result r = t.exec(q);
        t.commit();
        for (result::const_iterator i = r.begin(); i != r.end(); ++i)
            cout << i[0] << " " << i[1] << endl;
    }
    else if (command=="delete") {
        if(argc!=4)
		    return usage(self);
        string expid(argv[3]);
        connection n(dbconn);
        transaction<> t(n, "delete experiment");
        t.exec("DELETE from experiments WHERE experiment_id="+t.esc(expid)+";");
        t.commit();
    }
    else
       return usage(self);
    return 0;
}

