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

#include "switch.h"
#include "mpacket.h"
#include "data_packet_processor.h"
#include "scheduler.h"
#include "stats.h"
#include "net_timing_model.h"
#include "log.h"

#include "dump_gzip.h"
#include "liboptions.h"
#include "libmetric.h"

#include <iostream>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <netinet/ether.h>

using namespace std;

Switch::Switch(
    DataPacketProcessor* processor, 
    Scheduler* scheduler, 
    uint32_t m, 
    uint16_t sync_port, 
    uint8_t mcast_ttl, 
    const char* sync_ip, 
    double qmin, 
    double qmax, 
    double qup, 
    double qdown, 
    bool force_queue,
    const char* tracefile,
    int v) :
        nodes_(),
        numnodes_(0),
        mac_map_(),
        processor_(processor),
        scheduler_(scheduler),
        slirp_on_(false),
        vde_on_(false),
        mac_base_(m),
        timing_(0),
        sync_socket_(::socket(AF_INET, SOCK_DGRAM, 0)),
        sync_addr_(Sockaddr::null<sockaddr_in>()),
        last_sync_(0), last_qtime_(0), last_qpacks_(0),
        qmin_(qmin),
        qmax_(qmax),
        qup_(qup),
        qdown_(qdown),
        force_queue_(force_queue),
        gt_(0),nextgt_(qmin_),tmin_(ULONG_MAX),tmax_(0),nanos_(0),
        dump_(tracefile ? new DumpGzip(tracefile) : 0),
        nmutex_(), seqno_(0), sync_started_(false), verbose_(v)
{
    if (sync_socket_ < 0) 
        die("Cannot create sync socket");
    sync_addr_.sin_family=AF_INET;
    sync_addr_.sin_addr.s_addr=inet_addr(sync_ip);
    sync_addr_.sin_port=htons(sync_port);

    // Set multicast scope (through TTL) for multicasting routers
    // [ 0: host, 1: subnet, 32: site 64: region, 128: continent, 255 unrestricted ]
    unsigned char ttl = mcast_ttl; 
    if (setsockopt(sync_socket_,IPPROTO_IP,IP_MULTICAST_TTL,&ttl,sizeof(ttl)) < 0)
        die("Error setting multicast scope");

    // Set multicast loopback to make sure simulations on the same host can receive
    unsigned char loop = 1; 
    if (setsockopt(sync_socket_,IPPROTO_IP,IP_MULTICAST_LOOP,&loop,sizeof(loop)) < 0)
        die("Error setting multicast loopback");
    quantum_ = qmax_;
    
    beat_interval_ = Option::get<uint64_t>("heartbeat_interval");
    throttle_usec_ = Option::get<int>("throttle");
    beat_gt_ = 0;
    HeartBeat::add(*this);
    add("nanos",nanos_);
    add("mediator.",Stats::get());
    
    clear_metrics();
}

Switch::~Switch()
{
    if (sync_socket_ >= 0)
        ::close(sync_socket_);
    if (dump_)
        delete dump_;
}

void Switch::die(const string& s)
{
   int e = errno;
   cerr << "ERROR: " << s;
   if (e) cerr << ": " << strerror(e);
   cerr << kill;
}

inline Node::Ptr Switch::find_node(const MacAddress& mac) 
{
    NodesMacMap::iterator iter = mac_map_.find(mac);
    return (iter != mac_map_.end()) ? iter->second : Node::Ptr();
}

inline Node::Ptr Switch::find_node(uint32_t id) 
{
    return find_node(MacAddress(mac_base_,id));
}

inline Node::Ptr Switch::find_node(const sockaddr_in &address)
{
    if (!Sockaddr::valid(address))
        return Node::Ptr();
    for (NodeSet::iterator i = nodes_.begin(); i != nodes_.end(); ++i) {
        Node::Ptr node = *i;
        if (Sockaddr::equal(node->data_addr(),address))
            return node;
    }
    return Node::Ptr();
}

void Switch::erase_dup_nodes(const sockaddr_in &address)
{
    if (!Sockaddr::valid(address))
        return;
    for (NodeSet::iterator i = nodes_.begin(); i != nodes_.end();) {
        NodeSet::iterator cur = i++;
        Node::Ptr node = *cur;
        if (!node->mac() && !node->id() && Sockaddr::equal(node->data_addr(),address)) {
            node.reset();
            nodes_.erase(cur);
        }
    }
}

