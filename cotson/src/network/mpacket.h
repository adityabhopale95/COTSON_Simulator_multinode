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

// $Id: mpacket.h 11 2010-01-08 13:20:58Z paolofrb $


#ifndef PACKET_H_
#define PACKET_H_

#include <stdint.h>
#include <string.h>
#include <iostream>
#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

class Packet : public boost::noncopyable
{
public:
    inline Packet():buf_len_(0) { buf_.reset(); }
    inline Packet(const Packet& p):buf_len_(0) { load(p.buf(),p.len()); }
    inline Packet(ssize_t n):buf_len_(n) { buf_.reset(new uint8_t[n]); }
    inline ~Packet() {}

	typedef boost::shared_ptr<Packet> Ptr;

    inline void load(const uint8_t* b,ssize_t l)
	{
		if (l > buf_len_) 
            buf_.reset(new uint8_t[l]);
		(void)::memcpy(buf_.get(),b,l);
	    buf_len_ = l;
	}

	inline ssize_t len() const { return buf_len_; }
	inline const uint8_t* buf() const { return buf_.get(); }

private:
    // The buffer that keeps the received packet and its length
	boost::scoped_array<uint8_t> buf_;
    ssize_t buf_len_;
};

inline std::ostream& operator<<(std::ostream& s,const Packet& p)
{
	return s << "len " << p.len();
}

#endif /*PACKET_H_*/
