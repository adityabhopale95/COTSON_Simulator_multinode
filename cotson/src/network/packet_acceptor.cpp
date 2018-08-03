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

// $Id: packet_acceptor.cpp 77 2010-02-11 17:44:07Z paolofrb $

#include "reactor.h"
#include "packet_processor.h"
#include "packet_acceptor.h"

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

using namespace std;
const int MAXPACKSZ = 16384; // enough for jumbo frames, FDDI, etc.

PacketAcceptor::PacketAcceptor(Reactor *r, PacketProcessor *p)
        : EventHandler(DATA),
		  handle_(-1),
		  addr_(Sockaddr::null<sockaddr_in>()),
          from_(Sockaddr::null<sockaddr_in>()),
		  reactor_(r),processor_(p)
{}

PacketAcceptor::~PacketAcceptor()
{
    (void)close_acceptor();
}

int PacketAcceptor::open_acceptor(uint16_t port, int sockopt)
{
    if (handle_ >= 0)
        return -1;  // Endpoint already opened.

    handle_ = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (handle_ == -1) {
		::perror("Unable to create socket");
        return -1;
    }
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = htonl(INADDR_ANY);

	if (sockopt) {
	    int yes = 1;
	    if (::setsockopt(handle_,SOL_SOCKET,sockopt,&yes,sizeof(yes)) < 0) {
		    ::perror("Unable to set input socket options");
            return -1;
		}
	}

	// Bind the port
    if (::bind(handle_,reinterpret_cast<sockaddr *>(&addr_),sizeof(addr_)) != 0) {
		cerr << "Unable to bind socket (port " << port << ") ";
		::perror("");
        return -1;
    }
    // Register the acceptor in the reactor
    reactor_->register_handler(this);
    return 0;
}

int PacketAcceptor::close_acceptor()
{
    if (handle_ >= 0) {
        if (::close(handle_) != 0) {
            cerr << "Unable to close socket" << endl;
            return -1;
        }
		else
            handle_ = -1;
    }
    return 0;
}

void PacketAcceptor::flush()
{
	uint8_t junk[MAXPACKSZ];
	uint32_t nj = 0;
    if (handle_ < 0)
	    return;
    fd_set rfd;
	// Flush all pending messages from previous runs
	while(true) {
	    FD_ZERO(&rfd);
	    FD_SET(handle_,&rfd);
        timeval timeout = {0,1000000}; // timeout after 1ms
	    int nfd = ::select(handle_+1,&rfd,0,0,&timeout);
	    if (!(nfd && FD_ISSET(handle_,&rfd))) {
			if (nj)
			    cout << "Flushed " << nj << " junk packets" << endl;
		    return;
		}
		cout << "junk packet\n";
	    recv(handle_,junk,MAXPACKSZ,0);
		nj++;
	}
}

int PacketAcceptor::sendto(const void* buf, size_t n, const sockaddr* to, size_t tolen)
{
    // return ::sendto(outsock_,buf,n,MSG_DONTWAIT,to,tolen);
    return ::sendto(handle_,buf,n,0,to,tolen);
}

int PacketAcceptor::fill(int maxfd,fd_set* r,fd_set* w,fd_set* e)
{
    FD_SET(handle_, r);
    return handle_ > maxfd ? handle_ : maxfd;
}

int PacketAcceptor::poll(fd_set* r,fd_set* w,fd_set* e)
{
    if (FD_ISSET(handle_, r)) {
		uint8_t buf[MAXPACKSZ]; // FIXME: we need to remove this extra copy
        socklen_t fromlen = sizeof(from_);
	    sockaddr* from = reinterpret_cast<sockaddr*>(&from_);
        int nb = ::recvfrom(handle_,buf,MAXPACKSZ,0,from,&fromlen);
		if (nb < 0) {
		    ::perror("Unable to receive packet");
			return -1;
		}
		else {
		    processor_->packet(buf,nb);
			return processor_->process(this);
	    }
	}
    return 0;
}

void PacketAcceptor::timeout()
{
    processor_->timeout();
}