Node::Ptr Switch::register_node(const MacAddress& mac, const sockaddr_in &address)
{
    boost::mutex::scoped_lock lk(nmutex_); // lock the nodes
    if (!mac) {
        // register a node with a new port and no mac
        const Node::Ptr newnode(new Node(address,mac,0));
        nodes_.insert(newnode);
        LOG2("(SWITCH) register new port",Sockaddr::str(address));
        dump_nodes();
        return newnode;
    } 
    if (mac.is_broadcast()) {
        LOG2("(SWITCH) Warning: cannot register broadcast MAC");
        return Node::Ptr();
    }
    Node::Ptr node = find_node(mac);
    if (!node) {
        // mac not found
        Node::Ptr oldnode = find_node(address);
        if (oldnode) {
            LOG2("(SWITCH) New mac",mac.str(), "for old port",Sockaddr::str(address));
            // Update the old node (port only) with the new mac
            oldnode->mac(mac);
            mac_map_[mac] = oldnode;
            dump_nodes();
            return oldnode;
        }
        else {
            // Register a new node
            LOG2("(SWITCH) New mac",mac.str(), "for new port",Sockaddr::str(address));
            const Node::Ptr newnode(new Node(address,mac,0));
            nodes_.insert(newnode);
            mac_map_[mac] = newnode;
            dump_nodes();
            return newnode;
        }
    }

    const sockaddr_in& oldaddr = node->data_addr();
    if (Sockaddr::equal(oldaddr,address)) {
        // node with same mac and same address
        LOG2("(SWITCH) Found mac",mac.str(),
              "port",Sockaddr::str(address),
              "nodeid",node->id());
        return node;
    }

    if (!Sockaddr::valid(oldaddr)) {
        // node with mac and no port set by a timestamp message
        node->data_addr(address);
        erase_dup_nodes(address);
        LOG2("(SWITCH) Set port : ",*node);
        dump_nodes();
        return node;
    }

    // someone else already registered with the same MAC
    cout << "(SWITCH) WARNING: replacing port from " << (*node);
    node->id(0);
    node->data_addr(address);
    cout << " to " << (*node) << endl;
    dump_nodes();
    return node;
}

Node::Ptr Switch::register_node(const sockaddr_in &address)
{
    return register_node(MacAddress(),address);
}

Node::Ptr Switch::register_node(uint32_t id,const sockaddr_in &address)
{
    boost::mutex::scoped_lock lk(nmutex_); // lock the nodes
    MacAddress mac(mac_base_,id);

    // called when we see a timestamp message
    Node::Ptr node = find_node(id);
    if (!node)
        node = find_node(address);
    if (node) {
        // node already registered, just add id/mac if needed
        bool change=false;
        if (!node->id()) {
            node->id(id);
            change=true;
            LOG2("(SWITCH) Update nodeid :",*node);
        }
        if (!node->mac()) {
            node->mac(mac);
            mac_map_[mac] = node;
            change=true;
            LOG2("(SWITCH) Update mac :",*node);
        }
        if(change)
            dump_nodes();
        return node;
    }
    // node not yet registered, add it with constructed mac
    // NOTE: we can't use the address, it's possible that
    // a node could use two separate ports for data and ctrl
    const Node::Ptr newnode(new Node(mac,id)); 
    nodes_.insert(newnode);
    mac_map_[mac] = newnode;
    LOG2("(SWITCH) New node:",*newnode);
    dump_nodes();
    return newnode;
}

void Switch::really_send(const Node::Ptr to_node, const Packet& packet)
{
    if (!to_node || !to_node->data_valid())
        return;
    const sockaddr_in& to_addr = to_node->data_addr();
    size_t l = packet.len();
    LOG2("(SWITCH) Send msg: len [", l ,"] to node ", 
          to_node->id(), "addr ", Sockaddr::str(to_addr));
    throttle(l);
    processor_->send_data_packet(to_addr,packet,"DATA fwd");
}

