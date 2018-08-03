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

#ifndef DATAPACKETPROCESSOR_H_
#define DATAPACKETPROCESSOR_H_

#include "packet_processor.h"
#include "net_typedefs.h"

#include <map>

// Forward declarations.
class PacketAcceptor;
class Switch;
class Stats;
class Packet;

class DataPacketProcessor : public PacketProcessor
{
public:
    DataPacketProcessor(Switch*, int);
    virtual ~DataPacketProcessor();

    // Process the data packet.
    virtual int process(EventHandler*);

    // Send a data packet to the destination address
    int send_data_packet(const sockaddr_in& addr,
                         const Packet& packet,
                         const char* description);

    // Send a data packet to the VDE
    int send_vde_packet(const Packet& packet,
                        const char* description);

    // Send a data packet to the Slirp
    int send_slirp_packet(const Packet& packet,
                          const char* description);

    void set_data_handler(EventHandler *h) { data_handler_ = h; }
    void set_slirp_handler(EventHandler *h) { slirp_handler_ = h; }
    void set_vde_handler(EventHandler *h) { vde_handler_ = h; }

private:
    // Process an ethernet packet
	enum { MAX_SEND_RETRIES = 5 };
    int process_packet(ether_header *eh,
                       int len,
                       MacAddress& src,
                       MacAddress& dst,
                       int reg,
                       bool from_vde);

    // Send the packet to the address - retries several times.
    int send_packet(EventHandler* handler,
                    const sockaddr* addr,
                    size_t addre_len,
					const Packet& p,
                    const char *description);

    EventHandler *data_handler_;
    EventHandler *slirp_handler_;
    EventHandler *vde_handler_;
};

#endif /*DATAPACKETPROCESSOR_H_*/
