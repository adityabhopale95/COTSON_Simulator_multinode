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

// $Id: reactor.h 11 2010-01-08 13:20:58Z paolofrb $

#ifndef REACTOR_H_
#define REACTOR_H_

#include "event_handler.h"

#include <sys/types.h>
#include <vector>

class Reactor
{
public:
    Reactor();
    ~Reactor();

    // Begin the event loop.
    void handle_events();

    // Register <event_handler> with <mask> (the handle comes from <event_handler>)
    void register_handler(EventHandler*);

private:
	// Vector of handlers
	typedef std::vector<EventHandler*> HandlerVec;
	HandlerVec handlers_;
};

#endif /*REACTOR_H_*/

