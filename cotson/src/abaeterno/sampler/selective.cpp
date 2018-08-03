// (C) Copyright 2006-2009 Hewlett-Packard Development Company, L.P.
// Copyright (c) 2015-2018 Roberto Giorgi - University of Siena
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//

// $Id: selective.cpp 727 2016-01-05 18:57:13Z robgiorgi $
// #define _DEBUG_THIS_

#include "abaeterno_config.h"
#include "abaeterno.h"
#include "logger.h"
#include "error.h"
#include "cotson.h"

#include "sampler.h"
#include "liboptions.h"
#include "cpuid_call.h"

using namespace std;
using namespace boost;
using namespace boost::assign;

class Selective : public Sampler
{
	public:
	Selective(Parameters&);
	~Selective() {}

	StateChange changeState(SimState);

private:
	int current;
	std::string changer;
	std::vector<boost::shared_ptr<Sampler> > samplers;
	void* functional_call(FunctionalState,uint64_t,uint64_t);
	InstructionInQueue simulation_call(Instruction*);
        void* functional_call_casm(FunctionalState,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t);
        InstructionInQueue simulation_call_casm(Instruction*);
};

// Data passed between functional and timing phase, locally managed (see pool below)
struct XData
{
    uint64_t arg1, arg2, imm;
    XData(uint64_t a1, uint64_t a2, uint64_t im):arg1(a1),arg2(a2),imm(im){}
};
boost::pool<> xdata_pool(sizeof(struct XData));

static inline XData* xdata(uint64_t a1, uint64_t a2, uint64_t im)
{
    XData* xdp = new (xdata_pool.malloc()) XData(a1,a2,im);
    LOG("a1",a1,"a2",a2,"im",im);
    return xdp;
}

registerClass<Sampler,Selective> selective_c("selective_tracing");

void* Selective::functional_call(FunctionalState fs,uint64_t b,uint64_t c)
{
	if(fs == ONLY_FUNCTIONAL) 
	{
	 	current=lexical_cast<int>(Option::run_function(changer, c==0, b));
		AbAeterno::get().break_sample();
	}
	return 0;
}

InstructionInQueue Selective::simulation_call(Instruction* inst)
{
	uint64_t  b = inst->get_xdata(2);
	uint64_t  c = inst->get_xdata(3);
	current=lexical_cast<int>(Option::run_function(changer, c==0, b));
	AbAeterno::get().break_sample();
	return DISCARD;
}

void* Selective::functional_call_casm(
    FunctionalState fs, uint64_t nanos, uint64_t devid,
    uint64_t op,uint64_t regop,uint64_t im)
{
    uint64_t a1 = CpuidCall::casm_read_arg1(regop);
    uint64_t a2 = CpuidCall::casm_read_arg2(regop);
    bool     sw = ((~im&0x80)>>7);
    int      zo = im & 0x7F;
    if(fs == ONLY_FUNCTIONAL) {
        LOG(__func__," nanos",nanos,"zo",zo,"sw",sw);
        LOG(__func__,"-- FUNCTIONAL MODE -- NO DATA TO PASS -- SWITCHING SAMPLER zone",zo,"sw",sw);
        current=lexical_cast<int>(Option::run_function(changer, sw, zo));
        AbAeterno::get().break_sample();
        return 0;
    } else {
        LOG(__func__," nanos",nanos,"a1",a1,"a2",a2,"im",im);
        LOG(__func__,"-- SIMULATION MODE -- PASSING DATA FROM FUNCTIONAL TO TIMING PHASES");
        return xdata(a1,a2,im);
    }
}

InstructionInQueue Selective::simulation_call_casm(Instruction* inst)
{
    XData* xdp = reinterpret_cast<XData*>(inst->get_xdata(0)); // saved execution data
    LOG(__func__,"xdp",xdp);
    if (!xdp) {
        LOG(__func__,"-- NO DATA FROM FUNCTIONAL MODE");
        return DISCARD;
    } else {
        uint64_t im = xdp->imm;
        bool     sw = ((~im&0x80)>>7);
        int      zo = im & 0x7F;
        LOG(__func__,"-- SWITCHING SAMPLER zone",zo,"sw",sw);
#ifdef _DEBUG_THIS_
        uint64_t a1 = xdp->arg1;
        uint64_t a2 = xdp->arg2;
        LOG(__func__,"a1",a1,"a2",a2,"im",im);
#endif
        current=lexical_cast<int>(Option::run_function(changer, sw, zo));
        AbAeterno::get().break_sample();
        return DISCARD;
    }
}

Selective::Selective(Parameters&p) : 
	Sampler(FUNCTIONAL),
	current(0),
	changer(p.get<string>("changer"))
{
	FunctionalCall f=bind(&Selective::functional_call,this,_1,_5,_6);
	SimulationCall s=bind(&Selective::simulation_call,this,_1);
	CpuidCall::add(COTSON_RESERVED_CPUID_SELECTIVE,f,s,0); 
        FunctionalCall fcasm=bind(&Selective::functional_call_casm,this,_1,_2,_3,_4,_5,_6);
        SimulationCall scasm=bind(&Selective::simulation_call_casm,this,_1);
        CpuidCall::add(COTSON_RESERVED_ASM_BASE+COTSON_RESERVED_CASM_SELECTIVE,fcasm,scasm,0);

	string constructor=p.get<string>("constructor");
	int quantity=p.get<int>("quantity");

	for(int i=0;i<quantity;i++)
	{
		luabind::object& luatable = Option::run_function(constructor, i);
		set<string> required_sam;
		required_sam += "type";
		Parameters o=Option::from_lua_table(luatable,required_sam,"Sampler");
		o.track();
		boost::shared_ptr<Sampler> sp(Factory<Sampler>::create(o));
		ERROR_IF(!sp,"error creating sampler number ", i);
		samplers.push_back(sp);
	}
}

Sampler::StateChange Selective::changeState(SimState st)
{
	return samplers[current]->changeState(st);
}
