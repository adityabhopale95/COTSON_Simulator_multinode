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

// $Id: simplevec.h 11 2010-01-08 13:20:58Z paolofrb $

#ifndef SIMPLEVEC_H
#define SIMPLEVEC_H

#include <stdlib.h>
#include <boost/noncopyable.hpp>

template <typename T> class simplevec : public boost::noncopyable
{
public:
    typedef const T* const_iterator;
	const_iterator begin() const { return buf_; }
	const_iterator end() const { return end_; }
	simplevec(uint32_t sz=2) : defsz_(sz),buf_(init(defsz_)),end_(buf_),sz_(0),maxsz_(defsz_) {}
	~simplevec() { free(buf_); } 
	inline size_t size() const { return sz_; }
	inline void copy(const simplevec<T>& other)
	{
		sz_ = other.sz_;
		if (sz_ > maxsz_)
		    buf_=resize(maxsz_=sz_);
		if (sz_)
	        (void)memcpy(buf_,other.buf_,sz_*sizeof(T));
		end_=buf_+sz_;
	}
	inline void push_back(const T& a)
	{
		if (sz_>=maxsz_)
			end_=(buf_=resize(maxsz_*=16))+sz_; // grow the buffer
	    *end_++=a;
		sz_++;
	}
	inline void clear()
	{
	    if (maxsz_>defsz_)
			buf_=resize(maxsz_=defsz_); // shrink the buffer back
		end_=buf_;
		sz_=0;
	}
	inline T& operator[](size_t n) { return buf_[n]; }
	inline const T& operator[](size_t n) const { return buf_[n]; }
private:
	inline T* resize(size_t s) 
	{
	    T* b = static_cast<T*>(realloc(buf_,s*sizeof(T)));
		if (!b)
		    throw std::runtime_error("simplevec: can't allocate memory");
	    return b;
	}
	inline T* init(size_t s) 
	{
	    T* b = static_cast<T*>(malloc(s*sizeof(T)));
		if (!b)
		    throw std::runtime_error("simplevec: can't allocate memory");
	    return b;
	}
	const size_t defsz_;
    T *buf_, *end_;
	size_t sz_, maxsz_;
};

#endif

