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

// $Id: sampler.h 118 2010-09-17 13:00:27Z paolofrb $
#ifndef SAMPLER_H
#define SAMPLER_H

#include "states.h"

class Sampler
{
public:
	Sampler(SimState p) : pre_state(p) {};
	virtual ~Sampler() {}

	typedef boost::tuple<SimState,uint64_t> StateChange;
	virtual StateChange changeState(SimState)=0;

	SimState preState() const { return pre_state; }

protected:
    uint32_t seed() const;

private:
	SimState pre_state;
};
#endif
