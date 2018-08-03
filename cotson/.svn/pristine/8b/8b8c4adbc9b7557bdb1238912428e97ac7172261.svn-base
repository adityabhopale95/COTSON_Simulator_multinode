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

#ifndef NODE_H_
#define NODE_H_

#include "net_typedefs.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <math.h>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

class Node : boost::noncopyable
{
public:
	Node();
    Node(const sockaddr_in&,const MacAddress&,uint32_t);
    Node(const MacAddress&,uint32_t);
    ~Node() {}

	typedef boost::shared_ptr<Node> Ptr;

    // Get/set the socket addresses of this node.
    const sockaddr_in& data_addr() const { return data_addr_; }
    void data_addr(const sockaddr_in& a) { data_addr_ = a; }

    // Get the Mac and Id of this node
    const MacAddress& mac() const { return mac_; }
    const uint32_t id() const { return nodeid_; }

	// Node start/stop
    enum State {STOP = 0, RUNNING = 1};
    void stop() { state_ = STOP; }
    void start() { state_ = RUNNING; } 
    bool running() const { return sync_valid() && state_ == RUNNING; }
	void mac(const MacAddress &mac) { mac_ = mac; }
	void id(uint32_t id) { nodeid_ = id; }
	bool sync_valid() const { return nodeid_ && mac_; }
	bool data_valid() const { return Sockaddr::valid(data_addr_); }
	bool valid() const { return data_valid() && sync_valid(); }
    void warning();
	bool obsolete(uint32_t);

	// Time management
	uint64_t getSimtime() const { return simtime_; }
	void setSimtime(uint64_t st, uint16_t sn);
	uint64_t getRealtime() const { return realtime_; }

private:
    // The address of this port.
    sockaddr_in data_addr_;

    // MacAddress
    MacAddress mac_;

	// Nodeid
    uint32_t nodeid_;

    // The state of the node.
    State state_;

	// The last observed simulation and real time
	uint64_t simtime_;
	uint64_t realtime_;
	uint16_t seqno_;
	bool warned_;
};

std::ostream& operator<<(std::ostream&,const Node&);


#endif /*NODE_H_*/
