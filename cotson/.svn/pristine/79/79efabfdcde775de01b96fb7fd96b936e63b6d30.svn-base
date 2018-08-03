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
#ifndef METRIC_H
#define METRIC_H

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include <map>
#include <set>
#include <stdexcept>
#include <stdint.h>
#include <boost/ref.hpp>

template<typename T>
struct basic_difference
{
	T v;
	T base;

	inline basic_difference() : v(0), base(0) {}
	inline void set(T n) { v=n-base; }
	inline void reset(T n) { base=n,v=0; }
	inline T absolute() { return base+v; }
	inline operator T() const { return v; }
};


template<typename T,typename R> class basic_metric_iterator;
template<typename T,typename R> class basic_metric;

template<typename T,typename R>
struct basic_metric_storage
{
public:
	basic_metric_storage() {}
	virtual ~basic_metric_storage() {}

	void operator+=(const basic_metric_storage&);
	void operator-=(const basic_metric_storage&);
	template<typename S> void operator*=(S v);
	template<typename S> void operator/=(S v);

protected:
	typedef std::map<std::string,T> Values;
	Values vals;
	
	friend class basic_metric<T,R>;
};

template<typename T,typename R>
class basic_metric
{
public:
	virtual ~basic_metric() {}
	typedef basic_metric_iterator<T,R> iterator;

	T& operator[](const std::string& name);
	const T& operator[](const std::string& name) const;

	R ratio(const std::string& name) const;
	R percentage(const std::string& name) const;

	void clear_metrics();
	void compute_metrics();
	
	iterator begin();
	iterator end();

	iterator names_begin();
	iterator names_end();

	iterator ratios_begin();
	iterator ratios_end();

	basic_metric_storage<T,R> checkpoint();
	void restore(basic_metric_storage<T,R>&);

	virtual void dump() {}

protected:	
	void add(const std::string& name,T& elem);
	void add(const std::string& name,basic_difference<T>& elem);
	void add_ratio(const std::string& name,const std::string& num,const std::string& den);
	void add(const std::string& prefix,basic_metric<T,R>&);

private:	
	virtual void do_compute_metrics() {}

	typedef std::map<std::string,boost::reference_wrapper<T> > References;
	typedef std::map<std::string,std::pair<std::string,std::string> > Ratios;
	typedef std::map<basic_metric<T,R>*,std::string> Metrics;
	typedef std::set<basic_difference<T>*> Differences;

	References refs;   // all references (including subs)
	Ratios ratios;     // all ratios     (including subs)
	Metrics subs,sups; // all metrics (with prefixes used)
	Differences difs;  // all differences

	void must_not_exist(const std::string& name) const;
	typename References::const_iterator must_exist(const std::string& name) const;
	Ratios::const_iterator must_existRatio(const std::string& name) const;

	friend class basic_metric_iterator<T,R>;
};

typedef basic_difference<uint64_t> difference;
typedef basic_metric_storage<uint64_t,double> metric_storage;
typedef basic_metric<uint64_t,double> metric;

template<typename T,typename R>
class basic_metric_iterator
{
public:
	enum position 
	{
		START=0, 
		NAMES_START=START, 
		NAMES_END=1,
		RATIOS_START=NAMES_END,
		END=2,
		RATIOS_END=END
	};
	basic_metric_iterator();
	basic_metric_iterator(basic_metric<T,R>*,position);

	std::string operator*();
	
	void operator++();
	void operator++(int);
	bool operator==(const basic_metric_iterator<T,R>&);
	bool operator!=(const basic_metric_iterator<T,R>&);

private:
	void normalize();

	basic_metric<T,R>* metric;
	bool normal;
	typename basic_metric<T,R>::References::iterator fI;
	typename basic_metric<T,R>::Ratios::iterator tI;
};

//// implementation of basic_metric_storage

template<typename T,typename R>
void basic_metric_storage<T,R>::operator+=(const basic_metric_storage<T,R>&other)
{
	typename Values::iterator i;
	typename Values::const_iterator j;
	for(i=vals.begin(), j=other.vals.begin();i!=vals.end() && j!=other.vals.end();++i,++j)
	{
		if(i->first != j->first)
			throw std::runtime_error("discrepancy in elements to operator+= " + i->first + " and " + j->first);
		i->second+=j->second;
	}
	if(i!=vals.end())
		throw std::runtime_error("operator+= found more elements in destiny");
	if(j!=other.vals.end())
		throw std::runtime_error("operator+= found more elements in source");
}

