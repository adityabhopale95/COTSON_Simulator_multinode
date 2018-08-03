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

// $Id: no_timing.cpp 11 2010-01-08 13:20:58Z paolofrb $
#include "abaeterno_config.h"
#include "sampler.h"
#include "liboptions.h"

class NoTiming : public Sampler
{
	public:
	NoTiming(Parameters&);

	StateChange changeState(SimState);

	private:
	uint64_t quantum;
};

using namespace std;

registerClass<Sampler,NoTiming> no_timing_c("no_timing");

NoTiming::NoTiming(Parameters&p) : 
	Sampler(FULL_WARMING),
	quantum(p.get<uint64_t>("quantum")) { }

Sampler::StateChange NoTiming::changeState(SimState curr)
{
    return StateChange(FUNCTIONAL,quantum);
}
