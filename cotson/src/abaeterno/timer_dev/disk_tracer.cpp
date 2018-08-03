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

// $Id: disk_tracer.cpp 11 2010-01-08 13:20:58Z paolofrb $
#include "abaeterno_config.h"
#include "logger.h"
#include "cotson.h"
#include "disk_timer.h"
#include "liboptions.h"

using namespace std;
using namespace boost;

class DiskTracer : public DiskTimer
{
public: 
	DiskTracer(Parameters&);
	~DiskTracer();
	uint32_t accept(bool,bool,uint64_t,uint64_t);
	void setBlocksize(uint64_t);
	void setCapacity (uint64_t);

private: 
	boost::format trace_formatter(std::string format);
	 
	std::string tracefilename;
	std::string traceformat;
	std::ofstream trace;
	boost::format fmt;

	uint64_t num_reads;
	uint64_t num_writes;
	uint64_t blocks_read;
	uint64_t blocks_written;	
};

registerClass<DiskTimer,DiskTracer> disk_tracer_c("disk_tracer");

DiskTracer::DiskTracer(Parameters& p) :
	tracefilename(p.get<string>("tracefile")), 
	traceformat(p.get<string>("traceformat", "ascii"))
{
	LOG("Disk init");
	clear_metrics();
	
	fmt = trace_formatter(traceformat);
	fmt.exceptions(boost::io::all_error_bits ^ (boost::io::too_many_args_bit | boost::io::too_few_args_bit));
		
	trace.open(tracefilename.c_str(), ios::out);
	if(!trace.is_open())
	{
		throw runtime_error("Could not create trace file "+tracefilename); 
	}
}

DiskTracer::~DiskTracer() 
{
	trace.close();
}

void DiskTracer::setBlocksize(uint64_t b) 
{ 
	LOG("setBlocksize",b);
	// Nothing to do here
}

void DiskTracer::setCapacity(uint64_t c) // in blocks
{ 
	LOG("setCapacity",c);
	// Nothing to do here
}

uint32_t DiskTracer::accept(
	bool dma, 
	bool read,
	uint64_t start, 
	uint64_t count
) {
	LOG("accept");
	
	double now = Cotson::nanos();
	now=now/1e6; //convert to ms
	
	int DEVICE_NUMBER = 0;
	char read_letter = read ? 'R' : 'W';
	
	trace << fmt 
				% now 			// Request arrival time (ms), relative to start of simulation (0.0 ms)
				% DEVICE_NUMBER // Device number
				% start 		// Block number; first device address of the request
				% count 		// Request size
				% read		 	// read (1) or write (0)
				% read_letter	// read (R) or write (W)
				% dma 			// Direct Memory Access (DMA)?
	;
	
    if (read) {
       num_reads++;
       blocks_read += count;
       LOG((dma?"dma read":"pio read"),
           "start=",start, "blocks=",count);
   }
   else {
       num_writes++;
       blocks_written += count;
       LOG((dma?"dma write":"pio write"),
           "start=",start, "blocks=",count);
   }
	
	return(0);
}


// Returns formatter for output trace.
// This is the correct place to add new trace formats; just create 
// a new formatter for the output.
format DiskTracer::trace_formatter(string traceformat) 
{

// Using trace formats as defined by disksim (disksim_iotrace.c)

	if (!strcmp(traceformat.c_str(), "ascii")) 
	{
		return format("%1% %2% %3% %4% %5%\n");
	}	

	throw runtime_error("Trace format"+traceformat+"not recognized"); 
}
