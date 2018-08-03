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
#include "error.h"
#include "sampler.h"
#include "liboptions.h"

class SimPointReader : public Sampler 
{
	public:
	SimPointReader(Parameters&);

	StateChange changeState(SimState);

	private:

	typedef std::pair<uint64_t,double> Point;
	typedef std::vector<Point> PointVec;
	PointVec points;
	PointVec::iterator which;
	
	const uint64_t warming;
	const uint64_t simulation;
};

using namespace std;
using namespace boost;

registerClass<Sampler,SimPointReader> simpoint_reader_c("simpoint_reader");

SimPointReader::SimPointReader(Parameters&p) : 
	Sampler(SIMULATION), // so that we run into FUNCTIONAL
	warming    (p.get<uint64_t>("warming")),
	simulation (p.get<uint64_t>("simulation"))
{
	ERROR("Simpoint_reader does not weight its samples yet");

	vector<uint32_t> simpoints=p.getV<uint32_t>("points");
	vector<double>   weights=p.getV<double>("weights");
	
	if(simpoints.size()!=weights.size())
		throw invalid_argument("simpoint_reader.points and simpoint_reader.weights file are inconsistent");

	double total=fabs(accumulate(weights.begin(),weights.end(),0.0)-1.0);
	if(total>0.005)
		throw invalid_argument("simpoint_reader.weights do not add to 1");
	
	map<uint32_t,double> points_;
	for(uint i=0;i<simpoints.size();i++)
		points_[simpoints[i]]=weights[i];

	points.push_back(Point(0,0)); //not used
	map<uint32_t,double>::iterator i=points_.begin();
	uint32_t v=i->first*simulation;
	if(v>warming)
		v-=warming;
	else
		v=0;
	points.push_back(Point(v,i->second));
			
	uint32_t previous=i->first;
	for(++i;i!=points_.end();previous=i->first,++i)
	{
		v=(i->first-previous)*simulation-simulation-warming;
		points.push_back(Point(v,i->second));	
	}
	which=points.begin();
}

Sampler::StateChange SimPointReader::changeState(SimState state)
{
	switch(state)
	{
		case FUNCTIONAL: 
		    if (warming) 
			    return StateChange(FULL_WARMING,warming);
			else 
			    return StateChange(SIMULATION,simulation);

		case FULL_WARMING:
			return StateChange(SIMULATION,simulation);
	
		case SIMULATION:
			which++;
			if (which==points.end())
				return StateChange(END,0);
			else	
				return StateChange(FUNCTIONAL,which->first);

		default: ERROR("SimState not handled");
	}

	return StateChange(FUNCTIONAL,0);
}
