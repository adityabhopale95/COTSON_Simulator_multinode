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


#include "control_packet_processor.h"
#include "switch.h"
#include "stats.h"
#include "timing_message.h"
#include "log.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

ControlPacketProcessor::ControlPacketProcessor(
    Switch *sw,
    const uint16_t &control_port,
    const uint16_t &data_port,
	int v
) : PacketProcessor(sw,v),
    control_port_(control_port),
    data_port_(data_port)
{}

ControlPacketProcessor::~ControlPacketProcessor()
{}

int ControlPacketProcessor::process(EventHandler *handler)
{
    // Process the control packet
    const uint16_t type = TimingMessage::type(packet_.buf());
	const size_t len = packet_.len();
    switch (type) {
        case TimingMessage::PortRequestMsg:
            LOG2("(CTRL) Port request from", Sockaddr::str(handler->from()));
			if (!port_reply(handler))
			    return -1;
            break;

        case TimingMessage::TimeStampMsg:
			if (TimeStamp::check(len)) {
                LOG2("(CTRL) Timestamp from", Sockaddr::str(handler->from()));
			    timestamp(handler);
			}
			else cerr << "Warning: ignore timestamp, len " << len << endl;
		    break;

        case TimingMessage::NodeStartMsg:
			if (TimeStamp::check(len)) {
                LOG2("(CTRL) Node start from", Sockaddr::str(handler->from()));
			    start_node(handler);
			}
			else cerr << "Warning: ignore nodestart, len " << len << endl;
		    break;

        case TimingMessage::NodeStopMsg:
			if (TimeStamp::check(len)) {
                LOG2("(CTRL) Node stop from", Sockaddr::str(handler->from()));
			    stop_node(handler);
			}
			else cerr << "Warning: ignore nodestop, len " << len << endl;
		    break;

        case TimingMessage::TimeQueryMsg:
			if (TimeStamp::check(len)) {
                LOG2("(CTRL) Time query from", Sockaddr::str(handler->from()));
			    time_query(handler);
			}
			else cerr << "Warning: ignore time query, len " << len << endl;
		    break;

		case TimingMessage::GTimeMsg:
		    break; // We shouldn't be getting it, ignore

		case TimingMessage::TerminateMsg:
			if (TimeStamp::check(len)) {
				int n = process_timing_message(handler)->id();
			    cout << "(CTRL) termination message from node " 
				     << n << ": goodbye!" << endl;
				switch_->send_terminate();
			    Stats::get().print_stats(1);
		        exit(0); // Goodbye!
			}
			else 
			    cerr << "Warning: ignore terminate, len " << len << endl;
		    break; 

		case TimingMessage::CpuidMsg:
			if (TimeStamp::check(len)) {
	            TimeStamp tdata(packet_.buf());
	            uint64_t x = tdata.tstamp();
	            uint16_t y = tdata.nodeid();
	            uint16_t z = tdata.seqno();
			    cout << "(CTRL) got cpuid message: " << y << " " << z << " " << x << endl;
				switch_->send_cpuid(x,y,z);
			}
			else 
			    cerr << "Warning: ignore cpuid, len " << len << endl;
		    break; 

        default:
            cout << "(CTRL) Unknown timing message: 0x" 
			     << hex << type << dec << endl;
            break;
    }
    return 0;
}

bool ControlPacketProcessor::port_reply(EventHandler *h)
{
	DataPort dp(data_port_);
    ssize_t nb = h->sendto(dp.data(),dp.len(),h->from(),sizeof(sockaddr_in));
    if (nb != dp.len()) {
        cerr << "Error in port request reply "
             << "(len=" << dp.len() << ", sent=" << nb << ")" 
             << endl;
        return false;
    }
	// Register the new node in the switch
	switch_->register_node(*reinterpret_cast<const sockaddr_in*>(h->from()));
	return true;
}

void ControlPacketProcessor::timestamp(EventHandler *h)
{
	Stats::get().n_msg_++;
	switch_->heartbeat_node(*process_timing_message(h));
}

void ControlPacketProcessor::start_node(EventHandler *h)
{
	// Message from abaeterno signaling a node has started
	Stats::get().n_msg_++;
	switch_->heartbeat_node(*process_timing_message(h));
}

void ControlPacketProcessor::stop_node(EventHandler *h)
{
	// Message from abaeterno signaling a node has stopped
	Stats::get().n_msg_++;
	switch_->stop_node(*process_timing_message(h));
}

void ControlPacketProcessor::time_query(EventHandler *h)
{
	// Message from abaeterno asking a resend of the last time
	Stats::get().n_msg_++;
	Node::Ptr node = process_timing_message(h);
	if (node->sync_valid())
	    switch_->timeout(); // force a sync
}

Node::Ptr ControlPacketProcessor::process_timing_message(EventHandler *h)
{
	TimeStamp tdata(packet_.buf());
	uint64_t tstamp = tdata.tstamp();
	uint16_t nodeid = tdata.nodeid();
	uint16_t seqno = tdata.seqno();

	// Register the new node in the switch
	const sockaddr_in *from = reinterpret_cast<const sockaddr_in*>(h->from());
	Node::Ptr node = switch_->register_node(nodeid,*from);
    node->setSimtime(tstamp,seqno); // Set the node sim time
    LOG2("(CTRL) Timing Message: Nodeid", nodeid, "Time", tstamp, "MAC", node->mac().str());
    return node;
}

