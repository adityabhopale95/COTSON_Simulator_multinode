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

class Interval : public Sampler
{
public:
	Interval(Parameters&);

	StateChange changeState(SimState);

private:
	const uint64_t functional;
	const uint64_t warming;
	const uint64_t simulation;
};

using namespace std;

registerClass<Sampler,Interval> interval_c("interval");

Interval::Interval(Parameters&p) : 
	Sampler(FULL_WARMING),
	functional (p.get<uint64_t>("functional")),
	warming    (p.get<uint64_t>("warming")),
	simulation (p.get<uint64_t>("simulation"))
{}

Sampler::StateChange Interval::changeState(SimState state)
{
	switch (state) 
	{
		case FUNCTIONAL:   return StateChange(FULL_WARMING,warming);
		case FULL_WARMING: return StateChange(SIMULATION,simulation);
		case SIMULATION:   return StateChange(FUNCTIONAL,functional);
		default: ERROR("Sim state not handled");
	}
	return StateChange(SIM_STATES,0);
}
