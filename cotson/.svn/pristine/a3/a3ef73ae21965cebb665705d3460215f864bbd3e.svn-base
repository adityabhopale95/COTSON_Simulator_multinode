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

#ifndef PACKETACCEPTOR_H_
#define PACKETACCEPTOR_H_

#include "event_handler.h"
#include <netinet/in.h>

// Forward declarations
class Reactor;
class PacketProcessor;

class PacketAcceptor : public EventHandler
{
public:
    PacketAcceptor(Reactor *, PacketProcessor *);
    virtual ~PacketAcceptor();

    // Open a passive connection on the provided port number.
    int open_acceptor(uint16_t, int=0);

    // Close the open connection.
    int close_acceptor(void);

	// Fill select data
    int fill(int maxfd,fd_set* r,fd_set* w,fd_set* e);

	// Handle events
	int poll(fd_set* r,fd_set* w,fd_set* e);

	// Flush pending events 
	void flush();

	// Handle timeouts
	void timeout();

	// Send packets
	int sendto(const void*,size_t,const sockaddr*,size_t);
	const sockaddr* from() { return reinterpret_cast<const sockaddr*>(&from_); }

private:
	// Socket
    int handle_;

    // The underlying socket address structure.
    sockaddr_in addr_;

	// Connector's address information
    sockaddr_in from_; 

    // The Reactor in which the stream will be registered.
    Reactor * const reactor_;

    // The processor that processes the packet when received.
    PacketProcessor * const processor_;
};

#endif /*PACKETACCEPTOR_H_*/
