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

// $Id: switch.h 187 2011-04-11 16:10:57Z paolofrb $

#ifndef SWICTH_H_
#define SWICTH_H_

#include "net_typedefs.h"
#include "node.h"
#include "mpacket.h"

#include <boost/thread/mutex.hpp>
#include <map>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <boost/ref.hpp>

#include "metric.h"
#include "net_timing_model.h"

// Forward declarations

class ControlPacketProcessor;
class DataPacketProcessor;
class Scheduler;
class DataPacketProcessor;
class Stats;
class Packet;
class DumpGzip;

class Switch : public metric
{
public:
    Switch(DataPacketProcessor*, Scheduler*, 
           uint32_t, uint16_t, uint8_t, const char*, 
           double, double, double, double,
           bool, const char*, int);
    ~Switch();

    // Registers a new node with mac and address:port
    Node::Ptr register_node(const MacAddress&,const sockaddr_in &);

    // Registers a new node with the nodeid
    Node::Ptr register_node(uint32_t,const sockaddr_in &);

    // Registers a new node with address:port
    Node::Ptr register_node(const sockaddr_in &);

    int process_packet(const Packet&,
                       const sockaddr_in&,
                       const MacAddress&,
                       const MacAddress&,
                       bool, bool);

    // Really send a packet to a node
    void really_send(const Node::Ptr, const Packet&);

    // UDP transmit pacing (throttle)
    void throttle(uint64_t);

    // A node stopped
    bool stop_node(Node&);

    // A node sent a heartbeat
    bool heartbeat_node(Node&);

    // Signal that simulation has advanced
    void start_nodes(const uint64_t quantum);

    // Set the vde on and off
    void vde_on(bool on) { vde_on_ = on; }

    // Set the slirp on and off
    void slirp_on(bool on) { slirp_on_ = on; }

    // Register a timing model in the switch
    inline void register_timing_model(TimingModel* timing) 
    { 
        timing_ = timing; 
        timing_->startquantum_mt(gt_,nextgt_);
        add("timer.", *timing_);
    }

    // Get the timing model
    TimingModel* get_timing() { return timing_; }

    // Termination
    void send_terminate();

    // Cpuid
    void send_cpuid(uint64_t,uint16_t,uint16_t);

    // Global time
    inline uint64_t GT() const { return gt_; }
    inline uint64_t nextGT() const { return nextgt_; }
    void timeout();
    void send_sync(bool); // Send a sync back to simulation
        bool sync_started() const { return sync_started_; }

private:
    inline void GT_advance();

    // Send a packet to a node (enqueue if needed)
    void send(const Node::Ptr, const Packet&, uint64_t);

    // Send a packet to a destination mac (enqueue if needed)
    bool send(const MacAddress&, const Packet&, uint64_t);

    // Broad cast a message from a destination to all nodes (enqueue if needed)
    int broadcast(const MacAddress&, const Packet&, uint64_t);

    // Get the node with that mac address.
    inline Node::Ptr find_node(const MacAddress&);

    // Get the node with that port
    inline Node::Ptr find_node(const sockaddr_in&);

    // Get the node with that id
    inline Node::Ptr find_node(uint32_t);

    // Get the node with that id
    void erase_dup_nodes(const sockaddr_in&);

    // Read quantum dynamically from file
    inline bool quantum_from_file();

    void die(const std::string&);
    bool simtime_advance(uint64_t);
    void sync_cluster(uint64_t);
    uint64_t compute_quantum(uint64_t,uint64_t);

    void dump_nodes();

    // The set of all nodes
    typedef std::set<Node::Ptr> NodeSet;
    NodeSet nodes_;
    int numnodes_;

    // The map to translate from macaddress to nodes 
    typedef std::map <MacAddress,Node::Ptr> NodesMacMap;
    NodesMacMap mac_map_;

    // Allows to send packets.
    DataPacketProcessor* processor_;

    // Schedule packets into the future.
    Scheduler* scheduler_;

    bool slirp_on_;
    bool vde_on_;
    uint32_t max_messages_;
    uint32_t mac_base_;

    // The timing model.
    TimingModel* timing_;
    int sync_socket_;
    sockaddr_in sync_addr_;
    uint64_t last_sync_;
    uint64_t last_qtime_;
    uint64_t last_qpacks_;
    double quantum_;
    const double qmin_;
    const double qmax_;
    const double qup_;
    const double qdown_;
    bool force_queue_;
    // Time data
    uint64_t gt_, nextgt_, tmin_, tmax_, nanos_;
    uint64_t beat_gt_, beat_interval_;
    // Compressed tracefile
    DumpGzip* const dump_; 
    boost::mutex nmutex_;
    uint16_t seqno_;
    bool sync_started_;
    int verbose_;
    int throttle_usec_;
};

#endif /*SWICTH_H_*/
