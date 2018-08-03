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
#ifndef MACHINE_H
#define MACHINE_H

#include "cpu.h"
#include "nic.h"
#include "disk.h"
#include "libmetric.h"

class Machine : public metric
{
	typedef std::map<uint64_t,Disk*>    Disks;
	typedef std::map<uint64_t,Nic*>     Nics;
	typedef std::map<uint64_t,Cpu*>     Cpus;
	typedef std::set<TokenQueue*>       Queues;
	
	Queues queues;
	Disks disks_;
	Nics nics_;
	Cpus cpus_;

	Machine();
	~Machine();

	public:

	static inline Machine& get()
	{
		static Machine singleton;
		return singleton;
	}

	void build();
		
	void inject(uint64_t,InjectState);
	void tag(uint64_t,uint32_t,const Cotson::Inject::info_tag&);
	bool execute(uint64_t,uint32_t);

	void empty_queue(TokenQueue*tq);
	void empty_queues();

	static int cpus();
	static int disks();
	static int nics();

	static Cpu* cpu(int i);
	static Disk* disk(int i);
	static Nic* nic(int i);

	uint32_t nic_read(uint64_t d,uint64_t length,void* data,bool dma);
	uint32_t nic_write(uint64_t d,uint64_t length,void* data,bool dma);
	uint32_t nic_end(uint64_t d);

	uint32_t disk_read(uint64_t d,uint64_t block_number,uint64_t block_count,bool dma);
	uint32_t disk_write(uint64_t d,uint64_t block_number,uint64_t block_count,bool dma);
	uint32_t disk_end(uint64_t d);
};

#endif
