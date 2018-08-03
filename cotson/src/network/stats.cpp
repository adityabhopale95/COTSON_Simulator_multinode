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

// $Id: stats.cpp 121 2010-09-20 15:01:30Z paolofrb $

#include "stats.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <stdint.h>

using namespace std;

Stats::Stats() 
{
    reset(); 
    add("packets", n_packs_);
	add("bytes", n_bytes_);
    add("broadcast",n_bcast_);
    add("forward",n_fwd_);
    add("vde",n_vde_);
    add("slirp",n_slirp_);
    add("nsync",n_mcsync_);
    add("control",n_msg_);
}

Stats::~Stats() {}

void Stats::reset()
{
    n_packs_ = n_bcast_ = n_control_ = n_vde_ = n_fwd_ = n_bytes_ = 0;
	n_queued_packs_ = last_queued_packs_ = n_q_ = 0;
	n_mcsync_ = last_mcsync_ = 0;
	simtime_ = simtime0_ = 0;
    realtime_ = realtime0_ = now();
	last_rtime_ = 0;
	last_stime_ = 0;
}

uint64_t Stats::now()
{
    struct timeval tv;
    ::gettimeofday(&tv, 0);
    return (static_cast<uint64_t>(tv.tv_sec) * 1000000ULL) + tv.tv_usec;
}

bool Stats::print_stats (uint32_t interval)
{
    if ((n_packs_ % interval) == 0)  {
	    uint64_t tnow = now();
		uint64_t qp = n_queued_packs_ - last_queued_packs_;
		uint64_t st = simtime_ - last_stime_;
		double rt = tnow - last_rtime_;
		uint64_t ns = n_mcsync_ - last_mcsync_;
		uint64_t nm = n_msg_ - last_msg_;
		uint64_t nq = n_q_ - last_q_;
		const char *s = "";
		if (interval == 1) {
		    s = "FINAL: ";
		    qp = n_queued_packs_;
		    ns = n_mcsync_;
		    nm = n_msg_;
		    st = simtime_ - simtime0_;
		    rt = tnow - realtime0_;
			nq = n_q_;
        }
		double slowdown = st > 0 ? static_cast<double>(rt)/static_cast<double>(st) : 0.0;
		double qq = nq ? static_cast<double>(st*10/nq)/10.0 : 0.0;
	    cout  << s
			  << simtime_ << "] "
		      << " Packs=" << n_packs_
              << " Bcast=" << n_bcast_
              << " Fwd=" << n_fwd_
              << " Out=" << n_vde_
              << " Bytes=" << n_bytes_
              << " QP=" << qp
              << " Sy=" << ns
              << " Msg=" << nm
              << " ST=" << st
              << " RT=" << llround(rt*1e-3)
			  << " Q=" << qq
              << " SD=" << llround(slowdown)
		      << endl
			  << flush;

		last_queued_packs_ = n_queued_packs_;
		last_mcsync_ = n_mcsync_;
		last_msg_ = n_msg_;
	    last_stime_ = simtime_;
	    last_rtime_ = tnow;
		last_q_ = n_q_;
	    return true;
    }
	return false;
}

uint64_t Stats::sync_packs()
{
    uint64_t n = n_packs_ - last_packs_;
    last_packs_ = n_packs_;
	return n;
}

void Stats::update_time(uint64_t st)
{
	uint64_t rt = now();
    if (simtime0_==0) {
	   simtime0_ = last_stime_ = st;
       last_rtime_ = rt;
	   cout << "Stats sim reset: simtime " << simtime0_ 
	        << " realtime " << rt - realtime0_ << endl;
	}
    simtime_ = st;
    realtime_ = rt;
}

bool Stats::timeout(uint64_t us) const
{
    return (now() > realtime_ + us);
}

