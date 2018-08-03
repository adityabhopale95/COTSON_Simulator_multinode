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
#ifndef UNIT_H
#define UNIT_H

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <stdint.h>

// needed for the template friend function!
template<typename T> class units;
template<typename T> std::ostream& operator<< (std::ostream&,const units<T>&);
template<typename T> std::istream& operator>> (std::istream&,units<T>&);

template<typename T=uint64_t>
class units
{
	T n;

	static T getTranslation(const std::string& v)
	{
		static std::map<std::string,T> ts;
		if(ts.empty())
		{
			//See ... for an explanation of these
			//http://en.wikipedia.org/wiki/Kilobyte
			//http://en.wikipedia.org/wiki/SI_prefix
			ts["k"]=static_cast<T>(1000);
			ts["M"]=static_cast<T>(1000ULL*1000ULL);
			ts["G"]=static_cast<T>(1000ULL*1000ULL*1000ULL);
			ts["kB"]=static_cast<T>(1024);
			ts["MB"]=static_cast<T>(1024ULL*1024ULL);
			ts["GB"]=static_cast<T>(1024ULL*1024ULL*1024ULL);
		}
		if(v == "")
			return T(1);
		typename std::map<std::string,T>::iterator i=ts.find(v);
		if(i!=ts.end())
			return i->second;
		return T(0);
	}

	public:

	units(T n_=0): n(n_) {}
	units(const char*);
	operator T () const { return n; }

	friend std::ostream& operator<< <> (std::ostream& o,const units& d);
	friend std::istream& operator>> <> (std::istream& i,units& d);
};

template<typename T>
units<T>::units(const char* cs)
{
	std::string s(cs);
	std::stringstream a(s);
	T num;
	std::string mod;
	
	a >> num;
	if(!a)
		throw std::invalid_argument("wrong number: "+s);
	if(!a.eof())
		a >> mod;

	if(!a || !a.eof())
		throw std::invalid_argument("wrong number: "+s);

	T t=units<T>::getTranslation(mod);
	if(t==0)
		throw std::invalid_argument("wrong modifier: "+mod);

	n=num*t;
}

template<typename T>
std::ostream& operator<< (std::ostream& o,const units<T>& d)
{
	return o << d.n;
}

template<typename T>
std::istream& operator>> (std::istream& i,units<T>& d)
{
	std::string s;
	i >> s;
	d=units<T>(s.c_str());
	return i;
}

// specialisation for strings, which cant have units

template<> units<std::string>::units(const char*);
template<>
std::ostream& operator<<(std::ostream&,const units<std::string>&);
template<>
std::istream& operator>>(std::istream&,units<std::string>&);

#endif
