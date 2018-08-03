// -*- C++ -*-
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

// $Id: mediator.cpp 146 2010-11-09 09:59:28Z paolofrb $

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdexcept>
#include <vector>
#include <map>
#include <set>
#include <sstream>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include "liboptions.h"

#include "mediator_server.h"
#include "net_timing_model.h"

using namespace std;

namespace 
{
    option  o01("help",                          "This help");
    option  o02("verbose",                  "0", "Increase verbosity");
    option  o03("quantum_min",            "1.0", "Minimum value for the quantum in us");
    option  o04("quantum_max",         "1000.0", "Maximum value for the quantum in us");
    option  o05("quantum_up",        "1.031250", "Quantum increase factor"); //==33/32
    option  o06("quantum_down",      "0.031250", "Quantum decrease factor"); //==1/32
    option  o08("autoscan_ports",       "false", "automatically scan for ports");
    option  o09("control_port",          "8196", "Control port to be used");
    option  o10("data_port",             "8197", "Data port to be used");
    option  o11("multicast_port",        "8198", "Multicast port to be used");
    option  o07("multicast_ip",   "239.255.0.1", "Multicast IP address");
    option  o14("multicast_ttl",           "15", "Set multicast TTL");
    option  o13("macbase",         "4207739136", "Set MAC base");//==0xFACD0100
    option  o12("save_config_file",          "", "dump config to this file");
    option  o15("vde",                       "", "vde socket to connect");
    option  o16("slirp",                 "true", "Enable slirp connection");
    option  o17("slirp_network", "192.168.10.0", "Sets the base DHCP ip for the VDE");
    option  o18("queue_packets",         "true", "Queue packets for the next quantum");
    options o19("redirect_port",                 "Array of redirections in format n:p1:p2 (Redirect port <p1> of slirp node <n> to host port <p2>)");
    option  o20("tracefile",                 "", "Sets a file for the mediator to store a trace of the network communication");
    optionR o21("timer",                         "Sets up a mediator timer");
    optionR o22("heartbeat",                     "Sets up a mediator heartbeat");
    option  o23("heartbeat_interval",     "1000", "packets between db logs");
    option  o24("network.mediator_nodeid",   "0", "cluster node id (for db logs)");
    option  o25("sync_heartbeat",         "1000", "sync heartbeat (ms)");
    option  o26("throttle",                 "10", "Transmit pacing (min delay) between udp sendto (us)");
}

namespace 
{
    bool socket_free(int port)
    {
        int sockfd = ::socket(AF_INET, SOCK_DGRAM, 0);
        if(sockfd < 0) return false;

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        bool valid=(::bind(sockfd,(struct sockaddr *)&addr, sizeof(addr)) != -1);
        close(sockfd);
        return valid;
    }

    int find_free_port(int start_port,int end_port)
    {
        for(int i=start_port;i<=end_port;i++)
            if(socket_free(i))
                return i;
        return 0;
    }
}

TimingModel* get_timing(Parameters &timingconf)
{
    return Factory<TimingModel>::create(timingconf);
}

