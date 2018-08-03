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
// #define _DEBUG_THIS_
#include "abaeterno_config.h"
#include "logger.h"
#include "cpu_timer.h"
#include "twolev.h"
#include "parameters.h"

using namespace std;
using namespace boost;

class TimerRandom : public CpuTimer
{
public: 
	TimerRandom(Parameters&);
	void simulation(const Instruction*);
	void simple_warming(const Instruction*) {}
	void full_warming(const Instruction*) {}
	void beginSimulation();
	void endSimulation();
    void idle(uint64_t);

private: 
	uint64_t instructions;
	uint64_t cycles;
	double dcycles;

	boost::mt19937 rng;
	uniform_real<> ipc_range;
	variate_generator< boost::mt19937&,uniform_real<> > ipc_gen;
};

registerClass<CpuTimer,TimerRandom> timer_random_c("random");

TimerRandom::TimerRandom(Parameters& p) : CpuTimer(&cycles,&instructions),
	instructions(0),
	cycles(0),
	dcycles(0.0),
	rng(p.has("seed")?p.get<uint>("seed"):time(0)),
	ipc_range(p.get<double>("min_ipc","0.5"),p.get<double>("max_ipc","2.0")),
	ipc_gen(rng,ipc_range)
{
	add("instructions",instructions);
	add("cycles",cycles);
	trace_needs.history=0;
	clear_metrics();
}

void TimerRandom::beginSimulation() 
{ 
	clear_metrics();
	dcycles = 0.0;
}

void TimerRandom::simulation(const Instruction*)
{
    instructions++;
	dcycles += static_cast<uint64_t>(1.0/ipc_gen());
	cycles = static_cast<uint64_t>(dcycles);
}

void TimerRandom::idle(uint64_t c)
{
	dcycles += static_cast<double>(c);
	cycles = static_cast<uint64_t>(dcycles);
}

void TimerRandom::endSimulation()
{
	LOG("RANDOM: instructions",instructions,"cycles",cycles);
}

