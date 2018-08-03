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

// $Id: scheduler.cpp 71 2010-02-10 15:35:54Z paolofrb $

#include "scheduler.h"
#include "switch.h"
#include "stats.h"
#include "log.h"

using namespace std;

Scheduler::Scheduler(Switch* sw, int v) :
        messages_(),
        switch_(sw),
		qmutex_(),
		verbose_(v)
{}

Scheduler::~Scheduler()
{}

void Scheduler::start(int)
{}

void Scheduler::schedule(const Packet& packet, const Node::Ptr node, uint64_t st)
{
	uint64_t rt = Stats::get().elapsed();
    Stats::get().n_queued_packs_++;
    LOG2("(SWITCH) Scheduling a packet on the queue at simtime",st,"realtime",rt);
	boost::mutex::scoped_lock lk(qmutex_); // lock the queue
    messages_.insert(Message(packet,node,st,rt)); // push the message on the queue
}

pair<uint64_t,uint64_t> Scheduler::send_packets(uint64_t stime)
{
	boost::mutex::scoped_lock lk(qmutex_); // lock the queue
    uint32_t m0 = messages_.size();
    if (m0)
        LOG2("(SWITCH) Releasing scheduled messages at",stime);

	uint64_t np = 0;
	for (MsgQueue::iterator i = messages_.begin(); i != messages_.end();) {
		MsgQueue::iterator cur = i++; // safe increment for erase
	    const Message& m = (*cur);
		Node::Ptr node = m.node();
		// uint64_t ntime = node->sync_valid() ? node->getSimtime() : stime;
		if (stime >= m.simtime()) {
		    switch_->really_send(node,m.packet());
			messages_.erase(cur);
			np++;
		}
	}
    uint32_t m1 = messages_.size();
    if (m0 > 0 && m0 > m1) 
        LOG2("(SWITCH) Time=",stime,"Msgs before=",m0,"Msgs after=",m1);
	MsgQueue::iterator top = messages_.begin();
	uint64_t tt = (top == messages_.end()) ? 0 : top->simtime();
	return pair<uint64_t,uint64_t>(np,tt);
}

