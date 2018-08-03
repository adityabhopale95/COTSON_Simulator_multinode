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
#ifndef ERROR_H
#define ERROR_H

#include "liboptions.h"

#define __str(x) #x
#define ___str(x) __str(x)

#define ERROR(...)		do { _PRINT("** ERROR: " __FILE__ ":" ___str(__LINE__) ", ",__VA_ARGS__); std::cerr << kill; } while(0)
#define WARNING(...)	do { _PRINT("** WARNING: " __FILE__ ":" ___str(__LINE__) ", ",__VA_ARGS__);  } while(0)

#ifndef NDEBUG
	#define ERROR_IF(cond, ...)		if ((cond)) ERROR(__VA_ARGS__)
	#define WARNING_IF(cond, ...)	if ((cond)) WARNING(__VA_ARGS__)
#else
	#define ERROR_IF(...)	do {;} while(0)
	#define WARNING_IF(...)	do {;} while(0)
#endif	



// Helper functions

template<typename A> 
static inline void _PRINT(const char *s,const A&a)
{std::cerr<<s<<a<<std::endl;}
template<typename A,typename B> 
static inline void _PRINT(const char *s,const A&a,const B&b)
{std::cerr<<s<<a<<' '<<b<<std::endl;}
template<typename A,typename B,typename C> 
static inline void _PRINT(const char *s,const A&a,const B&b,const C&c)
{std::cerr<<s<<a<<' '<<b<<' '<<c<<std::endl;}
template<typename A,typename B,typename C,typename D> 
static inline void _PRINT(const char *s,const A&a,const B&b,const C&c,const D&d)
{std::cerr<<s<<a<<' '<<b<<' '<<c<<' '<<d<<std::endl;}
template<typename A,typename B,typename C,typename D,typename E> 
static inline void _PRINT(const char *s,const A&a,const B&b,const C&c,const D&d,const E&e)
{std::cerr<<s<<a<<' '<<b<<' '<<c<<' '<<d<<' '<<e<<std::endl;}
template<typename A,typename B,typename C,typename D,typename E,typename F> 
static inline void _PRINT(const char *s,const A&a,const B&b,const C&c,const D&d,const E&e,const F&f)
{std::cerr<<s<<a<<' '<<b<<' '<<c<<' '<<d<<' '<<e<<' '<<f<<std::endl;}
template<typename A,typename B,typename C,typename D,typename E,typename F,typename G> 
static inline void _PRINT(const char *s,const A&a,const B&b,const C&c,const D&d,const E&e,const F&f,const G&g)
{std::cerr<<s<<a<<' '<<b<<' '<<c<<' '<<d<<' '<<e<<' '<<f<<' '<<g<<std::endl;}
template<typename A,typename B,typename C,typename D,typename E,typename F,typename G,typename H> 
static inline void _PRINT(const char *s,const A&a,const B&b,const C&c,const D&d,const E&e,const F&f,const G&g,const H&h)
{std::cerr<<s<<a<<' '<<b<<' '<<c<<' '<<d<<' '<<e<<' '<<f<<' '<<g<<' '<<h<<std::endl;}
template<typename A,typename B,typename C,typename D,typename E,typename F,typename G,typename H,typename I> 
static inline void _PRINT(const char *s,const A&a,const B&b,const C&c,const D&d,const E&e,const F&f,const G&g,const H&h,const I&i)
{std::cerr<<s<<a<<' '<<b<<' '<<c<<' '<<d<<' '<<e<<' '<<f<<' '<<g<<' '<<h<<' '<<i<<std::endl;}
template<typename A,typename B,typename C,typename D,typename E,typename F,typename G,typename H,typename I,typename J> 
static inline void _PRINT(const char *s,const A&a,const B&b,const C&c,const D&d,const E&e,const F&f,const G&g,const H&h,const I&i,const J&j)
{std::cerr<<s<<a<<' '<<b<<' '<<c<<' '<<d<<' '<<e<<' '<<f<<' '<<g<<' '<<h<<' '<<i<<' '<<j<<std::endl;}


#endif

