// -*- C++ -*-
// (C) Copyright 2008 Hewlett-Packard Development Company, L.P.
// $Id: net_dest_based.cpp 11 2010-01-08 13:20:58Z paolofrb $
//
#include "net_timing_model.h"
#include "param_factory.h"
#include "parameters.h"
#include <iostream>

using namespace std;
//namespace po = boost::program_options;

class DestBased : public TimingModel
{
public:
    DestBased(Parameters&);
    ~DestBased();

//    void configure();
    void startquantum(uint64_t,uint64_t);
    void packet_event(const MacAddress&,const MacAddress&,int size,bool=false);
    uint64_t get_delay();
    uint32_t register_lat();

private:
    uint64_t latency_;
    double   trace_lat_;
    uint64_t bcast_lat_;
    uint64_t unknown_mac_lat_;
    uint64_t quantum_;
    int      hops_;
	int      mac_prefix_;
    double   Knc, Ksw;
    double   rout_t, mapp_t, arb_t,cross_t, cross_conf_t, vl_arb_t, rel_lk_t,
             vl_arb_flowpk_t, vl_arb15_t, fly_t, BW, head_size;

    typedef std::vector<double> DestVecQueue;
    std::vector<double>::iterator queue_iter;

    // Data structure to implement a queue for each destination
    struct Dest_info { DestVecQueue queue_vec; };

    // The map to translate destination info
    typedef std::map <MacAddress,Dest_info*> DestMacMap;
    DestMacMap dest_map_;
    std::map<MacAddress,Dest_info*>::iterator dest_iter;

//  Methods to calculate physical, trip and contention latencies
    void    calc_phy_lats(double *lo, double *ttl, double pk_size);
    double  calc_acc_lat(DestVecQueue& queue, bool nextQ, double zero_lat);
};

namespace dbased
{
	registerClass<TimingModel,DestBased> mediator_dbased_timer_c("dbased");
}

// Constructor
DestBased::DestBased(Parameters &p) : latency_(0),trace_lat_(0),quantum_(0),mac_prefix_(0xFA) 
{
    rout_t          =   p.get<double>("routing_time");
    mapp_t          =   p.get<double>("mapping_time");
    arb_t           =   p.get<double>("arbitration_time");
    cross_t         =   p.get<double>("crossing_time");
    cross_conf_t    =   p.get<double>("crossbar_config_time");
    vl_arb_t        =   p.get<double>("vl_arbitration_time");
    rel_lk_t        =   p.get<double>("release_lkaccess_time");
    vl_arb_flowpk_t =   p.get<double>("vl_arbitration_flowpk_time");
    vl_arb15_t      =   p.get<double>("vl_arbitration_15_time");
    fly_t           =   p.get<double>("fly_time");
    head_size       =   p.get<double>("head_size");
    BW              =   p.get<double>("bandwidth");
    hops_           =   p.get<int>("av_distance");
    unknown_mac_lat_=   p.get<int>("lat_unknown_mac");
    bcast_lat_      =   p.get<int>("broadcast_lat");
	if (p.has("mac_prefix"))
	    mac_prefix_     =   p.get<int>("mac_prefix");

    Knc = mapp_t + vl_arb_t + fly_t;       // Physical delay of network adapter
    Ksw =   rout_t + mapp_t + vl_arb_t     // Physical delay of switch internals
          + cross_t + cross_conf_t
          + vl_arb_t + fly_t; //Has the arbitration time twice, is that right??
          
    //switch_latency = rout_t + arb_t + cross_conf_t + cross_t + rel_lk_t; 
}

// Destructor
DestBased::~DestBased() {}

