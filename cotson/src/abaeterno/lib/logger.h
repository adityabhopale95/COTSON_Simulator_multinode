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
#ifndef LOGGER_H
#define LOGGER_H

#ifndef _DEBUG_THIS_

#ifdef NDEBUG
#define LOG(...) /* empty */
#else
#include "selector.h"
#include <boost/io/ios_state.hpp>

namespace Logger
{
	template<typename T1> 
	struct Logger1 : public Selector
	{
		T1 msg1;
		Logger1(T1 m1) : msg1(m1) {}
		virtual void Do(const char* file, int line) const
		{
			if(shouldI(file,line))
			{
				boost::io::ios_all_saver ias(std::cout);
				std::cout << name(file,line) << " " << msg1 << std::endl;
			}
		}
	};
	
	template<typename T1,typename T2> 
	struct Logger2 : public Selector
	{
		T1 msg1;
		T2 msg2;
		Logger2(T1 m1,T2 m2) : msg1(m1),msg2(m2) {}
		virtual void Do(const char* file, int line) const
		{
			if(shouldI(file,line))
			{
				boost::io::ios_all_saver ias(std::cout);
				std::cout << name(file,line) << " " << msg1 
					<< " " << msg2 << std::endl;
			}
		}
	};

	template<typename T1,typename T2,typename T3> 
	struct Logger3 : public Selector
	{
		T1 msg1;
		T2 msg2;
		T3 msg3;
		Logger3(T1 m1,T2 m2,T3 m3) : msg1(m1),msg2(m2),msg3(m3) {}
		virtual void Do(const char* file, int line) const
		{
			if(shouldI(file,line))
			{
				boost::io::ios_all_saver ias(std::cout);
				std::cout << name(file,line) << " " << msg1 
					<< " " << msg2 
					<< " " << msg3 << std::endl;
			}
		}
	};
	
	template<typename T1,typename T2,typename T3,typename T4,typename T5,
		typename T6,typename T7,typename T8,typename T9,typename T10> 
	struct LoggerMany : public Selector
	{
		T1 msg1; T2 msg2; T3 msg3; T4 msg4; T5 msg5;
		T6 msg6; T7 msg7; T8 msg8; T9 msg9; T10 msg10;
		LoggerMany(T1 m1,T2 m2,T3 m3,T4 m4,T5 m5,
			T6 m6,T7 m7,T8 m8,T9 m9,T10 m10) :
			msg1(m1), msg2(m2), msg3(m3), msg4(m4), msg5(m5), 
			msg6(m6), msg7(m7), msg8(m8), msg9(m9), msg10(m10) {}
		virtual void Do(const char* file, int line) const
		{
			if(shouldI(file,line))
			{
				boost::io::ios_all_saver ias(std::cout);
				std::cout << name(file,line) << " " << msg1 
					<< " " << msg2 
					<< " " << msg3 
					<< " " << msg4 
					<< " " << msg5 
					<< " " << msg6 
					<< " " << msg7 
					<< " " << msg8 
					<< " " << msg9 
					<< " " << msg10 << std::endl;
			}
		}
	};

	template<typename T1>
	static Logger1<T1> Make(T1 m1)
	{
		return Logger1<T1>(m1);
	}
	
	template<typename T1,typename T2>
	static Logger2<T1,T2> Make(T1 m1,T2 m2)
	{
		return Logger2<T1,T2>(m1,m2);
	}

	template<typename T1,typename T2,typename T3>
	static Logger3<T1,T2,T3> Make(T1 m1,T2 m2,T3 m3)
	{
		return Logger3<T1,T2,T3>(m1,m2,m3);
	}

	typedef const char* cc;

	// version with 4 
	template<typename T1, typename T2, typename T3, typename T4>
	static LoggerMany<T1,T2,T3,T4,cc,cc,cc,cc,cc,cc> 
		Make(T1 m1,T2 m2,T3 m3,T4 m4)
	{
		return LoggerMany<T1,T2,T3,T4,cc,cc,cc,cc,cc,cc> 
			(m1,m2,m3,m4,"","","","","","");
	}

	// version with 5
	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	static LoggerMany<T1,T2,T3,T4,T5,cc,cc,cc,cc,cc> 
		Make(T1 m1,T2 m2,T3 m3,T4 m4,T5 m5)
	{
		return LoggerMany<T1,T2,T3,T4,T5,cc,cc,cc,cc,cc>
			(m1,m2,m3,m4,m5,"","","","","");
	}

	// version with 6
	template<typename T1, typename T2, typename T3, typename T4, typename T5,
		typename T6>
	static LoggerMany<T1,T2,T3,T4,T5,T6,cc,cc,cc,cc> 
		Make(T1 m1,T2 m2,T3 m3,T4 m4,T5 m5,T6 m6)
	{
		return LoggerMany<T1,T2,T3,T4,T5,T6,cc,cc,cc,cc>
			(m1,m2,m3,m4,m5,m6,"","","","");
	}
	
	// version with 7 
	template<typename T1, typename T2, typename T3, typename T4, typename T5,
		typename T6, typename T7>
	static LoggerMany<T1,T2,T3,T4,T5,T6,T7,cc,cc,cc> 
		Make(T1 m1,T2 m2,T3 m3,T4 m4,T5 m5,T6 m6,T7 m7)
	{
		return LoggerMany<T1,T2,T3,T4,T5,T6,T7,cc,cc,cc> 
			(m1,m2,m3,m4,m5,m6,m7,"","","");
	}

