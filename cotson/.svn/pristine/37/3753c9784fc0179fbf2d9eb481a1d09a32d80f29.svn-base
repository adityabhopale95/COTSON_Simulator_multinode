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
#ifndef NETWORK_TIMING_H
#define NETWORK_TIMING_H

#include "abaeterno_config.h"
#include "timing_message.h"
#include "libmetric.h"

#include <arpa/inet.h>
#include "boost/thread/mutex.hpp"
#include "boost/thread/condition.hpp"

class NetworkTiming : public metric
{
public:
    static NetworkTiming* get();
    static void Delay(uint64_t val);
    static void InitSync(uint64_t nanos);
    static bool Terminated();

    NetworkTiming();
    ~NetworkTiming();
    void init(uint64_t,uint64_t);
    void send_packet_delay();
    uint32_t net_delay() const { return net_delay_; }
    inline void net_delay(uint32_t d) { net_delay_ = d; }
    inline bool terminated() const { return gtime_.terminated(); }
    void cpuid(uint64_t,uint16_t,uint16_t);

    void start();
    void stop();
    void asktime(const char*, bool);
    void terminate();
private:

    class GT : boost::noncopyable
    {
    public:
        GT();
        inline uint64_t gt(); // next global time
        inline uint64_t lat(); // net latency
        inline uint64_t wait_until(uint64_t);
        inline bool terminated() const { return end; }
        inline void terminate();
        inline void init(uint64_t t);
        bool process(const GlobalTime&);
    private:
        uint64_t clust_gt;
        uint64_t clust_lat;
        boost::mutex gtmutex;
        bool end;
        boost::condition new_gt;
        uint16_t med_seqno_;
        void check_med_seq(const GlobalTime&);
    };

    void sync(uint64_t);
    void heartbeat();
    void heartbeat(uint64_t);
    void warn(const std::string&);
    void global_time(); // time receiver thread

    inline uint64_t simu_time();
    inline void send_msg(const TimeStamp&);
    inline void usleep(int64_t) const;
    inline void next();

    const uint32_t log_;
    uint64_t net_delay_;
    uint64_t last_ts_;

    // mediator info
    const int net_latency_;
    const int med_latency_;
    const std::string med_host_;
    const uint16_t med_port_;
    const uint16_t sync_port_;
    const uint32_t med_nodeid_;
    const uint64_t med_quantum_;
    int med_sock_;
    int sync_sock_;
    sockaddr_in med_addr_;
    bool med_ok_;
    uint64_t t0_;
    uint64_t sync_delay_;
    uint64_t sync_no_;
    uint64_t nsync_in_;
    uint64_t nsync_out_;
    GT gtime_;
    uint16_t seqno_;
};

#endif
