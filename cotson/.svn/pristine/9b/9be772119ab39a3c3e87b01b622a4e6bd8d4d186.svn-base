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
#ifndef CACHE_IMPL_H
#define CACHE_IMPL_H

#include "cache_line.h"
#include "timewheel.h"
#include "liboptions.h"
#include "machine.h"
#include "logger.h"
#include "error.h"

namespace Memory 
{

template <typename Storage, typename Timer>
class CacheImpl : public Interface
{
public:
	CacheImpl(const Parameters&);
	INLINE MemState read(const Access&,uint64_t,Trace&, MOESI_state);
	INLINE MemState readx(const Access&,uint64_t,Trace&, MOESI_state);
	INLINE MemState write(const Access&,uint64_t,Trace&,MOESI_state);
	INLINE MOESI_state state(const Access&, uint64_t);
	INLINE void invalidate(const Access&,uint64_t,const Trace&,MOESI_state);

	void beginSimulation();
	void endSimulation();

	// Full warming simulates, hence we need the timewheel
	void beginFullWarming() { LOG(name,"BEGIN FULL WARMING"); beginSimulation(); }
	void endFullWarming() { LOG(name,"END FULL WARMING"); endSimulation(); }

	INLINE uint32_t item_size() const { return item_size_; }

	void dump();

private:
	enum {LRU_INIT=1024};
	uint64_t lru_low,lru_counter;  // for LRU handling
	uint64_t read_;                
	uint64_t write_;                
	uint64_t read_miss_;
	uint64_t read_half_miss_;
	uint64_t write_miss_;
	uint64_t write_half_miss_;
	uint64_t data_invalidation_;
	uint64_t inclusion_invalidation_;
	uint64_t snoop_;
	uint64_t lookup_[MOESI_STATES_];	
	uint64_t update_[MOESI_STATES_];
	uint64_t writeback_;

	const uint32_t item_size_;

	Storage storage;
	Timer timer;

	const typename Storage::CacheType& cache;
		
	INLINE void inclusive_invalidations(uint64_t,uint64_t,Trace&);
	INLINE void perform_update(const Future&);
	INLINE void perform_invalidate(const Future&);
	INLINE void perform_up_to(uint64_t,uint32_t,uint32_t);
    INLINE MemState writeHit(const Access&,uint64_t,Trace&,MOESI_state);
    INLINE MemState writeMiss(const Access&,uint64_t,Trace&,MOESI_state);
	
	const uint32_t max_updates; //maximum number of parallel updates
	const uint32_t max_invalidates; //maximum number of parallel invalidations
	uint32_t max_latency; //maximum latency