void Switch::send(const Node::Ptr to_node, const Packet& packet, uint64_t now)
{
    // If delay < quantum, send right away (unless we force queueing),
    // else, enqueue the packet
    if (!to_node)
        return;

    int64_t delay = timing_ ? timing_->get_delay() : 0;
    if (!sync_started_ || (!force_queue_ && delay <= lround(quantum_)))
        really_send(to_node, packet);
    else {
        LOG2("(SWITCH) Schedule msg: len [", packet.len(),"] to node", to_node->id(),
            "time", now , "+", delay);

        uint64_t dest_simtime = now + delay;
        scheduler_->schedule(packet, to_node, dest_simtime);
    }
}

bool Switch::send(const MacAddress& dst, const Packet& packet, uint64_t now)
{
    Node::Ptr to_node = find_node(dst);
    if (to_node) {
        send(to_node,packet,now);
        return true;
    }
    return false;
}

int Switch::broadcast(const MacAddress& src, const Packet& packet,uint64_t now)
{
    LOG2("(SWITCH) broadcast", src.str());
    // send packets to all nodes
    for (NodeSet::iterator i = nodes_.begin(); i != nodes_.end(); ++i) {
        Node::Ptr to_node = *i;
        if (to_node) {
            if (timing_)
                timing_->packet_event_mt(src,to_node->mac(),packet.len(),true);
            send(to_node,packet,now);
        }
    }
    return 0;
}

bool Switch::stop_node(Node& n)
{
    bool rv = true;
    if (!n.data_valid()) {
        n.warning(); // Node with this mac address was not found
        rv = false;
        send_sync(true); // Force sync to avoid initial deadlock
    }
    // Mark the node as stopped
    LOG2("(SYNC) Stop node [", n.mac().str(), "]");
    if (n.sync_valid()) {
        sync_cluster(n.getSimtime());
        n.stop();
    }
    return rv;
}

bool Switch::heartbeat_node(Node& n) 
{
    bool rv = true;
    if (!n.data_valid()) {
        n.warning(); // Node with this mac address was not found
        rv = false;
    }
    LOG2("(SYNC) Hartbeat node [", n.mac().str(), "]");
    if (n.sync_valid()) {
        n.start();
        sync_cluster(n.getSimtime());
    }
    return rv;
}

void Switch::start_nodes(const uint64_t quantum)
{
    if (timing_)
        timing_->startquantum_mt(GT(), nextGT());

    // Start nodes, delete obsolete ones
    bool deleted = false;
    numnodes_ = 0;
    NodeSet::iterator i = nodes_.begin(); 
    while (i != nodes_.end()) {
        NodeSet::iterator cur = i++;
        Node::Ptr node = *cur;
        if (node && node->obsolete(300)) { // 5'
            deleted = true;
            LOG2("(SWITCH) Delete obsolete",*node);
            node.reset();
            nodes_.erase(cur);
        }
        else if (node && node->sync_valid()) {
            node->start();
            numnodes_++;
        }
    }
    if (deleted)
        dump_nodes();
}

int Switch::process_packet(
    const Packet& packet,
    const sockaddr_in &address,
    const MacAddress& src,
    const MacAddress& dst,
    bool from_slirp,
    bool from_vde)
{
    uint64_t now = GT();
    uint32_t len = packet.len();
    uint32_t lat = 0;
    Stats::get().n_bytes_ += len;
    bool bcast = dst.is_broadcast();

    LOG2("(SWITCH) process_packet: src",src.str(), "dst",dst.str(), "len",len);

    // FIXME: Do not register MAC of empty packets to filter out simnow keepalives
    bool registered = from_vde || from_slirp || ((bcast||len>14) && register_node(src,address));

    if (bcast) {
        broadcast(src,packet, now);
        if (vde_on_ && ! from_vde)
            processor_->send_vde_packet(packet, "bcast to VDE");
        if (slirp_on_ && ! from_slirp)
            processor_->send_slirp_packet(packet, "bcast to SLIRP");
        Stats::get().n_bcast_++;
    }
    else {
        if (timing_) {
            timing_->packet_event_mt(src,dst,len,false);
            lat = timing_->register_lat();
        }
        if (registered && send(dst, packet, now)) {
            Stats::get().n_fwd_++;
        }
        else {
            if (vde_on_ && ! from_vde) {
                processor_->send_vde_packet(packet, "VDE fwd");
                Stats::get().n_vde_++;
            }
            if (slirp_on_ && ! from_slirp) {
                processor_->send_slirp_packet(packet, "SLIRP fwd");
                Stats::get().n_slirp_++;
            }
        }
    }

    // Update stats
    Stats::get().n_packs_++;
    Stats::get().print_stats(1000);

    // Dump trace
    if (dump_)
       (*dump_) << now << src << dst << len << quantum_ << lat;
    return 0;
}

