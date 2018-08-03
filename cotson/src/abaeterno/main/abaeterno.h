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

// $Id: abaeterno.h 556 2013-07-25 12:48:43Z paolofrb $
#ifndef ABAETERNO_H 
#define ABAETERNO_H

#include "libmetric.h"
#include "sampler.h"
#include "profiler.h"

class AbAeterno : public boost::noncopyable, public metric
{
	AbAeterno(); 
	~AbAeterno();
public:
	inline static AbAeterno& get()
	{
		static AbAeterno singleton;
		return singleton;
	}

	void prepareToRun();

	void start();  
	void stop();
	void check_end();
	void end();
	void break_sample();

private:
	boost::shared_ptr<Sampler> sampler; //current sampler
	std::map<uint64_t,boost::shared_ptr<Sampler> > samplers;
	std::stack<uint64_t> sampler_stack;

	const std::string exitTrigger;
	const uint64_t maxNanos;
	const uint64_t maxSamples;
	const bool log_sim_time;
	const bool net_terminate;

	uint64_t code_tag;
	std::deque<uint32_t> translated_tags;
	std::map< uint32_t,std::map<uint32_t,uint32_t> > asm_tags;
	uint64_t samples;
	void do_compute_metrics();

public:
	void endQuantum();
	void translate(uint64_t);
	void execute(uint64_t,uint64_t,uint32_t);
	void network_cpuid(uint64_t,uint16_t,uint16_t);
	bool inject_tag(uint64_t,uint32_t,const uint8_t*);
	bool has_pending_tags() { return !translated_tags.empty(); }
	void clear_tags(uint32_t d) { asm_tags[d].clear(); }
	
private:
	void endSample();
	void startSample(); 
	void printStats();
	uint64_t next_sample;
	void callme(uint64_t);
    bool needs_sim_tag(const uint8_t*, uint32_t) const;
	
	uint64_t nanos;
	uint64_t cycles;
	uint64_t cyclesPerUsec;

	uint64_t sim_user_time;
	uint64_t sim_system_time;
	uint64_t sim_user_time_dead;
	uint64_t sim_system_time_dead;
	uint64_t sim_clock_ticks;

	SimState sim_state;
	uint64_t stateCounter[SIM_STATES];
	int translated_insts;
	bool tag_prefetch;
	bool print_stats;
	bool net_cpuid;
	bool custom_asm;
};
#endif 
