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
#include "error.h"
#include "sampler.h"
#include "liboptions.h"

using namespace boost;

class ExpRandom : public Sampler
{
public:
	ExpRandom(Parameters&);
	StateChange changeState(SimState);

private:
	boost::mt19937 rng;
    exponential_distribution<> funcDist,warmDist,simDist;
    variate_generator< mt19937&,exponential_distribution<> > eFunctional,eWarming,eSimulation;
};

using namespace std;

registerClass<Sampler,ExpRandom> exp_random_c("exp_random");

ExpRandom::ExpRandom(Parameters&p) : 
    Sampler(FULL_WARMING),
    rng(seed()),
	funcDist(1.0/static_cast<double>(p.get<uint64_t>("functional"))),
	warmDist(1.0/static_cast<double>(p.get<uint64_t>("warming"))),
	simDist(1.0/static_cast<double>(p.get<uint64_t>("simulation"))),
	eFunctional(rng,funcDist),
	eWarming(rng,warmDist),
	eSimulation(rng,simDist)
{
}

Sampler::StateChange ExpRandom::changeState(SimState curr)
{
	StateChange next;
	switch(curr)
	{
		case FUNCTIONAL:
			next = StateChange(FULL_WARMING, eWarming());
			break;

		case FULL_WARMING:
			next = StateChange(SIMULATION, eSimulation());
			break;
	
		case SIMULATION:
			next = StateChange(FUNCTIONAL, eFunctional());
			break;

		default: ERROR("SimState not handled");
	}
	return next;
}
