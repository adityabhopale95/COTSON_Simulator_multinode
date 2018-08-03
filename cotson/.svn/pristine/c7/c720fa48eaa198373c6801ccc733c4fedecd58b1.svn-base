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
#ifndef BUS_H
#define BUS_H

#include "memory_interface.h"
#include "liboptions.h"
#include "timewheel.h"
#include "logger.h"
#include "error.h"

namespace Memory {

template <typename Protocol>
class Bus : public Interface {
	public:

		Bus(const Parameters&);
		MemState read(const Access&,uint64_t,Trace&, MOESI_state);
		MemState readx(const Access&,uint64_t,Trace&, MOESI_state);
		MemState write(const Access&,uint64_t,Trace&, MOESI_state);
		INLINE MOESI_state state(const Access& m, uint64_t tstamp) 
		{
			return protocol.state(prevs,m,tstamp);
		}
		INLINE void invalidate(const Access& m, uint64_t tstamp ,const Trace& mt, MOESI_state ms) 
		{
			protocol.invalidate(prevs,m,tstamp,mt,ms);
		}

		void beginSimulation() { perform_up_to(~0ULL); }
	    INLINE uint32_t item_size() const { return linesize_; }

	private:
		Protocol protocol;
		std::set<uint64_t> contention; // annotate on each access

		void perform_up_to(uint64_t); // perform 

		bool infinite_bandwidth;

		uint32_t bandwidth;
		uint32_t latency;
		
		uint64_t bw_;
		uint64_t update_;
		uint64_t wait_time_read_;
		uint64_t wait_time_readx_;
		uint64_t wait_time_write_;

		uint64_t read_;
		uint64_t readx_;
		uint64_t write_;
		const uint64_t linesize_;
};


template <typename Protocol>
Bus<Protocol>::Bus(const Parameters& p): 
	protocol(), 
	infinite_bandwidth(p.get<bool>("infinite_bandwidth","false",false)), // no track
	bandwidth(p.get<uint32_t>("bandwidth","1",false)), // no track
	latency(p.get<uint32_t>("latency")),
	linesize_(p.get<uint32_t>("linesize","64",false)) // no track
{
		name = p.get<std::string>("name");
		add("bw",bw_);
		add("update",update_);
		add_ratio("av_bw","bw","update");
		add("wait_time_read",wait_time_read_);
		add("wait_time_write",wait_time_write_);
		add("read",read_);
		add("readx",readx_);
		add("write",write_);
		add_ratio("av_wait_time_read","wait_time_read","read");
		add_ratio("av_wait_time_readx","wait_time_read","readx");
		add_ratio("av_wait_time_write","wait_time_write","write");
		
		add("",protocol);
		clear_metrics();
}

template <typename Protocol>
MemState Bus<Protocol>::read(const Access& m,uint64_t tstamp,Trace& mt, MOESI_state ms)
{
	ERROR_IF(!next.get(),"next can not be null in a Cache");
	ERROR_IF(mt.getType()==READX,"read called with a readx trace");
	perform_up_to(tstamp);
	//just annotate on the timewheel
	//uint64_t bus_latency = (infinite_bandwidth) ? 0 : latency + (contention.size() * latency) / bandwidth;
	if (infinite_bandwidth)
		ERROR_IF(contention.size()!=0,"if infinite_badwidth==true => contention.size()==0");

	uint64_t bus_latency = latency + (contention.size() * latency) / bandwidth;
	LOG(name,"bus latency= lat:",latency,"contention:",(contention.size() * latency) / bandwidth);
	LOG(name,"read",m.phys);

	read_++;
	wait_time_read_+=bus_latency;
	const MemState& nstate = protocol.read(this,next,prevs,m,tstamp,mt,ms);

	uint64_t protocol_latency=nstate.latency();
	MOESI_state new_moesi=nstate.moesi();
	if (!infinite_bandwidth)
		contention.insert(tstamp+bus_latency+protocol_latency);
	LOG(name,"read latency",bus_latency+protocol_latency);
	return MemState(bus_latency+protocol_latency,new_moesi);
}

template <typename Protocol>
MemState Bus<Protocol>::readx(const Access& m,uint64_t tstamp,Trace& mt, MOESI_state ms)
{
	ERROR_IF(!next.get(),"next can not be null in a Cache");
	ERROR_IF(mt.getType()!=READX,"readx called with wrong trace");
	perform_up_to(tstamp);
	//just annotate on the timewheel
	//uint64_t bus_latency = (infinite_bandwidth) ? 0 : latency + (contention.size() * latency) / bandwidth;
	if (infinite_bandwidth)
		ERROR_IF(contention.size()!=0,"if infinite_badwidth==true => contention.size()==0");

	uint64_t bus_latency = latency + (contention.size() * latency) / bandwidth;
	LOG(name,"bus latency= lat:",latency,"contention:",(contention.size() * latency) / bandwidth);
	LOG(name,"readX",m.phys);

	readx_++;
	wait_time_readx_+=bus_latency;
	const MemState& nstate = protocol.readx(this,next,prevs,m,tstamp,mt,ms);

	uint64_t protocol_latency=nstate.latency();
	MOESI_state new_moesi=nstate.moesi();
	if (!infinite_bandwidth)
		contention.insert(tstamp+bus_latency+protocol_latency);
	LOG(name,"readx latency",bus_latency+protocol_latency);
	return MemState(bus_latency+protocol_latency,new_moesi);
}

template <typename Protocol>
MemState Bus<Protocol>::write(const Access& m,uint64_t tstamp,Trace& mt, MOESI_state ms)
{
	ERROR_IF(!next.get(),"next can not be null in a Cache");
	LOG(name,"write",m.phys);
	write_++;
	perform_up_to(tstamp);
	//just annotate on the timewheel
	//uint64_t bus_latency = (infinite_bandwidth) ? 0 : latency + (contention.size() * latency) / bandwidth;
	if (infinite_bandwidth)
		ERROR_IF(contention.size()!=0,"if infinite_badwidth==true => contention.size()==0");
	uint64_t bus_latency = latency + (contention.size() * latency) / bandwidth;
	wait_time_write_+=bus_latency;
	const MemState& nstate = protocol.write(this,next,prevs,m,tstamp,mt,ms);
	uint32_t protocol_latency = nstate.latency();
	MOESI_state new_moesi = nstate.moesi();
	if (!infinite_bandwidth)
		contention.insert(tstamp+bus_latency+protocol_latency);
	LOG(name,"write latency",bus_latency+protocol_latency);
	return MemState(bus_latency+protocol_latency,new_moesi);
}


template <typename Protocol>
void Bus<Protocol>::perform_up_to(uint64_t tstamp)
{
	LOG(name,"perform_up_to",tstamp);
	std::set<uint64_t>::iterator start,end;
	start = contention.begin(); 
	end = contention.upper_bound(tstamp);

	bw_+=contention.size();
	update_++;

	contention.erase(contention.begin(),contention.upper_bound(tstamp));
}

}
#endif
