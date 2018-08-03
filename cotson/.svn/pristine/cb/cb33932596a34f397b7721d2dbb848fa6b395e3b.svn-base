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
#include "abaeterno_config.h"
#include "cpu_timer.h"
#include "dump_gzip.h"
#include "liboptions.h"

using namespace std;

class GzipSink : public CpuTimer
{
public: 
	GzipSink(Parameters&);

	void simple_warming(const Instruction*) {}
	void full_warming(const Instruction*) {}
	void simulation(const Instruction*);
	void idle(uint64_t);

private:
	boost::scoped_ptr<DumpGzip> dump;	
	const std::string tracefile;

	uint64_t instructions;
	uint64_t cycles;
};

registerClass<CpuTimer,GzipSink> gzip_sink_c("gzip_sink");

GzipSink::GzipSink(Parameters&p) : CpuTimer(&cycles,&instructions),
	tracefile(p.get<string>("tracefile"))
{
	dump.reset(new DumpGzip(tracefile));
	if(!(*dump))
		throw invalid_argument("Error opening tracefile " + tracefile);
	
	add("cycles",cycles);
	add("instructions",instructions);
	add_ratio("ipc","instructions","cycles");   
	clear_metrics();

	trace_needs.history=1;
}

void GzipSink::simulation(const Instruction* inst)
{
	(*dump) << *inst;
	if(!(*dump))
		throw runtime_error("Error dumping data");
	++instructions;
	++cycles;
}

void GzipSink::idle(uint64_t c)
{
    cycles += c;
}

