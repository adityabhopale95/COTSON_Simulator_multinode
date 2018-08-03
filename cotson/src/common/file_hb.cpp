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

// $Id: file_hb.cpp 320 2012-05-21 10:58:36Z paolofrb $
#include <boost/format.hpp>
#include <fstream>
#include "heartbeat.h"
#include "liboptions.h"

class FileHB : public HeartBeat
{
public:
	FileHB(Parameters&);

protected:
	void do_beat();
	void do_last_beat();

private:
	void dump(const std::string&);
	std::string logfile;
	std::string base_logfile;
};

using namespace std;
using namespace boost;

registerClass<HeartBeat,FileHB> file_c("file");

FileHB::FileHB(Parameters&p) :
	logfile(p.get<string>("logfile")),
	base_logfile(logfile)
{}

void FileHB::dump(const string& logname)
{
	ofstream log(logname.c_str());
	if(!log)
		throw runtime_error("Could not open logfile "+logname);

	format fmt("%1%%|60t|%2%"); 

	const map<string,string>& v(Option::requested());
	log << "Input values: " << endl << endl;
	for(map<string,string>::const_iterator i=v.begin();i!=v.end();i++)
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

void FileHB::do_beat()
{
	static uint64_t count=0;

	string sample_name = logfile;
	sample_name+= "_hb." + lexical_cast<string>(count);
	count++;

	dump(sample_name);
}

void FileHB::do_last_beat()
{
	dump(logfile);
}
