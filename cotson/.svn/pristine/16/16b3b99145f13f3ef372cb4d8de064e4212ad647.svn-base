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
#include "logger.h"
#include "error.h"
#include "disk_timer.h"
#include "liboptions.h"

using namespace std;

// // Usually, hard disks have 255 heads and 63 blocks/cylinder (use this by default)
// #define HEADS 					255
// #define BLOCKS_PER_CYLINDER	63
// // e.g., floppy disks have 80 cylinders, 2 heads and 18 blocks/cylinder (of 512 bytes)

class SimpleDisk : public DiskTimer
{
public: 
	SimpleDisk(Parameters&);
	~SimpleDisk();
	uint32_t accept(bool,bool,uint64_t,uint64_t);
	void setBlocksize(uint64_t);
	void setCapacity (uint64_t);
private: 
	const bool functional_disk_parameters; //Use disk parameters (capacity & blocksize) given by VM?

	//Physical disk parameters
	const uint32_t cylinders; 
	const uint32_t heads;
	const uint32_t tracks; 
	uint32_t blocks_per_cylinder; 
	uint32_t blocks_per_track; 
	uint64_t blocksize;
	const double avg_seek_lat;    //in ms
	const uint32_t rpm;
	
	uint64_t capacity; //in blocks
	double access_lat; //in ms
	
	// Bus parameters
	const double dma_thruput; //in MB/s
	const double pio_thruput; //in MB/s
	
	// Controller parameters
	const double controller_overhead; //in ms

	uint64_t last_cylinder;
	double initial_time; // in nanoseconds

	uint64_t num_reads;
	uint64_t num_writes;
	uint64_t tot_rdelay;
	uint64_t tot_wdelay;
	uint64_t bytes_read;
	uint64_t bytes_written;	
	
	double disk_seek_latency (uint64_t); //returns ms
	double disk_transfer_latency (uint64_t, uint64_t); //returns ms
	double bus_transfer_latency (uint64_t, bool); //returns ms

};

registerClass<DiskTimer,SimpleDisk> simple_disk_c("simple_disk");

// ***Data for "Seagate Barracuda ST32171W", ignoring functional parameters
//   Cylinders: 5178
//   Heads: 5
//   Tracks (cylinders*heads): 25890
//   Blocks per track: 163
//   Block size: 512 
//   Capacity: 4,220,070 blocks (= 2.16GB (25890*163*512 bytes))
//   RPM: 7200
//   AVG seek lat: 9.4 ms
//   DMA thruput: SCSI-3 Fast-20 (Ultra SCSI) : 20MB/s
//   PIO thruput: PIO mode 4 : 16.6MB/s

// ***Data for "Quantum Atlas 10k", ignoring functional parameters
//   Cylinders: 10042
//   Heads: 6
//   Tracks (cylinders*heads): 60252
//   Blocks per track: 298
//   Block size: 512 
//   Capacity: 17,955,096 blocks (= 9.19GB (60252*298*512 bytes))
//   RPM: 10000
//   AVG seek lat: 5 ms
//   DMA thruput: SCSI-3 Fast-20 (Ultra SCSI) : 20MB/s
//   PIO thruput: PIO mode 4 : 16.6MB/s

// ***Data for "Seagate Cheetah ST3146855FC ", ignoring functional parameters (DEFAULT)
//   Cylinders: 72170
//   Heads: 4
//   Tracks (cylinders*heads): 288680
//   Blocks per track: 994
//   Block size:  512
//   Capacity:  286,947,920 blocks (= 146.9GB (286,947,920*512 bytes))
//   RPM: 15000
//   AVG seek lat: 2 ms
//   DMA thruput: Fibre Channel : 4 GB/s == 425 MB/s
//   PIO thruput: PIO mode 4 : 16.6MB/s