template<typename T,typename R>
void basic_metric_storage<T,R>::operator-=(const basic_metric_storage<T,R>&other)
{
	typename Values::iterator i;
	typename Values::const_iterator j;
	
	for(i=vals.begin(),j=other.vals.begin();i!=vals.end() && j!=other.vals.end();++i,++j)
	{
		if(i->first != j->first)
			throw std::runtime_error("discrepancy in elements to operator-= " + i->first + " and " + j->first);
		i->second-=j->second;
	}
	if(i!=vals.end())
		throw std::runtime_error("operator-= found more elements in destiny");
	if(j!=other.vals.end())
		throw std::runtime_error("operator-= found more elements in source");
}

template<typename T,typename R> template<typename S>
void basic_metric_storage<T,R>::operator*=(S v)
{
	for(typename Values::iterator i=vals.begin();i!=vals.end();++i)
		i->second*=v;
}

template<typename T,typename R> template<typename S>
void basic_metric_storage<T,R>::operator/=(S v)
{
	for(typename Values::iterator i=vals.begin();i!=vals.end();++i)
		i->second/=v;
}

//// implementation of basic_metric

template<typename T,typename R>
void basic_metric<T,R>::add(const std::string& name,T& elem)
{
	must_not_exist(name);
	refs.insert(make_pair(name,boost::reference_wrapper<T>(elem)));
	for(typename Metrics::iterator i=sups.begin();i!=sups.end();++i)
		i->first->add(i->second+name,elem);
}

template<typename T,typename R>
void basic_metric<T,R>::add(const std::string& name,basic_difference<T>& elem)
{
	must_not_exist(name);	
	refs.insert(make_pair(name,boost::reference_wrapper<T>(elem.v)));
	difs.insert(&elem);
}

template<typename T,typename R>
void basic_metric<T,R>::add_ratio(const std::string& name,const std::string& num,const std::string& den)
{
	must_not_exist(name);	
	must_exist(num);
	must_exist(den);
	ratios[name]=make_pair(num,den);

	for(typename Metrics::iterator i=sups.begin();i!=sups.end();++i)
		i->first->add_ratio(i->second+name,i->second+num,i->second+den);
}

template<typename T,typename R>
void basic_metric<T,R>::add(const std::string& prefix,basic_metric<T,R>&sub)
{
	// we add all current references
	for(typename References::iterator i=sub.refs.begin();i!=sub.refs.end();++i)
		add(prefix+i->first,static_cast<T&>(i->second));
	// we add all current ratios
	for(typename Ratios::iterator i=sub.ratios.begin();i!=sub.ratios.end();++i)
		add_ratio(prefix+i->first,prefix+i->second.first,prefix+i->second.second);

	subs[&sub]=prefix;
	sub.sups[this]=prefix;
}

template<typename T,typename R>
const T& basic_metric<T,R>::operator[](const std::string& name) const
{
	typename References::const_iterator f=must_exist(name);
	return f->second;
}

// Use const whenever possible, (Meyer's Effective C++)
template<typename T,typename R>
T& basic_metric<T,R>::operator[](const std::string& name)
{
	return const_cast<T&>(static_cast<const basic_metric<T,R>&>(*this)[name]);
}

template<typename T,typename R>
R basic_metric<T,R>::ratio(const std::string& name) const
{
	typename Ratios::const_iterator f=must_existRatio(name);
	const T& num=operator[](f->second.first);
	const T& den=operator[](f->second.second);
	return static_cast<R>(num)/static_cast<R>(den);
}

template<typename T,typename R>
R basic_metric<T,R>::percentage(const std::string& name) const
{
	return ratio(name)*100;
}

template<typename T,typename R>
void basic_metric<T,R>::clear_metrics()
{
	for(typename Differences::iterator i=difs.begin();i!=difs.end();++i)
		(*i)->reset((*i)->v+(*i)->base);
	for(typename References::iterator i=refs.begin();i!=refs.end();++i)
		static_cast<T&>(i->second)=0;
}	

template<typename T,typename R>
void basic_metric<T,R>::compute_metrics()
{
	for(typename Metrics::iterator i=subs.begin();i!=subs.end();++i)
		i->first->compute_metrics();
	do_compute_metrics();
}	

template<typename T,typename R>
void basic_metric<T,R>::must_not_exist(const std::string& name) const
{
	typename References::const_iterator f=refs.find(name);
	if(f!=refs.end())
		throw std::runtime_error("reference '"+name+"' already exists");
}