bool Switch::simtime_advance(uint64_t ntime)
{
    // Compute min/max times of active nodes
    uint64_t t0 = ntime ? ntime : ULONG_MAX;
    uint64_t t1 = ntime ? ntime : 0;
    int nw = 0;
    // int nx = 0;
    for (NodeSet::iterator i = nodes_.begin(); i != nodes_.end(); ++i) {
        const Node::Ptr node = *i;
        if (node && node->sync_valid()) {
            uint64_t t = node->getSimtime();
            t0 = t < t0 ? t : t0;
            t1 = t > t1 ? t : t1;
            if (t < nextGT()) {
               nw++;
               // nx = node->id();
            }
        }
    }
    tmin_ = t0;
    tmax_ = t1;
    if (gt_ == 0 && !sync_started_) {
        LOG1("(SWITCH) Start synchronization [",t0,t1,"]");
        sync_started_ = true;
    }
    LOG3("GT=",GT(),"next=",nextGT(),"nt=",ntime,"t0=",t0,"t1=",t1);

    // Time advances when the slowest node has reached the barrier 
    bool advance = t0 >= nextGT();
    #if 0
    if (advance)
        cout << "++++++ t0=" << t0 << " t1="  << t1 << " nextgt=" << nextGT() << endl;
    else {
        if (nw==1)
            cout << "= t0=" << t0 << " t1="  << t1 << " nextgt=" << nextGT() 
                 << " nw=" << nw << " (" << nx << ")"
                 << endl;
    }
    #endif
    return advance;
}

inline void Switch::GT_advance()
{
    if (gt_ >= beat_gt_) {
        boost::mutex::scoped_lock lk(nmutex_); // lock the nodes
        HeartBeat::beat();
        beat_gt_ = gt_ + beat_interval_;
    }
    nanos_ = (gt_ = nextgt_) * 1000; // gt is in usec
    nextgt_ += lround(quantum_);
    Stats::get().update_time(gt_);
}

uint64_t Switch::compute_quantum(uint64_t nsent, uint64_t pqmax)
{
    uint64_t qtime = GT();
    uint64_t qpacks = Stats::get().n_packs_;
    uint64_t duration = qtime - last_qtime_;
    Stats::get().n_q_++;
    if (nsent) {
        // After we send packets, we do a min quantum to ensure 
        // we resync after all packets have been received
        // Still not optimal, best would be a 2-phase approach
        quantum_ = qmin_; 
        last_qtime_ = qtime;
        last_qpacks_ = qpacks;
        return duration;
    }
    if (quantum_from_file()) {
        last_qtime_ = qtime;
        last_qpacks_ = qpacks;
        return duration;
    }
    if (qmin_ == qmax_) {
        quantum_ = qmax_;
        last_qtime_ = qtime;
        last_qpacks_ = qpacks;
        return duration;
    }

    // max between the max quantum and the time of the first pending packet
    double qmax = pqmax ? static_cast<double>(pqmax) : qmax_;
    double packs = static_cast<double>(qpacks - last_qpacks_);
    double q = static_cast<double>(duration)/packs; // +inf is ok
    // Compute next quantum
    double nq = quantum_;
    if (q < quantum_ || (q==quantum_ && packs))
        nq = nq * qdown_;
    else if (q > quantum_)
        nq = nq * qup_;
    
    // Clip for sanity
    if (nq < qmin_)
        nq = qmin_;
    else if (nq > qmax)
        nq = qmax;

    if ((packs || lround(nq) != lround(quantum_)))
        LOG2("Time=",qtime,"Packs=",lround(packs), 
             "Duration=",duration,"Quantum=",lround(nq));

    quantum_ = nq;
    last_qtime_ = qtime;
    last_qpacks_ = qpacks;
    return duration;
}

