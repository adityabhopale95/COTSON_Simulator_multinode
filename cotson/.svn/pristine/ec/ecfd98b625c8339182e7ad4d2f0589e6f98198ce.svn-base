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
#include "liboptions.h"
#include "cpu_timer.h"

using namespace std;

class TraceStats : public CpuTimer
{
public: 
	TraceStats(Parameters&);
	~TraceStats();

	void simple_warming(const Instruction*) {}
	void full_warming(const Instruction*) {}
	void simulation(const Instruction*);
	void idle(uint64_t);

private:
 	ofstream disfile;
	bool advanced_stats;
	string disasm_file;
	uint64_t instructions;
	uint64_t cycles;
	uint64_t insn[InstType::_MAXCTRL];

	enum { LOAD=0, STORE=1 };
	uint64_t mem[2];
};

registerClass<CpuTimer,TraceStats> trace_stats_c("trace_stats");

TraceStats::TraceStats(Parameters& p) : CpuTimer(&cycles,&instructions),
	advanced_stats(false),
	disasm_file(p.get<string>("trace_file"))
{
	add("instructions",instructions);
	add("instruction_default",insn[InstType::DEFAULT]);
	add("instruction_call",insn[InstType::CTRL_CALL]);
	add("instruction_jump",insn[InstType::CTRL_JUMP]);
	add("instruction_ret",insn[InstType::CTRL_RET]);
	add("instruction_iret",insn[InstType::CTRL_IRET]);
	add("instruction_branch",insn[InstType::CTRL_BRANCH]);
	add("cycles",cycles);
	add("memory_loads",mem[LOAD]);
	add("memory_stores",mem[STORE]);
	clear_metrics();

	if (!disasm_file.empty())
	{
		advanced_stats = true;
		disfile.open(disasm_file.c_str());
	}
}

TraceStats::~TraceStats()	
{
	disfile.close();
}

void TraceStats::simulation(const Instruction* inst)
{
	instructions++;
	cycles++;
	insn[inst->Type()]++;
	mem[LOAD]+=inst->Loads();	
	mem[STORE]+=inst->Stores();	
	
	if (advanced_stats)
		inst->disasm(disfile);
}

void TraceStats::idle(uint64_t c)
{
    cycles += c;
}

