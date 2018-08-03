// -*- C++ -*-
// (C) Copyright 2008 Hewlett-Packard Development Company, L.P.
// $Id$
//
#include "net_timing_model.h"
#include "param_factory.h"
#include "parameters.h"
#include <iostream>

using namespace std;

struct DestinationInfo
{
	double last_quantum_delay;
	double next_quantum_delay;
	std::vector<double> packets_latency;
};

class NewDestBased : public TimingModel
{
public:
    NewDestBased(Parameters&);
    ~NewDestBased();

    void startquantum(uint64_t,uint64_t);
    void packet_event(const MacAddress&,const MacAddress&,int size,bool=false);
    uint64_t get_delay();
    uint32_t register_lat();

private:
    uint64_t latency;
    double switch_bandwidth;
    double switch_latency;
    double header_size;
    //cable bandwidth in bits/usec
    double bandwidth;
    //Fly time in usec (latency for the first byte of the header to cross the cable
    double fly_time;
    //Average hops
    double hops;
    uint64_t unknown_mac_lat;
    uint64_t broadcast_lat;
    uint64_t quantum;
	int	mac_prefix;

    typedef std::map<MacAddress,DestinationInfo *> DestinationMap;
    DestinationMap destinations;
    
    uint64_t packets, bytes;
};

namespace newdbased
{
	registerClass<TimingModel,NewDestBased> mediator_newdbased_timer_c("newdbased");
}

// Constructor
NewDestBased::NewDestBased(Parameters &p) : latency(0),quantum(0),mac_prefix(0xFA) 
{
	//Cable
	bandwidth = p.get<double>("bandwidth");
	fly_time = p.get<double>("fly_time");
	//Protocol - Switching mechanism
	header_size = p.get<double>("head_size");
	//Topology --In the future there will be an option to map the hops per source/destination
	hops = p.get<double>("av_distance");
	//Physical switch
	if (p.has("mac_prefix"))
	    mac_prefix = p.get<int>("mac_prefix");
    unknown_mac_lat = p.get<uint64_t>("unknown_mac_latency");
    broadcast_lat = p.get<uint64_t>("broadcast_latency");
    switch_latency = p.get<double>("switch_latency");
	switch_bandwidth = p.get<double>("switch_bandwidth","0.0",false);
	add("packets", packets);
	add("bytes", bytes);
	
	clear_metrics();
}

// Destructor
NewDestBased::~NewDestBased() {}

void NewDestBased::startquantum(uint64_t gt, uint64_t nextgt)
{
	cout << "START QUANTUM gt " <<gt << " nextgt " << nextgt << endl;

	for(DestinationMap::iterator i=destinations.begin();i!=destinations.end();i++)
	{
		i->second->last_quantum_delay = i->second->next_quantum_delay;
		i->second->next_quantum_delay = 0;
		i->second->packets_latency.clear();
	}
	
	(nextgt<gt) ? quantum = 10 : quantum = nextgt-gt;
}

void NewDestBased::packet_event(const MacAddress& src, const MacAddress& dst, int size, bool bcast)
{
	cout << "Packet event from " << src << " to " << dst <<" : Size: " << size << " (Bcast " <<bcast << ")"<<endl;
	
    if (src.byte(0) != 0xFA) 
    {
    	latency= unknown_mac_lat;
    	cout << " Unknown source MAC: latency = " << latency <<endl;
    	return;
    }
    
    if (dst.byte(0) != 0xFA) 
    {
    	latency = bcast ? broadcast_lat : unknown_mac_lat;
    	cout << " Unknown dest MAC: latency = " << latency <<endl;
    	return;
    }
    
    packets ++;
    bytes ++;
    
    DestinationMap::iterator dest_iter = destinations.find(dst);
    DestinationInfo *dest_info;
    
    if(dest_iter == destinations.end())
    {
    	cout << "   New destination found " << dst << endl;
    	//First packet for this destination
		dest_info= new DestinationInfo();
		dest_info->last_quantum_delay=0;
		dest_info->next_quantum_delay=0;
		destinations.insert(make_pair(dst,dest_info));
    }
	else
	{
		dest_info = dest_iter->second;	
    }
    //All the calculation is done in usec
    double zll = hops * (fly_time + switch_latency + header_size*8/bandwidth) + fly_time + size*8/bandwidth;
    if(switch_bandwidth > 0)
       zll += ((double)hops * size * 8)/switch_bandwidth;
    cout << "   ZLL = " << zll << endl;
    double accumulated_delay = dest_info->packets_latency.size() ? dest_info->last_quantum_delay : 0;
    double delta_quantum = ((double)quantum )/ (dest_info->packets_latency.size()+1); 
    
    cout << "   Calculate accumulated delay for quantum = " << quantum << " dQuantum="<<delta_quantum << " and events= " << dest_info->packets_latency.size() <<" and last quantum delay of " << accumulated_delay << endl;  
    
    for(vector<double>::iterator i=dest_info->packets_latency.begin(); i!=dest_info->packets_latency.end();i++)
    {
    	accumulated_delay += (*i - delta_quantum);
    	if (accumulated_delay < 0)
    		accumulated_delay = 0;
    	if (accumulated_delay > 0)
    		cout << "      ------ Accumulated " << (*i - delta_quantum) << " more up to " << accumulated_delay << endl;	
    }
    cout << "   For " << dest_info->packets_latency.size() << " events the accumulated delay was of "<<accumulated_delay << endl;
    
    dest_info->packets_latency.push_back(zll);
    
    latency=static_cast<uint64_t>(zll + accumulated_delay);
    
    if(zll + accumulated_delay - delta_quantum  > 0)
    {
    	dest_info->next_quantum_delay =  zll + accumulated_delay - delta_quantum;
    	cout << "   Taking to next quantum a delay of " << dest_info->next_quantum_delay << endl; 
    }
}

uint64_t NewDestBased::get_delay()
{
	cout << "  GET DELAY " << latency << endl;
    return latency;
}

uint32_t NewDestBased::register_lat()
{
    return latency;
}
