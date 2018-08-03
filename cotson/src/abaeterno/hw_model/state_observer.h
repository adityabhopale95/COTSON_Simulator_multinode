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

// $Id: state_observer.h 11 2010-01-08 13:20:58Z paolofrb $
#ifndef STATE_OBSERVER_H
#define STATE_OBSERVER_H

#include "states.h"
#include "liboptions.h"
#include "logger.h"
#include "cotson.h"

class StateObserver
{
	public:
	StateObserver() { registerSetParam<StateObserver*> a(this);	}
	virtual ~StateObserver() { unregisterSetParam<StateObserver*> a(this); }

	static void transition(SimState new_state)
	{
		typedef StaticSet<StateObserver*> AllStates;
		for(AllStates::iterator i=AllStates::begin();i!=AllStates::end(); ++i)
			(*i)->doSetState(new_state);
		sim_state()=new_state;
	}

	protected:
	virtual void doSetState(SimState new_state)=0; 
	static SimState& sim_state()
	{
		static SimState singleton(SIM_STATES);
		return singleton;
	}
};

class SimpleStateObserver : public StateObserver
{
	public:
	SimpleStateObserver() {}
	virtual ~SimpleStateObserver() {}

	protected:
	void doSetState(SimState new_state)
	{
		switch(sim_state()) 
		{
			case FUNCTIONAL:     endFunctional();    break;
			case SIMPLE_WARMING: endSimpleWarming(); break;
			case FULL_WARMING:   endFullWarming();   break;
			case SIMULATION:     endSimulation();    break; 
			default: break;
		}

		switch(new_state) 
		{
			case FUNCTIONAL:     beginFunctional();    break;
			case SIMPLE_WARMING: beginSimpleWarming(); break;
			case FULL_WARMING:   beginFullWarming();   break;
			case SIMULATION:     beginSimulation();    break; 
			default: break;
		}
	}

	protected:
	virtual void beginFunctional() {LOG("Beginning FUNCTIONAL");}
	virtual void endFunctional() {LOG("Ending FUNCTIONAL");}

	virtual void beginSimpleWarming() {LOG("Beginning SWARMING");}
	virtual void endSimpleWarming() {LOG("Ending SWARMING");}

	virtual void beginFullWarming() {LOG("Beginning FWARMING");}
	virtual void endFullWarming() {LOG("Ending FWARMING");}

	virtual void beginSimulation() {LOG("Beginning SIMULATION");}
	virtual void endSimulation() {LOG("Ending SIMULATION");}
};

#endif
