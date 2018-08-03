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

// $Id: selector.h 721 2015-06-03 02:29:33Z paolofrb $
#ifndef SELECTOR_H
#define SELECTOR_H

class Selector
{
public:
    // this way we do not need to define the static variables
    // anywhere in a .cpp file
    static int& all()
    {
        // 0 means no debugging (checked in construction)
        // 1 means always (checked in shouldI)
        // 2 means only if selected by user 
        static int all_=1; 
        return all_;
    }
    
protected:
    static std::set<std::string>& allowed()
    {
        static std::set<std::string> allowed_;
        return allowed_;
    }
    
    static std::string name(const char* file,int line)
    {
        std::string n(file);
        n+=":";
        n+=boost::lexical_cast<std::string>(line);
        return n;
    }
    
    static bool shouldI(const char* file,int line)
    {
        if(all()==1)
            return true;
        if(allowed().find(file)!=allowed().end())
            return true;
        if(allowed().find(name(file,line))!=allowed().end())
            return true;
        return false;
    }

public:
    virtual void Do(const char* file,int line) const = 0 ;
    virtual ~Selector() {};

    static void on(const std::string& elem)  { allowed().insert(elem); }
    static void off(const std::string& elem) { allowed().erase(elem); }
};

#if defined(__GNUC__)
#define __UNUSED __attribute__((unused))
#else
#define __UNUSED /**/
#endif

#ifndef NDEBUG

    #define COND_SELECTOR(NMSPC,...) \
        if (false) ; else \
            if(!Selector::all()) ; else \
                struct Local { \
                  Local(const Selector& l) { \
                    l.Do(__FILE__,__LINE__); \
                  } \
                } __UNUSED _ (NMSPC ::Make(__VA_ARGS__))

    #define SELECTOR(NMSPC,...) \
        if (false) ; else \
            struct Local { \
              Local(const Selector& l) { \
                l.Do(__FILE__,__LINE__); \
              } \
            } __UNUSED _ (NMSPC ::Make(__VA_ARGS__))

#else

    #define COND_SELECTOR(NMSPC,...) \
        if (true) ; else \
            if(!Selector::all()) ; else \
                struct Local { \
                  Local(const Selector& l) { \
                    l.Do(__FILE__,__LINE__); \
                  } \
                } __UNUSED _ (NMSPC ::Make(__VA_ARGS__))

    #define SELECTOR(NMSPC,...) \
        if (true) ; else \
		    struct Local { \
              Local(const Selector& l) { \
                l.Do(__FILE__,__LINE__); \
              } \
           } __UNUSED _ (NMSPC ::Make(__VA_ARGS__))

#endif

#endif
