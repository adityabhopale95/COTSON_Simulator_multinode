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

// $Id$

#ifndef MEDIATORSERVER_H_
#define MEDIATORSERVER_H_

#include "reactor.h"
#include "switch.h"
#include "stats.h"
#include "packet_acceptor.h"
#include "control_packet_processor.h"
#include "data_packet_processor.h"
#include "slirp_acceptor.h"
#include "vde_acceptor.h"
#include "scheduler.h"

class MediatorServer
{
public:
    MediatorServer(uint16_t ctrl_port,
                   uint16_t data_port,
                   const char* slirp_network,
                   const char* mcast_ip,
                   uint16_t mcast_port,
                   uint8_t mcast_ttl,
                   const char *vde_file,
                   uint32_t mac_base,
				   double qmin, double qmax, 
				   double qup, double qdown,
				   bool force_queue,
				   const std::vector<uint32_t>& slirp_redir_node,
				   const std::vector<uint16_t>& slirp_redir_port1,
				   const std::vector<uint16_t>& slirp_redir_port2,
				   const char* tracefile,
                   int verbose);

    ~MediatorServer() {}

    // Initialize the server
    int init (void);

    // Run the server
    int run (void);

    // Set verbose mode.
    void verbose(int);

    // Register a timing model
    void register_timing_model(TimingModel*);

private:
    // The siwtch that registers all the ports and mac addresses
    Switch switch_;

    // The reactors that handle event multiplexing.
    Reactor data_reactor_;
    Reactor ctrl_reactor_;

	// Heartbeat (sends periodic time events to the cluster)
	void Heartbeat();

    // The PacketProcessor for control and data packets;
    ControlPacketProcessor control_processor_;

    // The PacketProcessor for control and data packets;
    DataPacketProcessor data_processor_;

    // The acceptor for control packets
    PacketAcceptor control_acceptor_;

    // The acceptor for the data port
    PacketAcceptor data_acceptor_;

    // The VDE acceptor
    VDEAcceptor vde_acceptor_;

    // The Slirp acceptor (reference to a singleton class)
    SlirpAcceptor& slirp_acceptor_;

    // Schedueler
    Scheduler scheduler_;

	// Others
    const uint16_t ctrl_port_;
    const uint16_t data_port_;
	const char* slirp_network_;
	const std::vector<uint32_t> slirp_redir_node_;
	const std::vector<uint16_t> slirp_redir_port1_;
	const std::vector<uint16_t> slirp_redir_port2_;
	const char* mcast_ip_;
    const char *vde_file_;
    int verbose_;
};

#endif /*MEDIATORSERVER_H_*/

/* vi: set sw=4 ts=4: */
