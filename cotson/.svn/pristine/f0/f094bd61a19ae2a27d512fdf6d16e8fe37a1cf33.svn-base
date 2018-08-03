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

#ifndef STATS_H_
#define STATS_H_

#include <iostream>
#include <stdint.h>
#include "metric.h"

// time in ms/us precision
class Stats : public metric
{
public:
    static Stats& get() { static Stats singleton; return singleton; }
    Stats();
    ~Stats();
    void reset(); // resets stats

    // Prints out the stats at a given interval (in packets)
    bool print_stats(uint32_t);

    uint64_t n_packs_;
    uint64_t n_bcast_;
    uint64_t n_control_;
    uint64_t n_slirp_;
    uint64_t n_vde_;
    uint64_t n_fwd_;
    uint64_t n_bytes_;
    uint64_t n_queued_packs_;
    uint64_t n_mcsync_;
    uint64_t n_msg_;
	uint32_t n_q_;

	void update_time(uint64_t);
    uint64_t sync_packs();
	uint64_t elapsed() const { return now() - realtime0_; }
	bool timeout(uint64_t) const;

private:
    static uint64_t now(); // Time utility (in us)

    // Last values (for dff stat prints)
	uint64_t last_queued_packs_;
	uint64_t last_packs_;
	uint64_t last_mcsync_;
	uint64_t last_msg_;

    // Last sync'ed cluster timestamp
    uint64_t simtime_;
    uint64_t realtime_;
	uint64_t last_q_;

    // Start times
    uint64_t realtime0_;
    uint64_t simtime0_;

    // Last times
    uint64_t last_rtime_;
    uint64_t last_stime_;
};

#endif /*STATS_H_*/
