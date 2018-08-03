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
#ifndef SERIES_H
#define SERIES_H
#include "circbuf.h"

template<typename T> class Series
{
private:
    CircBuf<T> items;
    unsigned int count;
	T zero;
    T accum;

public:
    Series(unsigned int n, const T& v0):items(n),count(0),zero(v0),accum(v0)
    {
        items.init(v0);
    }
    ~Series() {}

    inline const T& operator[](unsigned int i) const
	{
       return items[i];
    }

    void push_back(const T& item) 
    {
        accum = count ? (accum - items[0]) + item : item;
        items.push_back(item); 
	    count++;
    }

    T mavg() const
    {
		unsigned int n = count < items.size() ? count : items.size();
		if (n==0)
		    return zero;
	    return accum / static_cast<T>(n);
    }

    T mvar2() const
    {
		unsigned int n = count < items.size() ? count : items.size();
		if (n <= 1)
		    return zero;
		T avg = mavg();
		T mv = zero;
		for (unsigned int i = 1; i <= n; ++i) {
		   T diff = items[i] - avg;
		   mv += diff * diff;
		}
	    return mv / static_cast<T>(n);
    }

    T arma22(const Series<T>& pred,
             const T& a0, const T& a1, 
	         const T& m0, const T& m1) const
    {
        const T& x0 = items[1];
	    if (count < 2)
		    return x0;
	    const T& x1 = items[2];
	    const T& h0 = pred[1];
	    const T& h1 = pred[2];
        return (a0 * x0) + (a1 * x1) + (m0 * (x0 - h0)) + (m1 * (x1 - h1));
    }
};

#endif