template<typename T,typename R>
typename basic_metric<T,R>::References::const_iterator basic_metric<T,R>::must_exist(const std::string& name) const	
{
	typename References::const_iterator f=refs.find(name);
	if(f==refs.end())
		throw std::runtime_error("reference '"+name+"' does not exist");
	return f;
}

template<typename T,typename R>
basic_metric<T,R>::Ratios::const_iterator basic_metric<T,R>::must_existRatio(const std::string& name) const
{
	typename Ratios::const_iterator f=ratios.find(name);
	if(f==ratios.end())
		throw std::runtime_error("reference '"+name+"' does not exist");
	return f;
}

template<typename T,typename R>
typename basic_metric<T,R>::iterator basic_metric<T,R>::begin()
{
	return basic_metric_iterator<T,R>(this,basic_metric_iterator<T,R>::START);
}

template<typename T,typename R>
typename basic_metric<T,R>::iterator basic_metric<T,R>::end()
{
	return basic_metric_iterator<T,R>(this,basic_metric_iterator<T,R>::END);
}

template<typename T,typename R>
typename basic_metric<T,R>::iterator basic_metric<T,R>::names_begin()
{
	return basic_metric_iterator<T,R>(this,basic_metric_iterator<T,R>::NAMES_START);
}

template<typename T,typename R>
typename basic_metric<T,R>::iterator basic_metric<T,R>::names_end()
{
	return basic_metric_iterator<T,R>(this,basic_metric_iterator<T,R>::NAMES_END);
}

template<typename T,typename R>
typename basic_metric<T,R>::iterator basic_metric<T,R>::ratios_begin()
{
	return basic_metric_iterator<T,R>(this,basic_metric_iterator<T,R>::RATIOS_START);
}

template<typename T,typename R>
typename basic_metric<T,R>::iterator basic_metric<T,R>::ratios_end()
{
	return basic_metric_iterator<T,R>(this,basic_metric_iterator<T,R>::RATIOS_END);
}

template<typename T,typename R>
basic_metric_storage<T,R> basic_metric<T,R>::checkpoint()
{
	basic_metric_storage<T,R> storage;
	for(typename References::iterator i=refs.begin();i!=refs.end();++i)
		storage.vals[i->first]=i->second;
	return storage;
}

template<typename T,typename R>
void basic_metric<T,R>::restore(basic_metric_storage<T,R>& storage)
{
	for(typename basic_metric_storage<T,R>::Values::iterator i=storage.vals.begin();
		i!=storage.vals.end();++i)
	{
		must_exist(i->first);
		(*this)[i->first]=i->second;	
	}
}

//// implementation of basic_metric_iterator

template<typename T,typename R>
basic_metric_iterator<T,R>::basic_metric_iterator() : 
	metric(0) 
{}

template<typename T,typename R>
basic_metric_iterator<T,R>::basic_metric_iterator(
	basic_metric<T,R>* m,
	basic_metric_iterator<T,R>::position pos) : 
		metric(m),
		normal(true),
		fI(m->refs.begin()),
		tI(m->ratios.begin()) 
{
	if(pos==1)
		fI=metric->refs.end();
	if(pos==2)
	{
		fI=metric->refs.end();
		tI=metric->ratios.end();
	}
	normalize();			
}

template<typename T,typename R>
std::string basic_metric_iterator<T,R>::operator*()
{
	if(normal)
		return fI->first;
	return tI->first;
}
	
template<typename T,typename R>
void basic_metric_iterator<T,R>::operator++()
{
	if(normal)
		fI++;
	else
		tI++;
	normalize();
}
	
template<typename T,typename R>
void basic_metric_iterator<T,R>::operator++(int) 
{ 
	operator++(); 
}

template<typename T,typename R>
bool basic_metric_iterator<T,R>::operator==(const basic_metric_iterator<T,R>& other)
{
	if(metric==other.metric && 
		normal==other.normal &&
		fI==other.fI &&
		tI==other.tI)
			return true;			
	return false;
}

template<typename T,typename R>
bool basic_metric_iterator<T,R>::operator!=(const basic_metric_iterator<T,R>& other)
{
	return !operator==(other);
}

template<typename T,typename R>
void basic_metric_iterator<T,R>::normalize()
{
	if(fI==metric->refs.end())
		normal=false;
}
#endif
