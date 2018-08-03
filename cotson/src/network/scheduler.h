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

// $Id: scheduler.h 11 2010-01-08 13:20:58Z paolofrb $
//
#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "net_typedefs.h"
#include "mpacket.h"
#include "node.h"

#include <boost/thread/mutex.hpp>
#include <stdint.h>
#include <string.h>
#include <set>

// Forward decls
class Node;
class Packet;
class Switch;

class Message
{
public:
    inline Message(const Packet& p, const Node::Ptr n, uint64_t st, uint64_t rt):
	    packet_(new Packet(p)),node_(n),simtime_(st),realtime_(rt) {}
    inline ~Message() {}
    inline uint64_t realtime() const { return realtime_; }
	inline uint64_t simtime() const { return simtime_; }
    inline const Node::Ptr node() const { return node_; }
    inline const Packet& packet() const { return *packet_; }
    inline Message& operator=(const Message& that) 
	{
	    node_ = that.node_;
		packet_ = that.packet_;
		simtime_ = that.simtime_;
		realtime_ = that.realtime_;
	    return *this;
	}

private:
    // Pointer to packet to send
	Packet::Ptr packet_;

    // Pointer to destination of the message
    Node::Ptr node_;

    // Message timestamps
    uint64_t simtime_;
    uint64_t realtime_;
};

class PrioritizeMessages
{
public :
    // Sort messages by inverse simtime first, and then by inverse realtime
	// (that is, smaller simtime time and then smaller realtime go first)
    inline int operator()(const Message &x, const Message &y) const
	{
	    return    ( x.simtime() > y.simtime() )
		       || (   x.simtime() == y.simtime()
			       && x.realtime() > y.realtime() ); 
	}
};

class Scheduler
{
public:
    Scheduler(Switch*, int);
    ~Scheduler();

    // Stars the scheduler
    void start(int);

    // Release all messages until time t 
	// (returns number of sent packets and top packet time)
    std::pair<uint64_t,uint64_t> send_packets(uint64_t);

    // Schedule a submission of a <Packet> to a <Node> for <Time>
    void schedule(const Packet&, const Node::Ptr, uint64_t);

private:
    // Sorted message queue (points to packets_)
	typedef std::set<Message,PrioritizeMessages> MsgQueue;
    MsgQueue messages_;

    // The switch that conatins all the nodes
    Switch* switch_;

	// Mutex to guard the packet queue
	boost::mutex qmutex_;
	int verbose_;
};

inline std::ostream& operator<<(std::ostream& s,const Message& m)
{
	return s << "message: [" << m.packet() << "]"
	         << " to [" << (*m.node()) << "]"
			 << " st " << m.simtime() 
			 << " rt " << m.realtime();
}

#endif /*SCHEDULER_H_*/
