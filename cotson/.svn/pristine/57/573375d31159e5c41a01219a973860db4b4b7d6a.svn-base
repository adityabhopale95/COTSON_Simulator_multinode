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

#ifndef CONTROLPACKETPROCESSOR_H_
#define CONTROLPACKETPROCESSOR_H_

#include "packet_processor.h"
#include "node.h"
#include "net_typedefs.h"

// Forward declarations.
class PacketAcceptor;
class Switch;

class ControlPacketProcessor : public PacketProcessor
{
public:
    ControlPacketProcessor(Switch *,const uint16_t &,const uint16_t &, int);
    ~ControlPacketProcessor();

    // Process the control packet
    int process(EventHandler *);

private:
	// timestamps, start/stop nodes
	void timestamp(EventHandler *);
	void start_node(EventHandler *);
	void stop_node(EventHandler *);
    void time_query(EventHandler *);
	bool port_reply(EventHandler*);
    Node::Ptr process_timing_message(EventHandler *);

    const uint16_t control_port_;
    const uint16_t data_port_;
};

#endif /*CONTROLPACKETPROCESSOR_H_*/
