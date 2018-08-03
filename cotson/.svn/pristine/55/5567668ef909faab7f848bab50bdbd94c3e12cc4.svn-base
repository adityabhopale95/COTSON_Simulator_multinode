// (C) Copyright 2010 Hewlett-Packard Development Company, L.P.
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
// Assumes compiler support for variadic macros
#ifndef LOG_H
#define LOG_H

#define __str(x) #x
#define ___str(x) __str(x)
extern boost::mutex io_mutex;
#define LOG1(...) if (verbose_>0) { boost::mutex::scoped_lock lk(io_mutex); _LOG(__VA_ARGS__); }
#define LOG2(...) if (verbose_>1) { boost::mutex::scoped_lock lk(io_mutex); _LOG(__VA_ARGS__); }
#define LOG3(...) if (verbose_>2) { boost::mutex::scoped_lock lk(io_mutex); _LOG(__VA_ARGS__); }
template<typename A> 
static inline void _LOG(const A&a)
{std::cout<<a<<std::endl;}
template<typename A,typename B> 
static inline void _LOG(const A&a,const B&b)
{std::cout<<a<<' '<<b<<std::endl;}
template<typename A,typename B,typename C> 
static inline void _LOG(const A&a,const B&b,const C&c)
{std::cout<<a<<' '<<b<<' '<<c<<std::endl;}
template<typename A,typename B,typename C,typename D> 
static inline void _LOG(const A&a,const B&b,const C&c,const D&d)
{std::cout<<a<<' '<<b<<' '<<c<<' '<<d<<std::endl;}
template<typename A,typename B,typename C,typename D,typename E> 
static inline void _LOG(const A&a,const B&b,const C&c,const D&d,const E&e)
{std::cout<<a<<' '<<b<<' '<<c<<' '<<d<<' '<<e<<std::endl;}
template<typename A,typename B,typename C,typename D,typename E,typename F> 
static inline void _LOG(const A&a,const B&b,const C&c,const D&d,const E&e,const F&f)
{std::cout<<a<<' '<<b<<' '<<c<<' '<<d<<' '<<e<<' '<<f<<std::endl;}
template<typename A,typename B,typename C,typename D,typename E,typename F,typename G> 
static inline void _LOG(const A&a,const B&b,const C&c,const D&d,const E&e,const F&f,const G&g)
{std::cout<<a<<' '<<b<<' '<<c<<' '<<d<<' '<<e<<' '<<f<<' '<<g<<std::endl;}
template<typename A,typename B,typename C,typename D,typename E,typename F,typename G,typename H> 
static inline void _LOG(const A&a,const B&b,const C&c,const D&d,const E&e,const F&f,const G&g,const H&h)
{std::cout<<a<<' '<<b<<' '<<c<<' '<<d<<' '<<e<<' '<<f<<' '<<g<<' '<<h<<std::endl;}
template<typename A,typename B,typename C,typename D,typename E,typename F,typename G,typename H,typename I> 
static inline void _LOG(const A&a,const B&b,const C&c,const D&d,const E&e,const F&f,const G&g,const H&h,const I&i)
{std::cout<<a<<' '<<b<<' '<<c<<' '<<d<<' '<<e<<' '<<f<<' '<<g<<' '<<h<<' '<<i<<std::endl;}
template<typename A,typename B,typename C,typename D,typename E,typename F,typename G,typename H,typename I,typename J> 
static inline void _LOG(const A&a,const B&b,const C&c,const D&d,const E&e,const F&f,const G&g,const H&h,const I&i,const J&j)
{std::cout<<a<<' '<<b<<' '<<c<<' '<<d<<' '<<e<<' '<<f<<' '<<g<<' '<<h<<' '<<i<<' '<<j<<std::endl;}
#endif

