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

// $Id: net_typedefs.h 77 2010-02-11 17:44:07Z paolofrb $

#ifndef NET_TYPEDEFS_H_
#define NET_TYPEDEFS_H_

#include "mac.h"

#include <set>
#include <iostream>
#include <string>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <boost/lexical_cast.hpp>

namespace Sockaddr {
    static inline bool equal(const sockaddr_in &a, const sockaddr_in &b)
    {
        return a.sin_port == b.sin_port && a.sin_addr.s_addr == b.sin_addr.s_addr;
    }

    static inline bool valid(const sockaddr_in &a)
    {
        return a.sin_port != 0 && a.sin_addr.s_addr != 0L;
    }

    static inline std::string str(const sockaddr_in &a)
    {
        return std::string(::inet_ntoa(a.sin_addr)) +
	           ":" + boost::lexical_cast<std::string>(a.sin_port);
    }
    static inline std::string str(const sockaddr &a)
	{
	    return str(*reinterpret_cast<const sockaddr_in*>(&a));
	}
    static inline std::string str(const sockaddr *a) { return str(*a); }
    static inline std::string str(const sockaddr_in *a) { return str(*a); }
	template<typename T> static inline T null() { T a; ::memset(&a,0,sizeof(T)); return a; };
};

#endif