void Switch::sync_cluster(uint64_t ntime)
{
    // Check for forward progress
    if (simtime_advance(ntime)) {
        // Send pending packets
        pair<uint64_t,uint64_t> pp = scheduler_->send_packets(nextGT());

        // Compute next quantum 
        uint64_t pqmax = pp.second ? pp.second - GT() : 0;
        uint64_t duration = compute_quantum(pp.first,pqmax);

        // Update the timing model
        start_nodes(duration);

        // Update next sync point
        GT_advance();

        // Send a sync to all nodes to restart them
        send_sync(false);
    }
}

inline void Switch::send_sync(bool force)
{
    uint64_t gt = GT();
    if (!force && last_sync_ && gt <= last_sync_)
        return;
    GlobalTime tmsg(nextGT(),0,++seqno_);
    if (tmsg.sendto(sync_socket_,&sync_addr_) == tmsg.len()) {
        boost::mutex::scoped_lock lk(nmutex_); // lock the nodes
        Stats::get().n_mcsync_++;
        last_sync_ = gt;
        if (gt > 0)
            LOG2("(SYNC) GLOBAL TIME", gt, nextgt_, tmin_, tmax_, quantum_);
    }
    else
        cout << "(SYNC) WARNING: Cannot send sync message: " << strerror(errno) << endl;
}

void Switch::timeout()
{
    if (GT() > 0)
        sync_cluster(GT());
    LOG2("(SYNC) TIMEOUT", GT(), nextgt_, tmin_, tmax_, quantum_);
    send_sync(true); // Force a sync
}

void Switch::send_terminate()
{
    HeartBeat::last_beat();
    
    GlobalTime tmsg;
    tmsg.mkterminate();
    if (tmsg.sendto(sync_socket_,&sync_addr_) != tmsg.len())
        cout << "(SYNC) WARNING: Cannot send terminate message: " << strerror(errno) << endl;
    if (dump_) 
       { dump_->flush(); delete dump_; }
}

void Switch::send_cpuid(uint64_t x,uint16_t y,uint16_t z)
{
    GlobalTime tmsg;
    tmsg.mkcpuid(x,y,z);
    if (tmsg.sendto(sync_socket_,&sync_addr_) != tmsg.len())
        cout << "(SYNC) WARNING: Cannot send cpuid message: " << strerror(errno) << endl;
    else
        cout << "(SYNC) sent cpuid message: " << y << " " << z << " " << x << endl;
}

void Switch::dump_nodes()
{
    if (verbose_ < 1)
        return;
    extern boost::mutex io_mutex;
    boost::mutex::scoped_lock lk(io_mutex);
    int n = 1;
    cout << "################### NODES ######################" << endl;
    for (NodeSet::const_iterator i = nodes_.begin(); i != nodes_.end(); ++i) {
        const Node::Ptr node = (*i);
        if (node)
            cout << n++ << "] " << (*node) << endl;
    }
    cout << "------------------------------------------------" << endl;
}

void Switch::throttle(uint64_t b)
{
    if (throttle_usec_ <=0)
        return;
    static uint64_t t0 = 0;
    int dt = Stats::get().elapsed()-t0;
    timespec ts = {0,0};
    if (dt < throttle_usec_) {
        uint64_t tt = throttle_usec_ - dt;
        LOG1("Transmit pacing ",tt);
        ts.tv_nsec = 1000 * tt;
        ::nanosleep(&ts,0);
    }
    t0 = Stats::get().elapsed();
}

#include <fstream>
inline bool Switch::quantum_from_file()
{
    return false; // Not yet ready for prime time

    ifstream qf("quantum.conf");
    if (!qf.is_open())
        return false;
    double q = 0.0;
    qf >> q;
    qf.close();
    if (q > 0.0) {
        if (q != quantum_)
            cout << "Quantum from file = " << q 
                 << " at simtime " << GT()
                 << endl;
        quantum_ = q;
        return true;
    }
    return false;
}

/* vi: set sw=4 ts=4: */
