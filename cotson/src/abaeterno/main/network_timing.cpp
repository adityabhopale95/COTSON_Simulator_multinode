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

// $Id: network_timing.cpp 721 2015-06-03 02:29:33Z paolofrb $

// #define _DEBUG_THIS_

#include "network_timing.h"
#include "cotson.h"
#include "logger.h"
#include "liboptions.h"
#include "callme.h"
#include "error.h"
#include "callbacks.h"
#include "abaeterno.h"

#include <errno.h>
#include <netdb.h>

// All time computations are in us unless otherwise noted

using namespace std;
using namespace boost;

namespace { 

option o1("network.latency",  "0", "network latency in us");
option o2("network.mediator_latency", "0", "mediator latency in us");
option o3("network.host", "localhost", "mediator host");
option o4("network.control_port", "8196", "mediator control port");
option o5("network.mediator_nodeid", "1", "cluster node id (for the mediator)");
option o6("network.quantum_max", "mediator sync interval in us");
option o7("network.multicast_ip", "multicast address for cluster messages");
option o8("network.multicast_port", "multicast port for cluster messages");
option o9("network.timeout", "safety timeout for global time messages ");
option o10("network.noadvance", "no-advance interval threshold");
option o11("network.log", "log interval for networking stats");
option o12("network.asktime", "ask time when out of sync");
option o13("network.sync_start", "when to start node sync");

scoped_ptr<NetworkTiming> unique_network_timing;

void network_timing_init() {
    if(Option::has("network.quantum_max")) 
        unique_network_timing.reset(new NetworkTiming()); 
}

void network_timing_terminate() { 
    if(unique_network_timing) 
        unique_network_timing->terminate();
    unique_network_timing.reset(0);
}

void network_timing_start() {
    if(unique_network_timing) 
        unique_network_timing->start();
}

void network_timing_stop() {
    if(unique_network_timing) 
        unique_network_timing->stop();
}

void network_timing_prepare() {
    if(unique_network_timing) 
        unique_network_timing->init(
            Cotson::nanos(),
            Option::get<uint64_t>("network.sync_start",0) 
        );
}

run_at_init      f1(&network_timing_init);
run_at_terminate f2(&network_timing_terminate);
run_at_start     f3(&network_timing_start);
run_at_stop      f4(&network_timing_stop);
run_at_prepare   f5(&network_timing_prepare);
}

NetworkTiming* NetworkTiming::get() { 
    return unique_network_timing.get();
}


void NetworkTiming::Delay(uint64_t val) {
    if(unique_network_timing && val != 0)
        unique_network_timing->net_delay(val); 
}

bool NetworkTiming::Terminated() {
    if(unique_network_timing)
        return unique_network_timing->terminated();
    return false;
}