void DestBased::startquantum(uint64_t gt, uint64_t nextgt)
{
    Dest_info* Dest_info_ptr;
    double next_acc=0;
	
	cout << "START QUANTUM" << endl;

	// Update and clean queue info to be used at next quantum
    for (dest_iter=dest_map_.begin() ; dest_iter != dest_map_.end(); dest_iter++) {
        Dest_info_ptr=(*dest_iter).second ;

		// Latency accumulated to next Quantum
        if(Dest_info_ptr->queue_vec.size()!=0) {
            next_acc = calc_acc_lat(Dest_info_ptr->queue_vec,true, 0); 
            Dest_info_ptr->queue_vec.clear();
            if (next_acc!=0) 
			    Dest_info_ptr->queue_vec.push_back(next_acc);
        }
    }
    // If   Quantum is negative assume a default value=10
    (nextgt<gt) ? quantum_ = 10 : quantum_ = nextgt-gt;
}

void DestBased::packet_event(const MacAddress& src, const MacAddress& dst, int size, bool bcast)
{
    Dest_info* dest_ptr;
    double lat_0, TtoL;
    lat_0=0;
    TtoL=0;

	cout << "Packet event from " << src << " to " << dst <<" : Size: " << size << " (Bcast " <<bcast << ")"<<endl;

    if (src.byte(0) == 0xFA) 
    {
        if (dst.byte(0) == 0xFA) 
        { // We just Model pks from valid MACs ("FA:xx:xx:...")
        	calc_phy_lats(&lat_0, &TtoL, (double) size);
            trace_lat_ = lat_0;

            if (dest_map_.empty()) 
            {
                dest_ptr = new Dest_info;
                dest_map_.insert(make_pair(dst,dest_ptr));
            } 
			else 
			{
                dest_iter = dest_map_.find(dst);
                if (dest_iter == dest_map_.end()) 
                {
                    dest_ptr = new Dest_info;
                    dest_map_.insert(make_pair(dst,dest_ptr));
                } 
				else 
				{
                    dest_ptr=dest_iter->second;
                    lat_0 = calc_acc_lat(dest_ptr->queue_vec,false,lat_0);
                }
            }
            dest_ptr->queue_vec.push_back(TtoL);
            latency_= static_cast<uint64_t>(lat_0);
        }
		else {
			// Bcast pk's and unrecognized MACs are assigned with a fixed Latency
			trace_lat_ = latency_ = bcast ? bcast_lat_ : unknown_mac_lat_;
        }
    }
	else {
        latency_= unknown_mac_lat_;
        trace_lat_ = latency_;
    }
}

uint64_t DestBased::get_delay()
{
	cout << "  GET DELAY " << latency_ << endl;
    return latency_;
}

void DestBased::calc_phy_lats(double *lo, double *ttl, double pk_size)
{
    *lo  = (Knc + (pk_size*8/BW) + hops_*(Ksw+(head_size*8/BW))); // Zero Load Latency calculation
    *ttl = (pk_size*8/BW) + vl_arb_t + rel_lk_t ; //  Contention time produced by the pk
}

uint32_t DestBased::register_lat()
{
    return static_cast<uint64_t>(trace_lat_);
}

double DestBased::calc_acc_lat(DestVecQueue& queue, bool nextQ, double zero_lat)
{
    unsigned int i;
    int     pk_num;
    double  sub_q, last_acc, acc;

    pk_num=0;
    sub_q=0;
    last_acc=0;
    acc=0;

    pk_num=queue.size();
    if(!nextQ) 
	    pk_num++;

    // Equidistant interarrival approach. Next I'll include Poisson, uniform, etc...
    if (pk_num>0) sub_q= (double)quantum_/(double)pk_num;
    else return (zero_lat+acc);

    for(i=0; i<queue.size(); i++) {
        acc=last_acc+ queue[i]- sub_q; // Accumulated latency before packet can be delivered
        if(acc<0) 
		    acc=0; // No contention at this dst
        last_acc=acc;
    }
    if(nextQ) return acc;
    else {
        trace_lat_ = zero_lat+acc; // modeled latency
        return ((zero_lat+acc) + sub_q*(double)i); // latency plus supposed arrival time
    }
}