	TimeWheel events;     // future events
	const bool writethrough;  // writethrough policy (writeback if false)
	const bool writeallocate;  // writeallocate policy (for writethrough)
	bool simulating; // simulation or warming
}; 

template <typename Storage, typename Timer>
CacheImpl<Storage,Timer>::CacheImpl(const Parameters& p):
	lru_low(LRU_INIT),lru_counter(LRU_INIT),
	item_size_(p.get<uint32_t>("line_size")),
	storage(p),
	timer(p),
	cache(storage.cache),
	max_updates(p.get<int32_t>("max_updates","-1",false)), // no track
	max_invalidates(p.get<int32_t>("max_invalidations","-1",false)), // no track
	max_latency(0), // no track
	events(p.get<uint32_t>("max_events","20000",false)), // no track
	writethrough(p.get<std::string>("write_policy")=="WT"),
	writeallocate(p.get<bool>("write_allocate",writethrough?"false":"true")),
	simulating(true)
{
	name=p.get<std::string>("name");

	add("read",read_);
	add("write",write_);
	add("read_miss",read_miss_);
	add("read_half_miss",read_half_miss_);
	add("write_miss",write_miss_);
	add("write_half_miss",write_half_miss_);
	add_ratio("read_miss_rate","read_miss","read");
	add_ratio("write_miss_rate","write_miss","write");
	add("data_invalidation",data_invalidation_);
	add("snoop",snoop_);
	add("inclusion_invalidation",inclusion_invalidation_);
	add("lookup_NOT_FOUND", lookup_[NOT_FOUND]); 
	add("lookup_MODIFIED",  lookup_[MODIFIED]); 
	add("lookup_OWNER",     lookup_[OWNER]); 
	add("lookup_EXCLUSIVE", lookup_[EXCLUSIVE]); 
	add("lookup_SHARED",    lookup_[SHARED]); 
	add("lookup_INVALID",   lookup_[INVALID]);
	add("update_NOT_FOUND", update_[NOT_FOUND]);     
	add("update_MODIFIED",  update_[MODIFIED]);  
	add("update_OWNER",     update_[OWNER]); 
	add("update_EXCLUSIVE", update_[EXCLUSIVE]); 
	add("update_SHARED",    update_[SHARED]);     
	add("update_INVALID",   update_[INVALID]);    
	add("timer",timer);
	add("writeback",writeback_); 

	clear_metrics();
}

template <typename Storage, typename Timer>
INLINE void 
CacheImpl<Storage,Timer>::inclusive_invalidations(
    uint64_t evict_tag,
	uint64_t tstamp,
	Trace& mt
) {
	if (prevs.size()==0) // L1 caches
	   return; 
	uint64_t evict_addr = cache.addr_from_group_id(evict_tag);
	Access evict(evict_addr,evict_addr,item_size_);
	const Interface* who=mt.getMem();
	for(std::vector<Interface*>::iterator i=prevs.begin();i!=prevs.end();++i)
	{
	    Interface *mi = *i;
		if (mi != who) //NOT_FOUND is our way of saying inclusive_invalidations
		    mi->invalidate(evict,tstamp,mt,NOT_FOUND);
    }
}

template <typename Storage, typename Timer>
INLINE MemState 
CacheImpl<Storage,Timer>::read(
    const Access& m,
	uint64_t tstamp,
	Trace& mt, 
	MOESI_state ms
) {
	ERROR_IF(!next.get(),"next can not be null in a Cache");
	ERROR_IF(mt.getMem() && mt.getType()!=READ,"read called with wrong trace");
	perform_up_to(tstamp,max_updates,max_invalidates);

	LOG(name,"read",m,mt);
	Line* cl = storage.find(m.phys);

	read_++;
	uint32_t latency = timer.latency(tstamp,mt,m);
	uint64_t tag = cache.group_id(m.phys);
	
	if(cl && cl->moesi.isValid())
	{
	    LOG(name,"hit",m,mt);
		cl->lru = ++lru_counter;
		lookup_[cl->moesi]++;
		LOG(name,"read hit - state:",cl->moesi);
		return MemState(latency,cl->moesi);
	}
	
	// check to see if miss is coming
	const Future* pending=simulating ? events.has(tag,Future::UPDATE,tstamp) : 0;
	if(pending && pending->when <= tstamp+max_latency)
	{
		read_half_miss_++;
		MOESI_state new_ms = pending->moesi;
		uint64_t pending_latency = pending->when - tstamp;
		uint32_t new_lat = latency > pending_latency ? latency : pending_latency;
		lookup_[new_ms]++;
		LOG(name,"read latency",new_lat,"half miss - state",new_ms,"max_lat",max_latency);
		return MemState(new_lat,new_ms);
    }

	// miss
	LOG(name,"miss",m,mt);
	MOESI_state upstate = cl ? INVALID : NOT_FOUND;
	read_miss_++;
	lookup_[upstate]++;
	update_[upstate]++;

	// Issue a read to fetch the line from the next level
	mt.add(this,READ);
	const MemState& nstate = next->read(m,tstamp,mt,upstate);
	uint32_t new_latency = nstate.latency();
	MOESI_state new_moesi = nstate.moesi();

	// Find line to evict, invalidate and schedule update for later
	// (if an INVALID line was found for the tag, force to use that one)
	Line* evict = cl ? cl : storage.find_lru(m.phys);
	ERROR_IF(!evict,"null evict line in read");
	evict->lru = ~0ULL; // mark line with max lru to reserve it

	inclusive_invalidations(evict->tag,tstamp+new_latency,mt);

	uint32_t rd_lat = latency + new_latency;
	max_latency = rd_lat > max_latency ? rd_lat : max_latency;

    Future f(tstamp+rd_lat,tag,m.phys,new_moesi,mt,Future::UPDATE,evict);
	simulating ? events.insert(f) : perform_update(f);
		
    LOG(name,"read latency",rd_lat,"miss - state",new_moesi);
	return MemState(rd_lat,new_moesi);
}

template <typename Storage, typename Timer>
INLINE MemState 
CacheImpl<Storage,Timer>::readx(
    const Access& m,
	uint64_t tstamp,
	Trace& mt, 
	MOESI_state ms
) {
	ERROR_IF(!next.get(),"next can not be null in a Cache");
	ERROR_IF(!mt.getMem(),"readx can't come from cpu");
	ERROR_IF(mt.getType()!=READX,"readx called with wrong trace");
	// if (ms!=INVALID)
	LOG(name,"readx",m,mt);
	if (ms.isValid())
	{
		// this happens on write hit, so return MODIFIED, meaning dirty
		LOG(name,"read latency",0,"readx - state:",MODIFIED);
		return MemState(0,MODIFIED); 
	}
	LOG(name,"READX -> READ");
	mt.add(this,READ);
	return read(m,tstamp,mt,ms);
}

template <typename Storage, typename Timer>
INLINE MemState
CacheImpl<Storage,Timer>::write(
    const Access& m,
	uint64_t tstamp,
	Trace& mt,MOESI_state ms
) {
	ERROR_IF(!next.get(),"next can not be null in a Cache");
	ERROR_IF(mt.getMem() && mt.getType()!=WRITE,"write called with wrong trace");
	LOG(name,"write",m,mt);
    perform_up_to(tstamp,max_updates,max_invalidates);

	Line* cl = storage.find(m.phys);
	uint32_t latency = timer.latency(tstamp,mt,m);
	write_++;

	uint64_t tag = cache.group_id(m.phys);
	if(cl && cl->moesi.isValid())
	{
	    LOG(name,"write hit",m,mt);
		MOESI_state ms = cl->moesi;
		cl->lru = ++lru_counter;
		const MemState& nstate = writeHit(m,tstamp,mt,ms);
		MOESI_state new_ms = nstate.moesi();
		uint32_t tot_latency = latency + nstate.latency();
		cl->moesi = new_ms;
		LOG(name,tstamp,"write hit, state before",ms,"after",new_ms,"lat",tot_latency);
		update_[ms]++;
		return MemState(tot_latency,new_ms); 
	}
	
	// check to see if miss is coming
	const Future* pending=simulating ? events.has(tag,Future::UPDATE,tstamp) : 0;
	if(pending && pending->when <= tstamp+max_latency)
	{
	    write_half_miss_++;
		uint64_t when=pending->when;
	    MOESI_state ms = pending->moesi;
		const MemState& nstate = writeHit(m,when,mt,ms);
	    MOESI_state new_ms = nstate.moesi();
		uint32_t tot_latency = latency + when - tstamp; 
		// Note: we don't add nstate.latency() because we assume this happens in background
		const_cast<Future*>(pending)->moesi=new_ms; // FIXME: changing data off a const ptr!!
		LOG(name,tstamp,"write half miss, state before",ms,"after",new_ms,
		    "lat",tot_latency,"max_lat",max_latency);
		update_[ms]++;
		return MemState(tot_latency,new_ms);
    }

	// miss
	LOG(name,"miss",m,mt);
	write_miss_++;
	MOESI_state upstate = cl ? INVALID : NOT_FOUND;
	update_[upstate]++;
	const MemState& nstate = writeMiss(m,tstamp,mt,upstate);
	uint32_t new_latency = nstate.latency();

	MOESI_state new_ms = nstate.moesi();
	uint32_t wr_lat = latency + new_latency;
	max_latency = wr_lat > max_latency ? wr_lat : max_latency;

	if (writeallocate)
	{
	    // Find line to evict, invalidate and schedule line update for later
	    // (if an INVALID line was found for the tag, force to use that one)
	    Line* evict = cl ? cl : storage.find_lru(m.phys);
	    ERROR_IF(!evict,"null evict line in write");
		LOG("schedule write allocate",std::hex,tag,"evict",evict->tag,std::dec);
	    evict->lru = ~0ULL; // mark line with max lru to reserve it

		// Schedule invalidations
	    inclusive_invalidations(evict->tag,tstamp+new_latency,mt);

        Future f(tstamp+wr_lat,tag,m.phys,new_ms,mt,Future::UPDATE,evict);
	    simulating ? events.insert(f) : perform_update(f);
	}

	LOG(name,"write latency",wr_lat,"miss - state",new_ms);
	return MemState(wr_lat,new_ms);
}

template <typename Storage, typename Timer>
INLINE MOESI_state 
CacheImpl<Storage,Timer>::state(
	const Access& m, 
	uint64_t tstamp
) {
	snoop_++;
	perform_up_to(tstamp,max_updates,max_invalidates);
	Line* cl = storage.find(m.phys);
	return cl ? cl->moesi : MOESI_state(NOT_FOUND);
}

template <typename Storage, typename Timer>
INLINE void 
CacheImpl<Storage,Timer>::invalidate(
	const Access& m, 
	uint64_t tstamp,
	const Trace& mt,
	MOESI_state ms
) {
	Line* cl = storage.find(m.phys);
	if (cl)
	{
		if(ms==INVALID)
			data_invalidation_++;
		else if(ms==NOT_FOUND)
		{
			inclusion_invalidation_++;
			ms=INVALID;
		}

        Future f(tstamp,cache.group_id(m.phys),m.phys,ms,Trace(0),Future::INVALIDATE,cl);
		simulating ? events.insert(f) : perform_invalidate(f);

		for(std::vector<Interface*>::iterator i = prevs.begin(); i != prevs.end(); ++i)
			(*i)->invalidate(m,tstamp,mt,ms);
	}
}

template <typename Storage, typename Timer>
void CacheImpl<Storage,Timer>::beginSimulation() 
{
	LOG(name,"BEGIN SIMULATION");
	perform_up_to(~0ULL,-1,-1); // commit pending changes
	events.reset();
    timer.reset();
	simulating=true;
	max_latency = 0;
}

template <typename Storage, typename Timer>
void CacheImpl<Storage,Timer>::endSimulation() 
{
	LOG(name,"END SIMULATION");
	perform_up_to(~0ULL,-1,-1); // commit pending changes
	events.reset();
	simulating=false;
}

template <typename Storage, typename Timer>
INLINE void CacheImpl<Storage,Timer>::perform_up_to(
    uint64_t tstamp, 
	uint32_t max_upd,
	uint32_t max_inv
) {
	if (!simulating || events.empty(tstamp))
	    return;
	LOG(name,"perform up to", tstamp);
	uint32_t n_updates = 0, n_invalidates = 0;
	while (!events.empty(tstamp)) 
	{
		const Future* f = events.top(); events.pop();
		bool reschedule = true;
		switch(f->type) 
		{
			case Future::UPDATE:
		        if (n_updates < max_upd) 
				{
	                ++n_updates;
		            perform_update(*f);
					reschedule=false;
				}
				break;

		    case Future::INVALIDATE:
	            if (n_invalidates < max_inv) 
				{
	                ++n_invalidates;
			        perform_invalidate(*f);
					reschedule=false;
				}
				break;
	    }
		if (reschedule)
		{
			Future f1(*f);
			f1.when=tstamp+1;
	        events.insert(f1);
	    }
	}
}

template <typename Storage, typename Timer>
INLINE void CacheImpl<Storage,Timer>::perform_update(const Future& f)
{
	Line* replace = f.line;
	ERROR_IF(!replace,"null replace line");

	// Writeback if needed
	if (!writethrough && replace->moesi.isDirty())
	{
		LOG(name,"writeback tag",replace->tag);
		writeback_++;
	    uint64_t raddr = cache.addr_from_group_id(replace->tag);
		Access wba(raddr,raddr,item_size_); // writeback address
		Trace wbmt(f.mt.getCpu()); // writeback memtrace
		next->write(wba,f.when,wbmt.add(this,WRITE),replace->moesi);
		// Note: we ignore the writeback timing (in the background)
	}

	// Allocate a new line
	storage.allocate(replace,f.addr,++lru_counter,f.moesi);
	LOG(name,"allocate", std::hex,f.addr,std::dec, "state", f.moesi, "cycle", f.when);
}

template <typename Storage, typename Timer>
INLINE void CacheImpl<Storage,Timer>::perform_invalidate(const Future& f)
{
	Line* cl = f.line;
	if (cl)
	{
		LOG(name,"invalidate tag",f.tag, "update state from", 
		    cl->moesi, "to", f.moesi, "cycle" , f.when);
		// Writeback if needed
		if (!writethrough && cl->moesi.isDirty() && f.moesi==SHARED)
	    {
		    LOG(name,"writeback tag",cl->tag);
		    writeback_++;
	        uint64_t raddr = cache.addr_from_group_id(cl->tag);
		    Access wba(raddr,raddr,item_size_); // writeback address
		    Trace wbmt(f.mt.getCpu()); // writeback memtrace
		    next->write(wba,f.when,wbmt.add(this,WRITE),cl->moesi);
		    // Note: we ignore the writeback timing (in the background)
	    }
		cl->moesi = f.moesi;
		if(f.moesi==INVALID)
		{
			cl->lru=lru_low--; 
			if (lru_low==0) 
			    lru_low=LRU_INIT;
	    }
	}
}

template <typename Storage, typename Timer>
INLINE MemState
CacheImpl<Storage,Timer>::writeHit(const Access& m,uint64_t t,Trace& mt,MOESI_state ms)
{
	if (writethrough) 
	{
        mt.add(this,WRITE);
        return next->write(m,t,mt,ms);
	}
	// writeback
	if (ms.isExclusive())
        return MemState(0,MODIFIED); // Note: this_cache->latency is added by the caller
	
	ERROR_IF(!writeallocate,"Writeback requires write-allocate!");
    mt.add(this,READX); // readx handled by the next level only if it is a bus
    return next->readx(m,t,mt,ms);
}

template <typename Storage, typename Timer>
INLINE MemState
CacheImpl<Storage,Timer>::writeMiss(const Access& m,uint64_t t,Trace& mt,MOESI_state ms)
{
	if (writethrough)
	{
		MOESI_state new_ms = ms;
		if (writeallocate)
		{
            mt.add(this,READX);
            new_ms = next->readx(m,t,mt,ms).moesi();
		}
	    mt.add(this,WRITE);
	    return next->write(m,t,mt,new_ms);
	}
	// writeback
	ERROR_IF(!writeallocate,"Writeback requires write-allocate!");
    mt.add(this,READX);
	return next->readx(m,t,mt,ms);
}

template <typename Storage, typename Timer>
void CacheImpl<Storage,Timer>::dump()
{
	storage.dump();
}


}
#endif 