	// version with 8
	template<typename T1, typename T2, typename T3, typename T4, typename T5,
		typename T6, typename T7, typename T8>
	static LoggerMany<T1,T2,T3,T4,T5,T6,T7,T8,cc,cc>
		Make(T1 m1, T2 m2, T3 m3, T4 m4, T5 m5, T6 m6, T7 m7,T8 m8)
	{
		return LoggerMany<T1,T2,T3,T4,T5,T6,T7,T8,cc,cc>
			(m1,m2,m3,m4,m5,m6,m7,m8,"","");
	}

	// version with 9 
	template<typename T1, typename T2, typename T3, typename T4, typename T5,
		typename T6, typename T7, typename T8, typename T9>
	static LoggerMany<T1,T2,T3,T4,T5,T6,T7,T8,T9,cc> 
		Make(T1 m1,T2 m2,T3 m3,T4 m4,T5 m5,T6 m6,T7 m7,T8 m8,T9 m9)
	{
		return LoggerMany<T1,T2,T3,T4,T5,T6,T7,T8,T9,cc>
			(m1,m2,m3,m4,m5,m6,m7,m8,m9,"");
	}

	// version with 10 
	template<typename T1, typename T2, typename T3, typename T4, typename T5,
		typename T6, typename T7, typename T8, typename T9, typename T10>
	static LoggerMany<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10> 
		Make(T1 m1,T2 m2,T3 m3,T4 m4,T5 m5,T6 m6,T7 m7,T8 m8,T9 m9,T10 m10)
	{
		return LoggerMany<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>
			(m1,m2,m3,m4,m5,m6,m7,m8,m9,m10);
	}
}

#define LOG(...) COND_SELECTOR(Logger,__VA_ARGS__)

namespace Setter
{
	struct Setter : public Selector
	{
		bool yes;
		Setter(bool b) : yes(b) {}
		virtual void Do(const char* file, int line) const
		{
			if(yes)
				on(name(file,line));
		}
	};
	
	template <class Boolean>
	static Setter Make(Boolean flag)
	{
		return Setter(flag);
	}
}

#define SET SELECTOR(Setter)

namespace Clearer
{
	struct Clearer : public Selector
	{
		bool yes;
		Clearer(bool b) : yes(b) {}
		virtual void Do(const char* file, int line) const
		{
			if(yes)
				off(name(file,line));
		}
	};
	
	template <class Boolean>
	static Clearer Make(Boolean flag)
	{
		return Clearer(flag);
	}
}

#define RESET SELECTOR(Clearer)
#endif /* _NDEBUG_ */

#else // _DEBUG_THIS_

// Simpler version, always on
#define __str(x) #x
#define ___str(x) __str(x)
#define LOG(...) _LOG(__FILE__ ":" ___str(__LINE__) " ",__VA_ARGS__)
template<typename A> 
static inline void _LOG(const char *s,const A&a)
{std::cout<<s<<a<<std::endl;}
template<typename A,typename B> 
static inline void _LOG(const char *s,const A&a,const B&b)
{std::cout<<s<<a<<' '<<b<<std::endl;}
template<typename A,typename B,typename C> 
static inline void _LOG(const char *s,const A&a,const B&b,const C&c)
{std::cout<<s<<a<<' '<<b<<' '<<c<<std::endl;}
template<typename A,typename B,typename C,typename D> 
static inline void _LOG(const char *s,const A&a,const B&b,const C&c,const D&d)
{std::cout<<s<<a<<' '<<b<<' '<<c<<' '<<d<<std::endl;}
template<typename A,typename B,typename C,typename D,typename E> 
static inline void _LOG(const char *s,const A&a,const B&b,const C&c,const D&d,const E&e)
{std::cout<<s<<a<<' '<<b<<' '<<c<<' '<<d<<' '<<e<<std::endl;}
template<typename A,typename B,typename C,typename D,typename E,typename F> 
static inline void _LOG(const char *s,const A&a,const B&b,const C&c,const D&d,const E&e,const F&f)
{std::cout<<s<<a<<' '<<b<<' '<<c<<' '<<d<<' '<<e<<' '<<f<<std::endl;}
template<typename A,typename B,typename C,typename D,typename E,typename F,typename G> 
static inline void _LOG(const char *s,const A&a,const B&b,const C&c,const D&d,const E&e,const F&f,const G&g)
{std::cout<<s<<a<<' '<<b<<' '<<c<<' '<<d<<' '<<e<<' '<<f<<' '<<g<<std::endl;}
template<typename A,typename B,typename C,typename D,typename E,typename F,typename G,typename H> 
static inline void _LOG(const char *s,const A&a,const B&b,const C&c,const D&d,const E&e,const F&f,const G&g,const H&h)
{std::cout<<s<<a<<' '<<b<<' '<<c<<' '<<d<<' '<<e<<' '<<f<<' '<<g<<' '<<h<<std::endl;}
template<typename A,typename B,typename C,typename D,typename E,typename F,typename G,typename H,typename I> 
static inline void _LOG(const char *s,const A&a,const B&b,const C&c,const D&d,const E&e,const F&f,const G&g,const H&h,const I&i)
{std::cout<<s<<a<<' '<<b<<' '<<c<<' '<<d<<' '<<e<<' '<<f<<' '<<g<<' '<<h<<' '<<i<<std::endl;}
template<typename A,typename B,typename C,typename D,typename E,typename F,typename G,typename H,typename I,typename J> 
static inline void _LOG(const char *s,const A&a,const B&b,const C&c,const D&d,const E&e,const F&f,const G&g,const H&h,const I&i,const J&j)
{std::cout<<s<<a<<' '<<b<<' '<<c<<' '<<d<<' '<<e<<' '<<f<<' '<<g<<' '<<h<<' '<<i<<' '<<j<<std::endl;}

#endif /* DEBUG_THIS */

#endif