int main(int argc,char*argv[])
{
    vector<uint32_t> slirp_redir_node;
    vector<uint16_t> slirp_redir_port1;
    vector<uint16_t> slirp_redir_port2;

    try 
    {
        Option::init(argc,argv,"mediator");
        
        if (Option::has("help"))
        {
            Option::print(cout,"Mediator");
            cout << kill;
        }
        
        //first we read all variables (most have defaults in options)
        int verbose= Option::get<int>("verbose"); 
        double quantum_min = Option::get<double>("quantum_min");
        double quantum_max = Option::get<double>("quantum_max");
        double quantum_up = Option::get<double>("quantum_up");
        double quantum_down = Option::get<double>("quantum_down");
        string multicast_ip = Option::get<string>("multicast_ip");
        uint16_t multicast_ttl = Option::get<uint16_t>("multicast_ttl");
        uint32_t macbase = Option::get<uint32_t>("macbase");

        bool queue_packets=Option::get<bool>("queue_packets");
        bool slirp=Option::get<bool>("slirp");
        string slirp_network = Option::get<string>("slirp_network");

        bool autoscan_ports=Option::get<bool>("autoscan_ports");
        uint16_t control_port, data_port, multicast_port;

        if(autoscan_ports)
        {
            ::srand(::time(0));
            int base = 25000 + (::rand() % 1024);
            control_port=find_free_port(base,base+10000);
            data_port=find_free_port(control_port+1,30000);
            multicast_port=find_free_port(data_port+1,30000);
            if(!data_port || !control_port || !multicast_port) 
                cerr << "could not find free ports" << kill;
        }
        else
        {
            control_port = Option::get<uint16_t>("control_port");
            data_port = Option::get<uint16_t>("data_port");
            multicast_port = Option::get<uint16_t>("multicast_port");
        }

        string save_config_file=Option::get<string>("save_config_file");
        string vde=Option::get<string>("vde");
        string tracefile=Option::get<string>("tracefile");

        if(Option::has("redirect_port"))
        {
            vector<string> q = Option::getV<string>("redirect_port");
            for(vector<string>::iterator i= q.begin(); i!=q.end();i++)
            {
                boost::tokenizer<> tok(*i);
                boost::tokenizer<>::iterator beg=tok.begin();
                if(beg==tok.end())
                    cerr << "ERROR: Invalid redirection " << *i << kill;
                slirp_redir_node.push_back(::strtoul(beg->c_str(),0,0));

                beg++;
                if(beg==tok.end())
                    cerr << "ERROR: Invalid redirection " << *i << kill;
                slirp_redir_port1.push_back(::strtoul(beg->c_str(),0,0));
                
                beg++;
                if(beg==tok.end())
                    cerr << "ERROR: Invalid redirection " << *i << kill;
                slirp_redir_port2.push_back(::strtoul(beg->c_str(),0,0));
            }
        }


        // we now check for incorrect values
        if (quantum_min > quantum_max || quantum_up < 1.0 || quantum_down > 1.0) 
            cerr << "ERROR: illegal quantum specification [" 
                << quantum_min <<":"<< quantum_max <<":"<< quantum_up  <<":"<< quantum_down << "]" << kill;

        if (data_port < 1024)  
            cerr << "ERROR: illegal data_port value (" << data_port  << ")" << kill;
        if (control_port < 1024)  
            cerr << "ERROR: illegal control_port value (" << control_port  << ")" << kill;
        if (multicast_port < 1024)  
            cerr << "ERROR: illegal multicast_port value (" << multicast_port  << ")" << kill;
        
        if (macbase == 0 || macbase > 0xffffff00)  
            cerr << "ERROR: illegal macbase value (" << hex << macbase << ")" << kill;

        switch(multicast_ttl) 
        {
            // case 0: case 1: case 32: case 64: case 128: case 255:
            case 32: case 64: case 128: case 255: break;
            default:
                if (multicast_ttl <= 16) 
                {
                    cerr << "WARNING: setting multicast TTL to 15 "
                        << "(some routers have problems with small TTLs)"
                        << endl;
                    multicast_ttl = 15;
                    break;
                }
                cerr << "ERROR: illegal multicast TTL value (" 
                    << (int)multicast_ttl << ")" << endl
                    << "       Valid TTL values are"
                    << "       0: host, 1: subnet, 32: site 64: region, "
                    << "128: continent, 255 unrestricted" << kill;
        }

        stringstream out;

        out << "control_port: " << control_port << endl;
        out << "data_port: " << data_port << endl;
        out << "multicast_port: " << multicast_port << endl;
        out << "multicast_ip: \"" << multicast_ip << "\"" << endl;
        out << "multicast_ttl: " << multicast_ttl << endl;
        out << "macbase: " << macbase << endl;

        out << "quantum_max: " << quantum_max << endl;
        out << "quantum_min: " << quantum_min << endl;
        out << "quantum_up: " << quantum_up << endl;
        out << "quantum_down: " << quantum_down << endl;
        out << "queue_packets: " << queue_packets << endl;
        out << "slirp: " << slirp << endl;
        out << "slirp_network: \"" << slirp_network << "\"" << endl;

        if(!save_config_file.empty())
        {
            ofstream a(save_config_file.c_str());
            a << out.str();
            a.close();
        }           
        
        MediatorServer server(
            control_port, 
            data_port, 
            slirp ? slirp_network.c_str() : 0,
            multicast_ip.c_str(),
            multicast_port, 
            multicast_ttl,
            vde.empty() ? 0 : vde.c_str(), 
            macbase,
            quantum_min,quantum_max,quantum_up,quantum_down,
            queue_packets,
            slirp_redir_node,slirp_redir_port1,slirp_redir_port2,
            tracefile.empty() ? 0 : tracefile.c_str(),
            verbose);

        if(Option::has("timer"))
        {
            set<string> required_tim;
            required_tim.insert("type");
            Parameters timer_param=Option::from_named_option("timer",required_tim,"Mediator");
            timer_param.set("name", "timer");
            timer_param.track();
            
            // Instantiate the right timing model according to configuration file
            TimingModel* timing = get_timing(timer_param);
            if (timing) {
                // Configure and register the timing model
                timing->verbose(verbose);
                server.register_timing_model(timing);
            }
        }
        
        server.init();
        server.run();
    }
    catch(exception& err)
    {
        cerr << "caught exception: " << err.what() << kill;
    }
    return 0;
}
