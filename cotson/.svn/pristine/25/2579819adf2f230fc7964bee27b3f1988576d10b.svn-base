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
#ifndef TIMEWHEEL_H
#define TIMEWHEEL_H

#include "liboptions.h"
#include "memory_interface.h"
#include "memory_trace.h"
#include "cache_line.h"
#include "inline.h"
#include "hashmap.h"
#include <boost/pool/pool_alloc.hpp>

namespace Memory {

class Future
{
public:
    enum Type { UPDATE=0, INVALIDATE=1 }; 
	static const size_t NTYPES=INVALIDATE+1;

    uint64_t when;
    uint64_t addr;
    MOESI_state moesi;
    Trace mt;
	Line *line;
	Type type;
    uint64_t tag;
	int mark;

    INLINE Future():
	    when(0),addr(0),moesi(NOT_FOUND),mt(Trace(0)),line(0),type(UPDATE),tag(0),mark(0)
		{}
	INLINE Future(uint64_t c,uint64_t t,uint64_t a,MOESI_state m,const Trace& tx,Future::Type tp,Line *l):
        when(c),addr(a),moesi(m),mt(tx),line(l),type(tp),tag(t),mark(0)
		{}

    ~Future() {}
};

class FuturePool
{
public:
    FuturePool(uint32_t sz):buf(new Future[sz]),end(&buf[sz]),cur(buf),mark(1) {}
	~FuturePool() { delete[] buf; }
	INLINE const Future* malloc(const Future& f) 
	{
	    Future *fp = new(findfree()) Future(f); 
		fp->mark=mark; 
		return fp;
	}
	INLINE void free(const Future* fp) { const_cast<Future*>(fp)->mark=0; }
	INLINE bool isfree(const Future* fp) const { return fp->mark != mark; }
	INLINE void reset() { cur=buf; mark++; }

private:
	INLINE Future *next() 
	{
		Future *fp=cur;
	    if (++cur==end)
		    cur=buf; 
		return fp;
	}
	INLINE Future *findfree() 
	{
		Future* fp0=next();
		if (fp0->mark != mark)
		    return fp0;
		for (Future *fp=next(); fp!=fp0; fp=next())
		    if (fp->mark != mark)
			    return fp;
		throw std::runtime_error("Event buffer too small: increase 'max_events'");
	    return 0;
	}
    Future * const buf;
    const Future * const end;
	Future * cur;
	int mark;
};

namespace EventPool 
{
    typedef boost::shared_ptr< boost::pool<> > PoolPtr;
    template <typename T> class Allocator : public std::allocator<T>
    {
    public:
        INLINE Allocator(PoolPtr p):pool(p) {}
        INLINE ~Allocator() {}
        INLINE T* allocate(size_t n, const void* =0) 
	    {
			if (n>1)
	            return static_cast<T*>(pool->ordered_malloc(n?n:1));
	        return static_cast<T*>(pool->malloc());
	    }
        INLINE void deallocate(void* p, size_t) { }
	    template <class U> struct rebind { typedef Allocator<U> other; };
    private:
        PoolPtr pool;
    };
}

class TimeWheel
{
private:
	typedef const Future* FuturePtr;


	struct fless
	{
	    INLINE bool operator()(FuturePtr f2,FuturePtr f1) const
	    {
			return f1 && f2 && f1->when < f2->when;
	    }
	};

	typedef std::vector< FuturePtr,EventPool::Allocator<FuturePtr> > EventContainer;
	typedef std::priority_queue<FuturePtr,EventContainer,fless> Events;
	typedef HashMap<int64_t,Events> Tags;

	FuturePool fpool;
	EventPool::PoolPtr epool;

    Events events;
	Tags tags[Future::NTYPES];

	static const size_t MAXPENDING=1000;
	FuturePtr pending[MAXPENDING];
	size_t npending;

public:
    TimeWheel(uint64_t m=1000) :
	    fpool(m),
		epool(new boost::pool<>(sizeof(FuturePtr))),
		events(noevents()),
		npending(0) 
	{}

	~TimeWheel() {}

    INLINE void insert(const Future& f)
	{
		// don't actually record anything in the tag map, but 
		// lazily delay until someone needs it. If time advances 
		// quickly, no event will ever be recorded
		FuturePtr fp=fpool.malloc(f);
		events.push(fp);
	    pending[npending++]=fp;
		if (npending==MAXPENDING) // bailout, shouldn't happen
		    flush_pending();
	}

    INLINE const Future* has(uint64_t tag, Future::Type tp, uint64_t c=0)
    {
		if (events.empty()) {
		    npending=0;
		    return 0;
		}
		update_pending(c,tp);
		
		Tags& ttags = tags[tp];
		Tags::iterator ti = ttags.find(tag);
		if (ti != ttags.end()) {
		    // lazy cleanup of old events
		    Events& tevents=ti->second;
			while (    (!tevents.empty() && fpool.isfree(tevents.top()))
			        || (!tevents.empty() && tevents.top()->tag != tag))
			    tevents.pop();
			if (!tevents.empty())
		        return tevents.top();
			ttags.erase(ti);
		}
		return 0;
    }

    INLINE size_t size() const { return events.size(); }
	INLINE bool empty() const { return events.empty(); }
	INLINE bool empty(uint64_t c) const { return events.empty() || events.top()->when > c; }
	INLINE const Future* top() const { return events.top(); }
	INLINE void pop() { fpool.free(events.top()); events.pop(); }

	void reset()
	{
		epool.reset(new boost::pool<>(sizeof(FuturePtr)));
		for(size_t i=0; i<static_cast<uint>(Future::NTYPES); ++i)
			tags[i].clear();
		events=noevents();
	    npending=0;
		fpool.reset();
	}

private:
	INLINE Events noevents()
	{
	    return Events(fless(),EventContainer(EventPool::Allocator<FuturePtr>(epool)));
	}

	INLINE void save_tag(FuturePtr fp)
	{
	    uint64_t tag = fp->tag;
	    Tags& ttags = tags[fp->type];
	    Tags::iterator ti = ttags.find(tag);
	    if (ti == ttags.end())
	        ti = ttags.insert(ti,std::make_pair(tag,noevents()));
	    ti->second.push(fp);
	}

    INLINE void update_pending(uint64_t c, Future::Type tp)
	{
		// lazy update of pending events for a given type
		int np=0;
		for (size_t i=0; i<npending; ++i) {
			FuturePtr fp = pending[i];
			if (fp && fp->when >= c && fp->type==tp) {
				save_tag(fp);
				pending[i]=0;
		    }
			else if (fp)
			    np++;
		}
		if (np==0)
		    npending=0;
	}
    INLINE void flush_pending()
	{
		for (size_t i=0; i<npending; ++i) {
			FuturePtr fp = pending[i];
			if (fp)
				save_tag(fp);
		}
		npending=0;
	}
};

}
#endif
