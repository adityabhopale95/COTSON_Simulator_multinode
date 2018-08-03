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

// $Id: random.cpp 119 2010-09-17 16:05:48Z paolofrb $
#include "abaeterno_config.h"
#include "error.h"
#include "sampler.h"
#include "liboptions.h"

uint32_t Sampler::seed() const
{
    if (Option::get<bool>("deterministic"))
        return 1234;
    struct timeval tv;
    ::gettimeofday(&tv, NULL);
    return (tv.tv_sec + tv.tv_usec + getpid()); 
}

using namespace boost;

class Random : public Sampler
{
public:
	Random(Parameters&);
	StateChange changeState(SimState);

private:
	const uint64_t functional;
	const uint64_t warming;
	const uint64_t simulation;
	mt19937 rng;
	uniform_real<> range; 
    // FIXME: should be uniform_int<>, but different boost versions produce different values...
	variate_generator< mt19937&,uniform_real<> > rnd_functional;
};

using namespace std;

registerClass<Sampler,Random> random_c("random");

Random::Random(Parameters&p) : 
	Sampler(FULL_WARMING),
	functional (p.get<uint64_t>("functional")),
	warming    (p.get<uint64_t>("warming")),
	simulation (p.get<uint64_t>("simulation")),
    rng(seed()),
    range(0,functional*2),
    rnd_functional(rng,range)
{
}

Sampler::StateChange Random::changeState(SimState curr)
{
	StateChange next;
	switch(curr)
	{
		case FUNCTIONAL:  
			next = StateChange(FULL_WARMING,warming);
			break;

		case FULL_WARMING:
			next = StateChange(SIMULATION,simulation);
			break;
	
		case SIMULATION:
			next = StateChange(FUNCTIONAL, rnd_functional());
			break;

		default: ERROR(true,"SimState not handled");
	}
	return next;
}
