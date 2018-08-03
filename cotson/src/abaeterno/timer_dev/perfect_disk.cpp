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

// $Id: perfect_disk.cpp 164 2011-02-10 16:12:51Z paolofrb $
#include "abaeterno_config.h"
#include "logger.h"
#include "error.h"
#include "disk_timer.h"
#include "liboptions.h"

using namespace std;

class PerfectDisk : public DiskTimer
{
public: 
	PerfectDisk(Parameters&);
	~PerfectDisk() {};
	uint32_t accept(bool,bool,uint64_t,uint64_t);
	void setBlocksize(uint64_t b) {blocksize = b; };
	void setCapacity (uint64_t) {};
private: 
	const double disk_latency; //in ms
	
	uint64_t blocksize;
	uint64_t num_reads;
	uint64_t num_writes;
	uint64_t bytes_read;
	uint64_t bytes_written;	
	uint64_t tot_rdelay;
	uint64_t tot_wdelay;
};

registerClass<DiskTimer,PerfectDisk> perfect_disk_c("perfect_disk");

PerfectDisk::PerfectDisk(Parameters& p) :
	disk_latency (p.get<double>("disk_latency","0")) // in ms
{
	LOG("Disk init (perfect_disk)");
	
	add("num_reads",num_reads);
	add("num_writes",num_writes);
	add("bytes_read",bytes_read);
	add("bytes_written",bytes_written);
	add("tot_rdelay",tot_rdelay);
	add("tot_wdelay",tot_wdelay);
	clear_metrics();
}

uint32_t PerfectDisk::accept(
	bool dma, 
	bool read,
	uint64_t block_start, // Initial block
	uint64_t block_count	// # of blocks
){
	
    uint64_t bytes = block_count * blocksize;
	double delay = disk_latency;

	if (read) {
		num_reads++;
		tot_rdelay += llround(delay);
		bytes_read += bytes;
		LOG((dma?"dma read":"pio read"),"start=",block_start, "blocks=",block_count, 
			"delay=",delay, "tot_rdelay=",tot_rdelay);
	}
	else {
		num_writes++;
		tot_wdelay += llround(delay);
		bytes_written += bytes;
		LOG((dma?"dma write":"pio write"), "start=",block_start, "blocks=",block_count, 
			"delay=",delay, "tot_wdelay=",tot_wdelay);
	}

	return llround(delay*1000.0); //return in us
}
