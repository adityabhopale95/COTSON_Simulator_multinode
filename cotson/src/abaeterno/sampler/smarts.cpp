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

// $Id: smarts.cpp 11 2010-01-08 13:20:58Z paolofrb $
#include "abaeterno_config.h"

//SMARTS PARAMETERS:
//        
// N = number of dynamic instructions of the whole simulation (e.g., 1 billion)
// n = number of samples you want (e.g., 10000)
// W = number of instructions for detailed warming (e.g., 2000)
// U = number of instructions of sampling unit (e.g., 1000)
//        
// with these numbers we obtain:
//        
// k = sample length (including warming) = N/n = 1 billion/10000 = 100000
// f = functional warming length = k-W-U = 100000-2000-1000 = 97000
//                        
// The simulation is as follows (for every k instructions):
//        
// [-----------------------------------------------------------------------]
// |                         f                          |     W     |  U  |

#include "sampler.h"
#include "liboptions.h"

class Smarts : public Sampler
{
public:
	Smarts(Parameters&);

	StateChange changeState(SimState);

private:
	const uint64_t functional;
	const uint64_t warming;
	const uint64_t simulation;
};

registerClass<Sampler,Smarts> smarts_c("smarts");

Smarts::Smarts(Parameters&p) : 
	Sampler(FULL_WARMING),
	functional (p.get<uint64_t>("functional")),
	warming    (p.get<uint64_t>("warming")),
	simulation (p.get<uint64_t>("simulation"))
{}

Sampler::StateChange Smarts::changeState(SimState state)
{
	switch (state) 
	{
		case SIMPLE_WARMING: return StateChange(FULL_WARMING,warming);
		case FULL_WARMING:   return StateChange(SIMULATION,simulation);
		case SIMULATION:     return StateChange(SIMPLE_WARMING,functional);
		default: throw std::runtime_error("Sim state not handled");
	}
	return StateChange(SIM_STATES,0);
}
