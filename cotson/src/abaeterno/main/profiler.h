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

// $Id: profiler.h 11 2010-01-08 13:20:58Z paolofrb $
#ifndef PROFILER_H 
#define PROFILER_H

#include "circbuf.h"
#include "hashmap.h"

namespace Profile {

// per PC profile data
struct PC // cr3 (if needed) and pc,len
{
    uint64_t cr3, pc; 
    int len;
	PC():cr3(0),pc(0),len(0){}
	PC(uint64_t p):cr3(0),pc(p),len(0){}
	PC(uint64_t p, int l):cr3(0),pc(p),len(l){}
	PC(uint64_t c, uint64_t p, int l):cr3(c),pc(p),len(l){}
	inline bool operator<(const PC& o) const 
	{
	    return (cr3 < o.cr3) || (cr3==o.cr3 && pc < o.pc);
	}
	inline bool operator!=(const PC& o) const 
	{
	    return pc!=o.pc || cr3!=o.cr3;
	}
	inline bool operator==(const PC& o) const 
	{
	    return pc==o.pc && cr3==o.cr3;
	}
};

} // namespace Profile

// Hash specialization for PC
#ifdef __HAVE_TR1
namespace std { namespace tr1 {
    template<> struct hash<Profile::PC> {
        size_t operator()(const Profile::PC& x) const { return x.pc; }
    };
}}
#endif

class Profiler : public boost::noncopyable
{
public:
    typedef boost::function<void()> BreakFunc;

	Profiler();
	~Profiler();
	inline static Profiler& get()
	{
		static Profiler singleton;
		return singleton;
	}
	inline void cr3_on()    { use_cr3=true; }
	inline void cr3_off()   { use_cr3=false; }
	inline bool on() const  { return active; }
	inline bool cr3() const { return active && use_cr3; }
	inline void clear_tags(uint64_t devid){ cpu_data[devid].tags.clear(); }
    inline void tag_pc(uint64_t devid, uint32_t t,uint64_t pc,int len) 
    {
        TagInfo& info=cpu_data[devid].tags[t];
	    info.pc=pc;
	    info.len=len;
    }
    inline void tag_cr3(uint64_t devid, uint32_t t) 
    {
        cpu_data[devid].tags[t].new_cr3=true; 
    }
    bool execute(uint64_t,uint32_t,uint64_t);
	void dump();

	void clear();
	void turn_on();
	void turn_off();

	typedef std::vector<Profile::PC> PCvec;
	struct TagInfo // Map from tags to (pc, len, cr3_change)
	{
		uint64_t pc;
		bool new_cr3;
		int len;
		TagInfo():pc(0),new_cr3(false),len(0) {}
		TagInfo(uint64_t p,int l):pc(p),new_cr3(false),len(l) {}
	};
	typedef HashMap<uint32_t,TagInfo> Tags;
	typedef HashMap<Profile::PC,uint64_t> PcMap;

	struct CpuInfo
	{
	    double cpi, insts, cycles;
		uint64_t cr3;
		uint64_t instcount,instcount0;
	    Tags tags;
		PcMap sim_bbs;
		PcMap func_bbs;
		uint64_t sim_bb_tot;
		uint64_t func_bb_tot;
		CpuInfo(): cpi(0.0),insts(0.0),cycles(0.0),cr3(0),
		           instcount(0),instcount0(0),
				   tags() {}
		// don't clear tags and cr3
	    void clear() { instcount=instcount0=0; cpi=insts=cycles=0; }
	};
	typedef std::map<uint64_t,CpuInfo> CpuData;

	void beginSimulation();
	void beginFunctional();
	double endSimulation();
    double endFunctional();

private:
    void profile_bb(uint64_t,const Profile::PC&);
    double endFunctional_bb();
	double endSimulation_bb();
	void dump_bb();
	CpuData cpu_data;

	uint64_t totinstcount;
	uint64_t totpathcount;

	// Current history
	size_t maxpathlen;
	boost::scoped_ptr< CircBuf<Profile::PC> > cur_path;

	bool active;
	bool use_cr3;
	int avgcpi;
	size_t min_reuse;
	bool simulating;
};

std::ostream& operator<<(std::ostream&,const Profile::PC&);
#endif 
