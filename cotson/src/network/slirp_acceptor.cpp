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

// $Id: slirp_acceptor.cpp 77 2010-02-11 17:44:07Z paolofrb $

#include "reactor.h"
#include "packet_processor.h"
#include "slirp_acceptor.h"
#include "../slirp/libslirp.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

using namespace std;

SlirpAcceptor::SlirpAcceptor()
    : EventHandler(SLIRP),reactor_(0),processor_(0),
      null_addr_(Sockaddr::null<sockaddr>())
{}

SlirpAcceptor::~SlirpAcceptor() {}

void SlirpAcceptor::init(const char *n, Reactor *r, PacketProcessor* p) 
{
    network_ = n; 
    reactor_ = r; 
    processor_ = p; 
}

void SlirpAcceptor::redir(uint32_t n, uint16_t p1, uint16_t p2)
{
    if (n && p1 && p2) {
	    struct in_addr guest_addr = inet_makeaddr(ntohl(inet_addr(network_)),n);
		if (slirp_redir(false,p2,guest_addr,p1)) {
            cerr << "ERROR: cannot redirect tcp " << inet_ntoa(guest_addr) << ":" << p1
		         << " to localhost:" << p2 << endl;
		}
		if (slirp_redir(true,p2,guest_addr,p1)) {
            cerr << "ERROR: cannot redirect udp " << inet_ntoa(guest_addr) << ":" << p1
		         << " to localhost:" << p2 << endl;
		}
		else {
            cout << "Redirected " << inet_ntoa(guest_addr) << ":" << p1
		         << " to localhost:" << p2 << endl;
	    }
	}
}

int SlirpAcceptor::open_acceptor(int pid)
{
	::strcpy(slirp_hostname,"slirp.cotson.com");
	slirp_init(false,network_);
    reactor_->register_handler(this);
    cout << "slirp connection started" << endl;
    return 0;
}

int SlirpAcceptor::close_acceptor(void) { return 0; }

int SlirpAcceptor::fill(int maxfd, fd_set* r,fd_set* w,fd_set* e)
{
	int mx = maxfd;
    slirp_select_fill(&mx,r,w,e);
	return mx;
}

int SlirpAcceptor::poll(fd_set* r,fd_set* w,fd_set* e)
{
    slirp_select_poll(r,w,e);
	return 0;
}

int SlirpAcceptor::sendto(const void* buf, size_t len,const sockaddr* to, size_t tolen)
{
	// cout << "### SLIRP SEND [" << len << "]\n";
	slirp_input(reinterpret_cast<const uint8_t*>(buf),len);
	return len;
}

int SlirpAcceptor::handle_input(const uint8_t* buf, int len) 
{
	if (processor_) {
		processor_->packet(buf,len);
        return processor_->process(this);
	}
    return -1;
}

// ########### LIBSLIRP INTERFACE ############

extern "C" void slirp_output(const uint8_t *pkt, int pkt_len) 
{
	// cout << "### SLIRP OUTPUT [" << pkt_len << "]\n";
	SlirpAcceptor::get().handle_input(pkt,pkt_len);
}

extern "C" int slirp_can_output(void) 
{ 
    return 1; 
}

