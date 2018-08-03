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

// $Id: disksim.cpp 11 2010-01-08 13:20:58Z paolofrb $
#include "abaeterno_config.h"
#include "logger.h"
#include "cotson.h"
#include "disk_timer.h"
#include "liboptions.h"
#include "disksim_interface.h"

using namespace std;

class DiskSim : public DiskTimer
{
public: 
	DiskSim(Parameters&);
	~DiskSim();
	uint32_t accept(bool,bool,uint64_t,uint64_t);
	void setBlocksize(uint64_t);
	void setCapacity (uint64_t);
private: 
	
	struct disksim_interface *disksim;
	
	std::string param_file;
	std::string output_file;

	uint64_t blocksize;
	uint64_t capacity;
	uint64_t blocks_per_cylinder;
	uint64_t cylinder;

	uint64_t num_reads;
	uint64_t num_writes;
	uint64_t tot_rdelay;
	uint64_t tot_wdelay;
	uint64_t blocks_read;
	uint64_t blocks_written;	
};

registerClass<DiskTimer,DiskSim> disksim_c("disksim");

static double now = 0;			/* current time (ms) */
static double next_event = -1;	/* next event */
static double delay = 0;		/* latency of this request */
static int completed = 0;		/* last request was completed */
/*
 * Schedule next callback at time t.
 * Note that there is only *one* outstanding callback at any given time.
 * The callback is for the earliest event.
 */
void cotson_schedule_callback(disksim_interface_callback_t, double t, void *ctx)
{
	LOG("cotson_schedule_callback: t=", t);
	next_event = t;
}


/*
 * de-schedule a callback.
 */
void cotson_deschedule_callback(double t, void *ctx)
{
	LOG("cotson_deschedule_callback");
	next_event = -1;
}


void cotson_report_completion(double t, disksim_request *r, void *ctx)
{
	LOG("cotson_report_completion: time=", t - r->start,"(",t," - ",r->start,")");
	completed = 1;
	now = t;
	delay = t - r->start;
}



DiskSim::DiskSim(Parameters& p) :
	param_file(p.get<string>("param_file")), 
	output_file(p.get<string>("output_file")), 
	blocksize(0),
	capacity(0),
	blocks_per_cylinder(1),
	cylinder(0)
{
	//LOG("Disk init (disksim); initial_time (ms):", initial_time/1e6 );
	
	disksim = disksim_interface_initialize (param_file.c_str(), 
					output_file.c_str(),
					cotson_report_completion,	
					cotson_schedule_callback,
					cotson_deschedule_callback,
					0,
					0,
					0);

	/* Simple disk statistics */
	add("blocksize (by Cotson)",blocksize);
	add("capacity (by Cotson)",capacity);
	add("blocks_read",blocks_read);
	add("blocks_written",blocks_written);
	add("tot_rdelay",tot_rdelay);
	add("tot_wdelay",tot_wdelay);
	add("num_reads",num_reads);
	add("num_writes",num_writes);
	clear_metrics();
}

DiskSim::~DiskSim() 
{
	double now = Cotson::nanos();
	now=now/1e6;//convert to ms
	disksim_interface_shutdown(disksim,now);
}

void DiskSim::setBlocksize(uint64_t b) 
{ 
	LOG("setBlocksize",b); 
	blocksize = b; 
}

void DiskSim::setCapacity(uint64_t c) 
{ 
	LOG("setCapacity",c);
	capacity = c; 
}

// Return delay in usecs
uint32_t DiskSim::accept(
	bool dma, 
	bool read,
	uint64_t block_start, 
	uint64_t block_count) 
{
	double now = Cotson::nanos();
	now=now/1e6; //convert to ms
	LOG("accept -> disk#",disk_id,"; s:",block_start,"; bc:",block_count, "; time:", now, dma ? " DMA ":" PIO ", read ? "(READ)":"(WRITE)");

	struct disksim_request req;
	req.flags = read ? DISKSIM_READ : DISKSIM_WRITE;
	req.devno=0;  
	req.start=now;
	req.blkno=block_start;
	req.bytecount=block_count*blocksize;

    completed = 0;
    disksim_interface_request_arrive(disksim, now, &req);
	
	/* Process events until this I/O is completed */
	while(next_event >= 0) 
	{
		now = next_event;
		next_event = -1;
		disksim_interface_internal_event(disksim, now, 0);
	}

    if (!completed) 
	{
      fprintf(stderr, "internal error. Last event not completed: %lf\n", now);
      exit(1);
    } 
	
	//Simple statistics
    if (read) {
       num_reads++;
       tot_rdelay += llround(delay);
       blocks_read += block_count;
       LOG((dma?"dma read":"pio read"),
           "start=",block_start, "blocks=",block_count, 
           "delay=",delay, "tot_rdelay=",tot_rdelay);
   }
   else {
       num_writes++;
       tot_wdelay += llround(delay);
       blocks_written += block_count;
       LOG((dma?"dma write":"pio write"),
           "start=",block_start, "blocks=",block_count, 
           "delay=",delay, "tot_wdelay=",tot_wdelay);
   }

	return llround(delay*1000.0); //us	
}