NetworkTiming::NetworkTiming() :
    log_(Option::get<uint>("network.log",0)),
    net_delay_(0), last_ts_(0),
    net_latency_(Option::get<int>("network.latency")), // us
    med_latency_(Option::get<int>("network.mediator_latency")), // us
    med_host_(Option::get<string>("network.host")),
    med_port_(Option::get<uint>("network.control_port")),
    sync_port_(Option::get<uint>("network.multicast_port",0)),
    med_nodeid_(Option::get<uint32_t>("network.mediator_nodeid")),
    med_quantum_(Option::get<uint>("network.quantum_max", 0)),
    med_sock_(-1), sync_sock_(-1), med_ok_(false), t0_(0), 
    sync_delay_(0), sync_no_(0), nsync_in_(0), nsync_out_(0),
    gtime_(), seqno_(0)
{
    if (med_quantum_ == 0)
        return;

    // open UDP control socket to the mediator
    med_sock_ = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (med_sock_ == -1) {
        warn("Cannot create mediator socket");
        return;
    }
    const hostent *mh = ::gethostbyname(med_host_.c_str());
    if (!mh) {
        warn("Cannot resolve mediator host "+string(med_host_));
        ::close(med_sock_);
        med_sock_ = -1;
        return;
    }
    ::memset(&med_addr_,0,sizeof(sockaddr_in));
    ::memcpy((char *)&med_addr_.sin_addr.s_addr,mh->h_addr_list[0],mh->h_length);
    med_addr_.sin_family = mh->h_addrtype;
    med_addr_.sin_port = htons(med_port_);
    
    // Open UDP sync socket for timestamps from mediator
    sync_sock_ = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (sync_sock_ == -1) {
        warn("Cannot create sync socket");
        return;
    }
    // Let port be reused by others
    int yes = 1;
    if (::setsockopt(sync_sock_,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
        warn("Cannot reuse port addresses");
        ::close(sync_sock_);
        sync_sock_ = -1;
        return;
    }
    // Create and bind to multicast address for timestamps
    sockaddr_in mcaddr;
    ::memset(&mcaddr,0,sizeof(sockaddr_in));
    mcaddr.sin_family=AF_INET;
    mcaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    mcaddr.sin_port=htons(sync_port_);
    if (::bind(sync_sock_,(sockaddr *) &mcaddr,sizeof(mcaddr)) < 0) {
        warn("Cannot bind to the multicast address");
        ::close(sync_sock_);
        sync_sock_ = -1;
        return;
    }
    // Setup multicast IP membership
    ip_mreq mreq;
    mreq.imr_multiaddr.s_addr=inet_addr(
        Option::get<string>("network.multicast_ip").c_str());
    mreq.imr_interface.s_addr=htonl(INADDR_ANY);
    if (::setsockopt(sync_sock_,IPPROTO_IP,IP_ADD_MEMBERSHIP,
                     &mreq,sizeof(mreq)) < 0
    ) {
        warn("Cannot add multicast membership");
        ::close(sync_sock_);
        sync_sock_ = -1;
        return;
    }
    add("sync_delay",sync_delay_);
    add("sync_number",sync_no_);
    add("sync_recd",nsync_in_);
    add("sync_sent",nsync_out_);
}

void NetworkTiming::init(uint64_t nanos, uint64_t start)
{
    if (med_sock_ < 0 || sync_sock_ < 0 || med_quantum_ == 0)
        return;
    if (!med_ok_) {
        // Start thread to receive med messages from the multicast
        t0_ = nanos;
        med_ok_ = true;
        last_ts_ = 0;
        if (start) gtime_.init(nanos+start);
        LOG("Starting global time thread at", t0_);
        boost::thread run(boost::bind(&NetworkTiming::global_time, this));
    
        // Start the cluster synchronization
        next();
    }
    else
        WARNING("NetworkTiming::init() called twice!");
}

inline uint64_t NetworkTiming::simu_time()
{
    return (Cotson::nanos() - t0_)/1000;
}

NetworkTiming::~NetworkTiming()
{
    if (med_sock_ >= 0 && ::close(med_sock_) != 0)
        warn("Unable to close mediator socket");
    if (sync_sock_ >= 0 && ::close(sync_sock_) != 0)
        warn("Unable to close sync socket");
}

void NetworkTiming::usleep(int64_t t) const
{
    if (t < 10) // do not bother
        return;
    uint64_t s = t / 1000000ULL;
    uint64_t ns = (t - s*1000000ULL) * 1000;
    timespec ts = {(__time_t)s,(long int)ns}; /* explict cast: library struct requires these types*/
    timespec tr = {0,0};
    LOG ("Sleep started",t,"us");
    while (::nanosleep(&ts,&tr) == -1 && errno == EINTR) {
        LOG("Sleep interrupted: remaining", tr.tv_sec, tr.tv_nsec);
        ts.tv_sec = tr.tv_sec;
        ts.tv_nsec = tr.tv_nsec;
    }
}

void NetworkTiming::sync(uint64_t nanos)
{
    if (gtime_.terminated())
        return;

    sync_no_++;
    uint64_t now = simu_time();
    heartbeat(now);
    uint64_t gt = gtime_.gt();
    if (now >= gt) {
        sync_delay_ += (now-gt);
        LOG("Ahead by",now-gt,"- GT",gt); 

        stop(); // Notify the mediator
        gt = gtime_.wait_until(now); // Wait for others
        start(); // Tell the mediator we're up and running
        LOG("Restart - GT",gt,"now",now);
        net_delay(gtime_.lat());

        if (log_ && (gt % log_)==0) {
            cout << ">>> Time " << gt
                 << " sync_recd " << nsync_in_
                 << " sync_sent " << nsync_out_
                 << " sync_delay " << sync_delay_
                << endl;
        }
    }
    else
        LOG("Behind by",gt-now,"- GT",gt);
    next(); // Schedule a sync() call to next_gt
}

inline void NetworkTiming::next()
{
    uint64_t q = med_quantum_; // default
    uint64_t now = simu_time();
    uint64_t ngt = gtime_.gt();
    if (ngt && ngt > now) {
        q = ngt - now;
    }
    // cout << "call me later now=" << now << " next=" << ngt << " q=" << q << endl;
    CallMe::later(q*1000,boost::bind(&NetworkTiming::sync,this,_1));
}

inline void NetworkTiming::send_msg(const TimeStamp& msg)
{
    if (med_sock_ < 0)
        return;
    last_ts_ = msg.tstamp();
    msg.sendto(med_sock_,&med_addr_);
    nsync_out_++;
}

void NetworkTiming::GT::check_med_seq(const GlobalTime &gt)
{
    // check for out of sequence (missed sync packets)
    uint16_t next_med_seqno = med_seqno_ + 1;
    if (med_seqno_ && (gt.seqno() != next_med_seqno)) {
        WARNING("Warning: missed sync packet",
            " simtime " , gt.gt(),
            " msgtime " , gt.gt(),
            " prevseq " , med_seqno_,
            " seq " , gt.seqno());
        // now what?
    }
    med_seqno_ = gt.seqno();
}

void NetworkTiming::global_time()
{
    // loop forever, listening to the multicast socket
    double dtimeout = Option::get<double>("network.timeout",2.0);
    int noadvance = Option::get<uint>("network.noadvance",100);
    bool askfortime = Option::get<bool>("network.asktime",false);
    uint32_t s  = static_cast<uint32_t>(dtimeout);
    uint32_t ns = static_cast<uint32_t>((dtimeout-static_cast<double>(s))*1e9);
    int nstalled = 0;
    fd_set rfd;
    FD_ZERO(&rfd);
    FD_SET(sync_sock_,&rfd);
    while (true) {
        timeval timeout = {s,ns};
        fd_set r = rfd;
        int nfd = ::select(sync_sock_+1,&r,0,0,&timeout);
        bool advance = false;
        if (nfd && FD_ISSET(sync_sock_,&r)) {
            GlobalTime gt(sync_sock_);
            if (gt.valid()) {
                LOG("GOT GT: ",gt.gt(),gt.lat(),gt.seqno());
                nsync_in_++;
                advance = gtime_.process(gt);
            }
            else
                LOG("GOT INVALID GT: ",gt.gt(),gt.lat(),gt.seqno());
        }
        else // no sync packets before timeout
            asktime("Sync timeout",askfortime);
        
        // check for stall condition
        nstalled = advance ? 0 : nstalled + 1;
        if (nstalled == noadvance) {
            asktime("No progress",askfortime);
            nstalled = 0;
        }
    }
}

inline void NetworkTiming::heartbeat(uint64_t now)
{
    uint64_t gt = gtime_.gt();
    if (now <= last_ts_ || now < gt) // filter redundant heartbeats
        return;

    // Send a timestamp packet to the mediator
    LOG("heartbeat at",now,"cluster time",gt);
    send_msg(TimeStamp(TimingMessage::TimeStampMsg,now,med_nodeid_,++seqno_));
}

void NetworkTiming::send_packet_delay()
{
    if (med_latency_ && net_latency_)
        usleep(2*net_latency_ + med_latency_);
}

void NetworkTiming::start()
{
    // FIXME: remove this, since the mediator currently ignores starts/stop
    // uint64_t now = simu_time();
    // send_msg(TimeStamp(TimingMessage::NodeStartMsg,now,med_nodeid_,++seqno_));
}

void NetworkTiming::stop()
{
    // FIXME: remove this, since the mediator currently ignores starts/stop
    // uint64_t now = simu_time();
    // send_msg(TimeStamp(TimingMessage::NodeStopMsg,now,med_nodeid_,++seqno_));
}

void NetworkTiming::terminate()
{
    uint64_t now = simu_time();
    cout << "Sending terminate message at " << now << endl;
    send_msg(TimeStamp(TimingMessage::TerminateMsg,now,med_nodeid_,++seqno_));
    GT().terminate();
}

void NetworkTiming::asktime(const char *s, bool message)
{
    uint64_t now = simu_time();
    cout << "Warning: " << s << " now=" << now << " gt=" << gtime_.gt() << endl;
    if (message)
        send_msg(TimeStamp(TimingMessage::TimeQueryMsg,now,med_nodeid_,++seqno_));
}

void NetworkTiming::cpuid(uint64_t x, uint16_t y, uint16_t z)
{
    uint64_t now = simu_time();
    cout << "Sending cpuid message at " << now << ": " << y << " " << z << " " << x << endl;
    send_msg(TimeStamp(TimingMessage::CpuidMsg,x,y,z));
}

void NetworkTiming::warn(const string& s)
{
    int e = errno; (void)e; // avoid compiler warning 
    WARNING ("[NetworkTiming] ", s);
    ERROR_IF (e, strerror(e));
}

//=======  Global Time synchronization ========

NetworkTiming::GT::GT():clust_gt(0),clust_lat(0),gtmutex(),end(false),med_seqno_(0) { }

inline uint64_t NetworkTiming::GT::gt()
{
    boost::mutex::scoped_lock lk(gtmutex);
    return clust_gt;
}

inline uint64_t NetworkTiming::GT::lat()
{
    boost::mutex::scoped_lock lk(gtmutex);
    return clust_lat;
}

inline uint64_t NetworkTiming::GT::wait_until(uint64_t now)
{
    boost::mutex::scoped_lock lk(gtmutex);
    while (clust_gt <= now && !end) {
        // cout << "wait until now=" << now << " gt=" << clust_gt << endl;
        new_gt.wait(lk);
    }
    return clust_gt;
}

inline void NetworkTiming::GT::terminate()
{
    end = true;
    new_gt.notify_one();
}

bool NetworkTiming::GT::process(const GlobalTime& gt)
{
    boost::mutex::scoped_lock lk(gtmutex);
    if (gt.is_terminate()) {
        cerr << "Received termination message" << endl;
        terminate();
        return true; // never gets here
    }
    if (gt.is_cpuid()) {
        uint64_t x = gt.gt();
        uint16_t y = gt.lat();
        uint16_t z = gt.seqno();
        cerr << "Received cpuid message: " << y << " " << z << " " << x << endl;
        AbAeterno::get().network_cpuid(x,y,z);
        return false; // no advance
    }
    check_med_seq(gt);
    if (gt.gt() > clust_gt) { // filter out OoO messages
        clust_gt = gt.gt();
        clust_lat = gt.lat();
        new_gt.notify_one();
        return true; // advance
    }
    return false; // no advance
}

void NetworkTiming::GT::init(uint64_t t)
{
    boost::mutex::scoped_lock lk(gtmutex);
    clust_gt = t;
    clust_lat = 0;
    new_gt.notify_one();
}

