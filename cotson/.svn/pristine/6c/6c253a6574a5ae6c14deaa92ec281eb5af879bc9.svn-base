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

#ifndef VDEACCEPTOR_H_
#define VDEACCEPTOR_H_

#include "event_handler.h"

// Forward declarations
class Reactor;
class PacketProcessor;

class VDEAcceptor : public EventHandler
{
public:
    VDEAcceptor(Reactor *,  PacketProcessor *);
    virtual ~VDEAcceptor();

    // Open a passive connection on the provided unix socket
    int open_acceptor (const std::string &unix_socket, int pid);

    // Close the open connection
    int close_acceptor (void);
	
	// Fill select data
    int fill(int maxfd,fd_set* r,fd_set* w,fd_set* e);

	// Poll for new events
    int poll(fd_set* r,fd_set* w,fd_set* e);

	// Send packets
	int sendto(const void*,size_t,const sockaddr*,size_t);
	const sockaddr* from() { return reinterpret_cast<const sockaddr*>(&from_); }

private:
	// The socket
	int handle_;

    // The Reactor in which the stream will be registered.
    Reactor * const reactor_;

    // The processor that processes the packet when received
    PacketProcessor * const processor_;

	// Connector's address information
    sockaddr_un from_; 
};

#endif /*VDEACCEPTOR_H_*/
