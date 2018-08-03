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

#include "data_packet_processor.h"
#include "switch.h"
#include "stats.h"
#include "log.h"

#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <errno.h>
#include <stdio.h>

using namespace std;

DataPacketProcessor::DataPacketProcessor(Switch *sw, int v):PacketProcessor(sw,v)
{}

DataPacketProcessor::~DataPacketProcessor()
{}

int DataPacketProcessor::process(EventHandler *handler)
{
	const ether_header *eh = reinterpret_cast<const ether_header*>(packet_.buf());
	MacAddress src(eh->ether_shost);
	MacAddress dst(eh->ether_dhost);

    // Discard IPv6
    if (ntohs(eh->ether_type) == ETH_P_IPV6) { // 0x86dd
		LOG1("Ignore IPv6 packet from ",src.str(),"to", dst.str());
        return 0;
    }

    // Deal with the rest
    bool from_slirp = (handler->type() == EventHandler::SLIRP);
    bool from_vde = (handler->type() == EventHandler::VDE);
    const sockaddr_in& from = *reinterpret_cast<const sockaddr_in*>(handler->from());
     if (!from_vde && !from_slirp)
         LOG2("(DATA) Peer", Sockaddr::str(from), 
		    "src", src.str(),
		    "dst", dst.str(),
			"type", eh->ether_type,
 			"size", packet_.len());

    if (switch_->process_packet(packet_,from,src,dst,from_slirp,from_vde) == -1)
        return -1;

    return 0;
}

int DataPacketProcessor::send_data_packet(
    const sockaddr_in& to,
    const Packet& packet,
    const char* description)
{
   LOG2("(DATA) Send ",description," len [",packet.len() ,"] to ",Sockaddr::str(to));

    int num_bytes = 
	    send_packet(data_handler_,
                    reinterpret_cast<const sockaddr*>(&to),
					sizeof(sockaddr_in),
					packet,
					"DATA fwd");
    return num_bytes;
}

int DataPacketProcessor::send_vde_packet(
    const Packet& packet,
    const char* description)
{
    LOG2("(DATA) Send ",description," len [",packet.len() ,"]");

    int num_bytes = 
	    send_packet(vde_handler_,
					vde_handler_->from(),
					sizeof(sockaddr_un),
                    packet,
					"VDE fwd");
    return num_bytes;
}

int DataPacketProcessor::send_slirp_packet(
    const Packet& packet,
    const char* description)
{
    LOG2("(DATA) Send ",description," len [",packet.len() ,"]");

    int num_bytes = 
	    send_packet(slirp_handler_,
					slirp_handler_->from(),
					sizeof(sockaddr_in),
                    packet,
					"SLIRP fwd");
    return num_bytes;
}

int DataPacketProcessor::send_packet(
	EventHandler *handler,
    const sockaddr* addr,
    size_t addr_len,
	const Packet& p,
    const char* description)
{
    int retries = 0;
	const uint8_t* buf = p.buf();
	size_t len = p.len();
    size_t num_bytes = 0;
	do {
        if ((num_bytes = handler->sendto(buf, len, addr, addr_len)) != len) {
            timespec ts = {0,20000000}; // wait for 20ms 
            ::nanosleep(&ts,0);
            cerr << "WARNING: " << description << " - Retrying sendto "
			    << "(sent " << static_cast<int64_t>(num_bytes)
				<< " expected " << len << ")" << endl;
		    ::perror("DataPacketProcessor::send_packet");
	    }
    }
    while (retries++ < MAX_SEND_RETRIES && num_bytes != len && errno != EFAULT);

    if (num_bytes != len) {
        cerr << "ERROR: " << description
		     << " to " << Sockaddr::str(addr)
		     << " failed (len=" << len << ")" << endl;
		::perror("DataPacketProcessor::send_packet");
		return 0;
	}
    return num_bytes;
}

