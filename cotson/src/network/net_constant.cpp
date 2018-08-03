// -*- C++ -*-
// (C) Copyright 2008 Hewlett-Packard Development Company, L.P.
// $Id: net_constant.cpp 141 2010-11-08 11:29:10Z paolofrb $
//
#include "net_timing_model.h"
#include "param_factory.h"
#include "parameters.h"

#include <iostream>

using namespace std;

// Example of the a switch timing model, returning a constant latency

class Constant : public TimingModel
{
public:
    Constant(Parameters &p);
    ~Constant();

    void configure();
    void startquantum(uint64_t,uint64_t);
    void packet_event(const MacAddress&,const MacAddress&,int,bool=false);
    uint64_t get_delay();
    uint32_t register_lat() { return 0; }
private:
    uint64_t latency_;
};

// Register the current timing model 
namespace constant 
{
	registerClass<TimingModel,Constant> constant_c("constant");
}

Constant::Constant(Parameters &p) : 
    latency_(p.get<uint64_t>("latency","1"))
{
}

Constant::~Constant()
{
}

void Constant::startquantum(uint64_t gt, uint64_t nextgt) 
{ 
    // Called at the beginnning of every synchronization quantum.
	// gt     : the last time when the cluster was synchronized (in us)
	// nextgt : the next time to be synchronized (in us)
}

void Constant::packet_event(const MacAddress& src, const MacAddress& dst, int size, bool bcast) 
{ 
    // Called when a packet is processed by the switch model
	// src     : source MAC address
	// dst     : dst MAC address
	// size    : packet size in bytes
	// bcast   : whether the packet came from a broadcast
}

uint64_t Constant::get_delay() 
{
    // Called when the switch requests the latency of the last packet
	// (passed to packet_event)
    return latency_; 
}
