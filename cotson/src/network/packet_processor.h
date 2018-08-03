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

// $Id: packet_processor.h 11 2010-01-08 13:20:58Z paolofrb $
#ifndef PACKETPROCESSOR_H_
#define PACKETPROCESSOR_H_

#include "event_handler.h"
#include "mpacket.h"
#include "switch.h"
#include "timing_message.h"
#include "net_typedefs.h"

#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <net/ethernet.h>
#include <math.h>

// Forward declarations
class Node;

class PacketProcessor
{
public:
    PacketProcessor(Switch*,int);
    virtual ~PacketProcessor();

    // Process the packet
    virtual int process(EventHandler*) = 0;

	// Load the packet
	inline void packet(const uint8_t* b,size_t l) { packet_.load(b,l); }

	// Handle timeouts
	inline void timeout() { switch_->timeout(); }
protected:

	// constants
    Packet packet_; // The received packet
    Switch* switch_; // Switch that keeps the registry of mac addresses and ports

	int verbose_;
};

#endif /*PACKETPROCESSOR_H_*/
