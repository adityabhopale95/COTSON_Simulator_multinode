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
//#define _DEBUG_THIS_
#include "abaeterno_config.h"
#include "logger.h"
#include "profiler.h"
#include "cotson.h"
#include "machine.h"

using namespace std;
using namespace boost;
using namespace Profile;

Profiler::Profiler(): 
	totinstcount(0),
	totpathcount(0),
    active(false),
	use_cr3(false),
	simulating(false)
{
}

Profiler::~Profiler() {}

void Profiler::turn_on()   
{ 
    active=true; 
    Cotson::Cpu::force_flush(); 
}

void Profiler::turn_off()  
{ 
    active=false; 
	Cotson::Cpu::force_flush(); 
}

void Profiler::clear()
{
	totinstcount=0;
	totpathcount=0;
	for (CpuData::iterator i=cpu_data.begin();i!=cpu_data.end();++i)
	    i->second.clear();
}

double Profiler::endFunctional()
{
	if (!active)
	    return 0.0;
     return endFunctional_bb();
}

double Profiler::endFunctional_bb()
{
	uint64_t all = 0;
	uint64_t sim = 0;
	double mismatch = 0.0;
    for (CpuData::const_iterator i=cpu_data.begin();i!=cpu_data.end();++i)
	{
		const CpuInfo& cpu = i->second;
		uint64_t sim_tot = cpu.sim_bb_tot;
		uint64_t func_tot = cpu.func_bb_tot;
        for (PcMap::const_iterator fi=cpu.func_bbs.begin();fi!=cpu.func_bbs.end();++fi)
	    {
	        const PC& pc = fi->first;
		    uint64_t insts = fi->second;
			double func_ratio = static_cast<double>(insts)/func_tot;
		    all += insts;
            PcMap::const_iterator si = cpu.sim_bbs.find(pc);
		    if (si != cpu.sim_bbs.end())
			{
				sim += insts;
			    double sim_ratio = static_cast<double>(si->second)/sim_tot;
				mismatch += func_ratio * fabs(func_ratio-sim_ratio);
		    }
			else
			    mismatch += func_ratio;
	    }
	}
	if (all)
	{
	    double r = static_cast<double>(sim)/all;
        cout << "END FUNCTIONAL " << 1.0 - mismatch << " " << sim << " " << all << " " << r << endl;
	    return 1.0 - mismatch;
	}
	return 0;
}

void Profiler::beginFunctional()
{
    if (!active) 
        return;
    LOG("BEGIN FUNCTIONAL");
    for (CpuData::iterator i=cpu_data.begin();i!=cpu_data.end();++i)
    {
        // save instructions and CPI
        CpuInfo& info = i->second;
        info.instcount0 = info.instcount; 
        info.cpi = Machine::get().cpu(i->first)->cpi();
        info.cycles = info.insts = 0;
		info.func_bbs.clear();
		info.func_bb_tot = 0;
    }
    simulating=false;
}

void Profiler::beginSimulation()
{
    if (!active) 
        return;
    for (CpuData::iterator i=cpu_data.begin();i!=cpu_data.end();++i)
    {
        // save instructions and CPI
        CpuInfo& info = i->second;
        info.instcount0 = info.instcount; 
        info.cycles = info.insts = 0;
		info.sim_bbs.clear();
		info.sim_bb_tot = 0;
    }
    LOG("BEGIN SIMULATION");
    simulating=true;
}

double Profiler::endSimulation()
{
    return endSimulation_bb();
}

double Profiler::endSimulation_bb()
{
   return 1.0;
}

void Profiler::profile_bb(uint64_t devid, const PC& cur_pc)
{
	CpuInfo& info = cpu_data[devid];
	uint64_t len = cur_pc.len;
	if (simulating)
	{
	   info.sim_bbs[cur_pc] += len;
	   info.sim_bb_tot += len;
	}
    else
	{
	   info.func_bbs[cur_pc] += len;
	   info.func_bb_tot += len;
    }
}

bool Profiler::execute(uint64_t nanos,uint32_t tag,uint64_t devid)
{
    if (!active) 
        return false;
    CpuInfo& info = cpu_data[devid];
    Tags::const_iterator ti = info.tags.find(tag);
    if (ti == info.tags.end())
        return false;

    const TagInfo& taginfo = ti->second;
    if (use_cr3 && taginfo.new_cr3) 
    { // invalidate cr3
        info.cr3 = 0;
        return true;
    }
    uint64_t len = taginfo.len;
    info.instcount += len;
    totinstcount += len;
    if (use_cr3 && !info.cr3)
        info.cr3=Cotson::X86::RegisterCR3();
    profile_bb(devid,PC(info.cr3,taginfo.pc,len));
    return true;
}

ostream& operator<<(ostream& o,const Profile::PC& p)
{
    o << "(";
    if (Profiler::get().cr3())
        o << (p.cr3 >> 12) << ",";
    o << "0x" << hex << p.pc << dec;
    o << ")+" << p.len;
    return o;
}

void Profiler::dump()
{
    dump_bb();
}

void Profiler::dump_bb()
{
    cout << "Profile: instcount " << totinstcount << " (";
    for(CpuData::iterator i=cpu_data.begin();i!=cpu_data.end();++i)
        cout << "(p" << i->first << " " << i->second.instcount << ")";
    cout << ")" << endl;
}

