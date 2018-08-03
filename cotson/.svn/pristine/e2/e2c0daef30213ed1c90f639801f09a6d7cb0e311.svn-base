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
#ifndef CIRCBUF_H
#define CIRCBUF_H

#include <boost/noncopyable.hpp>

template<typename T> class CircBuf : public boost::noncopyable
{
private:
   T* items;
   unsigned int head;
   const unsigned int N;
   unsigned int nn;

public:
   CircBuf(unsigned int size) : N(size),nn(0)
   {
        items = new T[size];
        head = 1 % N;
        for (unsigned int i=0; i < size; ++i)
	        items[i] = static_cast<T>(0);
   }

   ~CircBuf() 
   {
       delete[] items; 
   }

   inline const T& operator[](unsigned int i) const 
   {
       return items[ (head - i) % N ]; 
   }

   inline void push_back(const T& item) 
   {
       items[head] = item;
       head = ((head + 1) % N); 
	   if (nn < N) nn++;
   }

   inline unsigned int size() const { return nn; }
   inline bool empty() const { return nn==0; }

   void init(const T& val) 
   {
       for(unsigned int i = 0; i < N; ++i) 
           items[i] = val;
   }
};

#endif
