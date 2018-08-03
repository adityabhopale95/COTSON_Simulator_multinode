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

// $Id: cpu.h 725 2015-07-03 16:14:08Z paolofrb $
#ifndef CPU_H
#define CPU_H

#include "cpu_timer.h"
#include "lua_helpers.h"
#include "libmetric.h"
#include "cotson.h"
#include "states.h"

class TokenQueue;
class CpuTokenParser;
class Predictor;

class Cpu : public metric, public SimpleStateObserver
{
	public:
	Cpu(TokenQueue*,uint64_t,const std::string&);
	~Cpu();

	// lua hook
	void timer(luabind::object);
	void instruction_cache(Memory::lua&);
	void data_cache(Memory::lua&);
	void instruction_tlb(Memory::lua&);
	void data_tlb(Memory::lua&);

	void inject(InjectState);
	void tag(uint32_t,const Cotson::Inject::info_tag&);
	bool execute(uint32_t);

	double cpi() const;
	// void cpi(double);

	inline uint64_t id() const { return devid; }
	inline const std::string& name() const { return devname; }
	inline CpuTimer* cpu_timer() const { return timing.get(); }

	protected:

	const uint64_t devid;
	const std::string devname;

	void beginSimulation();
	void beginFunctional();
	void beginSimpleWarming();
	void beginFullWarming();

	private:

	std::map<std::string,difference> stats;
	
	void do_compute_metrics();
	void reset_sample();
	void set_sample();
	void setIPC();

	boost::scoped_ptr<CpuTokenParser> parser;
	boost::scoped_ptr<CpuTimer> timing;
	boost::scoped_ptr<Predictor> predictor;

	
	std::vector<uint64_t> hb;
	difference cycles;
	difference instructions;
	difference idlecount;

	difference sample_instructions;
	difference sample_cycles;
	difference sample_idle;

	double cpi_;

	bool init;
};

#endif
