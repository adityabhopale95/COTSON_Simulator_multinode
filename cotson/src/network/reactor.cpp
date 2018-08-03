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

// $Id: reactor.cpp 136 2010-11-07 15:50:41Z paolofrb $

#include "reactor.h"
#include "stats.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

Reactor::Reactor() { }
Reactor::~Reactor() { }

void Reactor::handle_events(void)
{
    if (handlers_.size() == 0)
        return;

	// Fill the file descriptor sets

	// Loop forever
    while (true) {
	    // Local copy (select modifies)
		fd_set rfds, wfds, xfds;
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_ZERO(&xfds);
	    int max_fd = -1;
	    for (HandlerVec::iterator hi = handlers_.begin(); hi != handlers_.end(); hi++)
            max_fd = (*hi)->fill(max_fd,&rfds,&wfds,&xfds);

        // Multiplex the file descriptors
		timeval timeout = {10,0}; // timeout after 1s fo0r safety
		int nb = ::select(max_fd+1,&rfds,&wfds,&xfds,&timeout);
        if (nb == -1) {
			perror("Error in select");
		    return;
		}
		if (nb > 0) { // Poll registered handlers
		    for (HandlerVec::iterator hi = handlers_.begin(); hi != handlers_.end(); hi++)
			    (*hi)->poll(&rfds,&wfds,&xfds);
		}
		else if (nb==0) { // Timeout
		    for (HandlerVec::iterator hi = handlers_.begin(); hi != handlers_.end(); hi++)
			    (*hi)->timeout();
		}
    }
}

void Reactor::register_handler(EventHandler *handler)
{
    if (handler)
	    handlers_.push_back(handler);
}