SimpleDisk::SimpleDisk(Parameters& p) :
	functional_disk_parameters(p.get<double>("functional_disk_parameters","0")),

	cylinders(p.get<uint32_t>("cylinders","72170")), 
	heads(p.get<uint32_t>("heads","4")), 
	tracks(p.get<uint32_t>("tracks","288680")), //should be cylinders*heads
	blocks_per_track(p.get<uint32_t>("blocks_per_track","994")), 
	blocksize(p.get<uint32_t>("blocksize","512")), 

	avg_seek_lat(p.get<double>("avg_seek_lat","2")),
	rpm(p.get<uint32_t>("rpm","15000")), 

	dma_thruput(p.get<double>("dma_thruput","425M")), // B/s
	pio_thruput(p.get<double>("pio_thruput","16.6M")), // B/s
	
	controller_overhead(p.get<double>("controller_overhead","0")), //ms
	
	last_cylinder(0)
{
	LOG("Disk init (simple_disk)");
	
	if (tracks && heads && cylinders)
		ERROR_IF(tracks!=cylinders*heads,"Number of 'tracks' should be equal to 'cylinders*heads'");
		
	// The following two vars may be redefined in setCapacity()
	capacity=tracks*blocks_per_track;
	blocks_per_cylinder=capacity/cylinders; 
	LOG("blocksize=", blocksize,"; capacity=",capacity);

	access_lat=30000/rpm; 
	
	add("bytes_read",bytes_read);
	add("bytes_written",bytes_written);
	add("tot_rdelay",tot_rdelay);
	add("tot_wdelay",tot_wdelay);
	add("num_reads",num_reads);
	add("num_writes",num_writes);
	clear_metrics();
}

SimpleDisk::~SimpleDisk() {}

void SimpleDisk::setBlocksize(uint64_t b) 
{ 
	if (!functional_disk_parameters)
		return;

	LOG("setBlocksize",b);
	ERROR_IF(b == 0,"Invalid disk blocksize (0)");
	
	uint64_t factor=b/blocksize; 
	blocksize = b; 
	
	// If we use the capacity given functionally, adjust related variables
	blocks_per_track=blocks_per_track*factor;
	blocks_per_cylinder=blocks_per_cylinder*factor; 
}

void SimpleDisk::setCapacity(uint64_t c) // in blocks
{ 
	if (!functional_disk_parameters)
		return;

	LOG("setCapacity",c);
	ERROR_IF(c == 0,"Invalid disk capacity (0)");
	capacity = c; 

	// If we use the capacity given by functionally, adjust related variables
	blocks_per_track=capacity/tracks;
	blocks_per_cylinder=capacity/cylinders; 
	
}

uint32_t SimpleDisk::accept(
	bool dma, 
	bool read,
	uint64_t block_start, // Initial block
	uint64_t block_count	// # of blocks
) {
	
	if (capacity == 0 || blocksize == 0)
		WARNING("Disk request arrived before disk was configured (SimpleDisk)");
	else 	
		ERROR_IF(block_start+block_count > capacity,"Disk block out of disk size (",block_start+block_count,">",capacity,")");

	LOG("accept -> disk#",disk_id,"; s:",block_start,"; bc:",block_count, " ",  dma ? " DMA ":" PIO ", read ? "(READ)":"(WRITE)");

    uint64_t bytes = block_count * blocksize;
    
	double delay = controller_overhead 
					+ disk_seek_latency(block_start) 
					+ disk_transfer_latency(block_start, block_count) 
					+ bus_transfer_latency(bytes, dma); // in ms

	if (read) {
		num_reads++;
		tot_rdelay += llround(delay);
		bytes_read += bytes;
		LOG((dma?"dma read":"pio read"),
			"start=",block_start, "bytes=",bytes, 
			"delay=",delay, "tot_rdelay=",tot_rdelay);
	}
	else {
		num_writes++;
		tot_wdelay += llround(delay);
		bytes_written += bytes;
		LOG((dma?"dma write":"pio write"),
			"start=",block_start, "bytes=",bytes, 
			"delay=",delay, "tot_wdelay=",tot_wdelay);
   }

	last_cylinder = (block_start+block_count) / blocks_per_cylinder;

	LOG("latency: ", delay, " ms");
	return llround(delay*1000.0); // in usec
}


double SimpleDisk::disk_seek_latency(uint64_t block_start)
{
    double seek = 0;	// head's seek delay

    uint64_t curr_cyl = block_start/blocks_per_cylinder;

    if (curr_cyl != last_cylinder)
    	seek = avg_seek_lat;
		
    return seek;
}

double SimpleDisk::disk_transfer_latency(uint64_t block_start, uint64_t block_count)
{
	return access_lat;
}

double SimpleDisk::bus_transfer_latency(uint64_t bytes, bool dma)
{
    double thruput = dma ? dma_thruput : pio_thruput;
	return (bytes / thruput * 1000.0); //ms 
}

