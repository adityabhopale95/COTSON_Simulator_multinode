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

// $Id: event_handler.h 77 2010-02-11 17:44:07Z paolofrb $

#ifndef EVENTHANDLER_H_
#define EVENTHANDLER_H_

#include <glob.h>
#include <stdint.h>
#include <sys/select.h>

// forward declaration
struct sockaddr;

class EventHandler
{
public:
	enum Type { DATA, VDE, SLIRP };

    EventHandler(Type t):type_(t) {}
    virtual ~EventHandler() {}

	Type type() const { return type_; }

	// Called to register a new handle
	virtual int fill(int,fd_set*,fd_set*,fd_set*) = 0;

	// Poll for new events
	virtual int poll(fd_set*,fd_set*,fd_set*) = 0;

	// Got a timeout
	virtual void timeout() {}

	// Send hooks
	virtual int sendto(const void*,size_t,const sockaddr*,size_t) = 0;
	virtual const sockaddr* from() = 0;

protected:
	Type type_;
};

#endif /*EVENTHANDLER_H_*/
