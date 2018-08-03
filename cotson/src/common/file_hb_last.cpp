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

// $Id: file_hb_last.cpp 320 2012-05-21 10:58:36Z paolofrb $
#include <boost/format.hpp>
#include <fstream>
#include "heartbeat.h"
#include "liboptions.h"

class FileHBLast : public HeartBeat
{
public:
	FileHBLast(Parameters&);

protected:
	void do_beat() {}
	void do_last_beat() { dump(logfile); }

private:
	void dump(const std::string&);
	std::string logfile;
	std::string base_logfile;
};

using namespace std;
using namespace boost;

registerClass<HeartBeat,FileHBLast> file_last_c("file_last");

FileHBLast::FileHBLast(Parameters&p) :
	logfile(p.get<string>("logfile")),
	base_logfile(logfile)
{}

void FileHBLast::dump(const string& logname)
{
	ofstream log(logname.c_str());
	if(!log)
		throw runtime_error("Could not open logfile "+logname);

	format fmt("%1%%|60t|%2%"); 

	map<string,string> v=Option::requested();
	log << "Input values: " << endl << endl;
	for(map<string,string>::iterator i=v.begin();i!=v.end();i++)
		log << fmt % i->first % i->second << endl; 

	log << endl << "Output values: " << endl << endl;
	for(vector<metric*>::iterator mi=mets.begin(); mi!=mets.end(); mi++)
	{
		metric* met = *mi;
	    for(metric::iterator i=met->names_begin();i!=met->names_end();i++)
		    log << fmt % *i % (*met)[*i] << endl;
	    for(metric::iterator i=met->ratios_begin();i!=met->ratios_end();i++)
		    log << fmt % *i % met->ratio(*i) << endl;
    }

	log << "END__DATA" << endl;
	log.flush(); 
	log.close();
}
