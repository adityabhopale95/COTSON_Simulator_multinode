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

// $Id: node.cpp 71 2010-02-10 15:35:54Z paolofrb $

#include "node.h"
#include "stats.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

Node::Node()
    : data_addr_(Sockaddr::null<sockaddr_in>()),
	  mac_(),nodeid_(0),state_(STOP),
	  simtime_(0),realtime_(0),seqno_(0),warned_(false)
{}

Node::Node(const sockaddr_in& addr, const MacAddress& mac, uint32_t id)
    : data_addr_(addr),mac_(mac),nodeid_(id),state_(STOP),
	  simtime_(0),realtime_(Stats::get().elapsed()),
	  seqno_(0), warned_(false)
{}

Node::Node(const MacAddress& mac, uint32_t id)
    : data_addr_(Sockaddr::null<sockaddr_in>()),
	  mac_(mac),nodeid_(id),state_(STOP),
	  simtime_(0),realtime_(Stats::get().elapsed()),
	  seqno_(0), warned_(false)
{}

void Node::setSimtime(uint64_t st, uint16_t sn)
{
	uint16_t next_sn = seqno_ + 1;
	if (sn != next_sn) {
	    std::cerr << "Warning: missed timestamp, node " << nodeid_ 
		          << " simtime " << simtime_
		          << " msgtime " << st
		          << " prevseq " << seqno_
		          << " seq " << sn
				  << std::endl;
	}
	if (simtime_ == 0) {
	    std::cout << "Node " << nodeid_ << " initial simtime " << st << std::endl;
	}
    realtime_ = Stats::get().elapsed();
	if (st > simtime_) { // UDP packets may be OoO
        simtime_ = st; 
    }
	seqno_ = sn;
}

void Node::warning()
{
	if (warned_)
	    return;
    std::cerr << "(SYNC) Warning: message from unregistered node id=" 
              << id() << " mac=" << mac().str() 
			  << std::endl;
    warned_=true;
}

bool Node::obsolete(uint32_t timeout_s)
{
	uint64_t now = Stats::get().elapsed();
	uint64_t t = realtime_ > now ? 0 : realtime_;
	return (!mac_ && !nodeid_ && now > t + timeout_s * 1000000ULL);
}

std::ostream& operator<<(std::ostream& s,const Node& n)
{
	return s << "id " << n.id()
	         << " mac " << n.mac().str()
	         << " addr " << Sockaddr::str(n.data_addr())
			 << " st " << n.getSimtime()
			 << " rt " << n.getRealtime()
			 << (n.sync_valid() ? " sync" : "")
			 << (n.data_valid() ? " data" : "")
			 << (n.running() ? " run" : "");
}

