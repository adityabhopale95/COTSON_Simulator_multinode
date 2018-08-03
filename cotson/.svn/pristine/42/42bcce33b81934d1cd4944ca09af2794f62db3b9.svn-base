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

#include "reactor.h"
#include "packet_processor.h"
#include "vde_acceptor.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SWITCH_MAGIC 0xfeedface
#define REQ_VERSION  3

using namespace std;

enum request_type { REQ_NEW_CONTROL };

struct request_v3
{
    uint32_t magic;
    uint32_t version;
    enum request_type type;
    sockaddr_un sock;
};

VDEAcceptor::VDEAcceptor(Reactor *reactor, PacketProcessor *processor)
        : EventHandler(VDE), handle_(-1), reactor_(reactor), processor_(processor),
		  from_(Sockaddr::null<sockaddr_un>())
{}

VDEAcceptor::~VDEAcceptor()
{
    if (handle_ >= 0)
        (void) close_acceptor ();
}

int
VDEAcceptor::open_acceptor(const string &unix_socket, int pid)
{
    if (handle_ >= 0)
        return -1;  // Endpoint already opened.

    sockaddr_un sa;
    sa.sun_family = AF_UNIX;
    ::snprintf(sa.sun_path, sizeof(sa.sun_path), "%s", unix_socket.c_str());

    struct request_v3 req;
    req.magic = SWITCH_MAGIC;
    req.version = 3;
    req.type = REQ_NEW_CONTROL;

    // create the socket in the abstract name scope, man 7 unix
    req.sock.sun_family = AF_UNIX;
    ::memset(req.sock.sun_path, 0, sizeof(req.sock.sun_path));
    ::sprintf(&req.sock.sun_path[1], "%5d", pid);

    int fdcntl = ::socket (AF_UNIX, SOCK_STREAM, 0);
    if (fdcntl == -1) {
        perror("vde: Unable to obtain network (fdcntl socket)");
        return -1;
    }
    if (::connect(fdcntl, (sockaddr *) &sa, sizeof(sockaddr_un)) == -1) {
        perror("vde: Unable to obtain network (fdcntl connect)");
        return -1;
    }

    int fddata = ::socket (AF_UNIX, SOCK_DGRAM, 0);
    if (fddata == -1) {
        perror("vde: Unable to obtain network (fddata socket)");
        return -1;
    }
    if(::bind(fddata, (sockaddr *) &req.sock, sizeof(req.sock)) == -1) {
        perror("vde: Unable to obtain network (fddata bind)");
        return -1;
    }
    if (::send(fdcntl,&req,sizeof(req),0) < 0) {
        perror("vde: Unable to connect with VDE");
        return -1;
    }
    if (::recv(fdcntl,&from_,sizeof(sockaddr_un),0) == -1) {
        perror("vde: Unable to receive from VDE");
        return -1;
    }
    handle_ = fddata;

    // Register the acceptor in the reactor
    reactor_->register_handler (this);

    struct { char zero; int pid; int usecs; } name;
    ::memcpy(&name, from_.sun_path, sizeof(name));
    cout << "vde switch at pid [" << name.pid << "] usecs [" << name.usecs <<"]" << endl;
    return 0;
}

int VDEAcceptor::close_acceptor(void)
{
    if (handle_ >= 0) {
        if (::close (handle_) != 0) {
            perror("vde: Unable to close socket");
            return -1;

        } else
            handle_ = -1;
    }
    return 0;
}

int VDEAcceptor::sendto(const void* buf, size_t n, const sockaddr* to, size_t tolen)
{
    return ::sendto(handle_,buf,n,0,to,tolen);
}

int VDEAcceptor::fill(int maxfd,fd_set* r,fd_set* w,fd_set* e)
{
    FD_SET(handle_, r);
    return handle_ > maxfd ? handle_ : maxfd;
}

int VDEAcceptor::poll(fd_set* r,fd_set* w,fd_set* e)
{
    if (FD_ISSET(handle_, r)) {
		uint8_t buf[16384]; // FIXME: we need to remove this extra copy
        socklen_t fromlen = sizeof(from_);
	    sockaddr* from = reinterpret_cast<sockaddr*>(&from_);
        int nb = ::recvfrom(handle_,buf,16384,0,from,&fromlen);
	    if (nb < 0) {
            cerr << "Unable to receive from VDE" << endl;
			return -1;
		}
		else {
		    processor_->packet(buf,nb);
            return processor_->process(this);
	    }
	}
    return 0;
}

