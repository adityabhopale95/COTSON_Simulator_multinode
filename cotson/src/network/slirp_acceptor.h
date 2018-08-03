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

// $Id: slirp_acceptor.h 77 2010-02-11 17:44:07Z paolofrb $

#ifndef SLIRPACCEPTOR_H_
#define SLIRPACCEPTOR_H_

#include "event_handler.h"

// Forward declarations
class Reactor;
class PacketProcessor;

class SlirpAcceptor : public EventHandler
{
public:
    static SlirpAcceptor& get() {
	    static SlirpAcceptor singleton; 
		return singleton; 
	}

    SlirpAcceptor();
    ~SlirpAcceptor();

    // Open a passive connection on the provided unix socket
    int open_acceptor (int pid);

    // Close the open connection.
    int close_acceptor (void);

	// Called to register a new handle
	int fill(int,fd_set*,fd_set*,fd_set*);

	// Poll for new events
	int poll(fd_set*,fd_set*,fd_set* e);

	// Handle incoming packet
    int handle_input(const uint8_t*, int) ;

	// Send and receive packets
	int sendto(const void*,size_t,const sockaddr*,size_t);
	int receive(uint8_t*,size_t);
	const sockaddr* from() { return &null_addr_; }

	// Set initial state
	void init(const char *n, Reactor *r, PacketProcessor* p);
	void redir(uint32_t n, uint16_t p1, uint16_t p2);

private:
    // The Reactor in which the stream will be registered.
    Reactor* reactor_;

    // The processor that processes the packet when received
    PacketProcessor* processor_;

	// The slirp network (for DHCP)
	const char* network_;
	sockaddr null_addr_; 
};

#endif /*SLIRPACCEPTOR_H_*/
