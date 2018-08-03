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
#include "cpu_timer.h"
#include "liboptions.h"

using namespace std;

class SimPointProfile : public CpuTimer
{
public: 
	SimPointProfile(Parameters&);
	~SimPointProfile();

	void simple_warming(const Instruction*) {}
	void full_warming(const Instruction*) {}
	void simulation(const Instruction*);
	void idle(uint64_t);

private:
	uint64_t instructions;
	uint64_t cycles;
	uint64_t interval_instructions;
	uint64_t basic_blocks;
	
	void dump_bbInfo ();
	void newBB(uint64_t pc, uint64_t numInst);

	std::map<uint64_t, std::pair<uint32_t, uint64_t> > bbInfo;

	uint32_t bbCounter;
	uint64_t instCounter;
	uint64_t intervalSize;
	 
	DumpGzip gz;
};


registerClass<CpuTimer,SimPointProfile> simpoint_profile_c("simpoint_profile");

SimPointProfile::SimPointProfile(Parameters&p) : CpuTimer(&cycles,&instructions),
	instructions(0),
	cycles(0),
	interval_instructions(0),
	basic_blocks(0),
	bbCounter(0), instCounter(0),
	intervalSize(p.get<uint32_t>("interval_size","1M")),
	gz(p.get<string>("logfile","/tmp/simpoint_profile.log.gz"))
{
	add("instructions",instructions);
	add("cycles",cycles);
	add("basic_blocks",basic_blocks);
	add_ratio("insts_x_bb","instructions","basic_blocks");
	clear_metrics();
}

SimPointProfile::~SimPointProfile() { } 

void SimPointProfile::simulation(const Instruction* inst)
{
	instructions++;
	cycles++;
	interval_instructions++;
	
	// End of BB?
	if (inst->is_control())
	{
		newBB(inst->PC().phys, interval_instructions);
		basic_blocks++;
		interval_instructions = 0;         
	} 
}

void SimPointProfile::idle(uint64_t c)
{
    cycles += c;
}

void SimPointProfile::newBB(uint64_t pc, uint64_t numInst)
{
	if (bbInfo.find(pc) == bbInfo.end())
		bbInfo[pc] = make_pair(bbCounter++, numInst); 
	else	
		bbInfo[pc].second += numInst;
	
	instCounter += numInst;

	// End of interval?
	if (instCounter > intervalSize) 
	{
		instCounter -= intervalSize;
		dump_bbInfo();
	}
}

void SimPointProfile::dump_bbInfo ()
{
	map<uint64_t, pair<uint32_t, uint64_t> >::iterator it;
	vector<uint32_t> ordered(bbCounter);
  
	// Order map by basic_block_ID
	for (it=bbInfo.begin(); it!=bbInfo.end(); it++)
	{
		ordered[it->second.first] = it->second.second;
		it->second.second = 0;
	}
	
	stringstream out;
	out << 'T';

	for(uint32_t i=0; i<bbCounter; i++) 
	{ 
		if(ordered[i] > 0) 
			out << ':' << i+1 << ':' << ordered[i] << "   ";
	}

	out << '\n';
	gz.as_text(out.str());
}
