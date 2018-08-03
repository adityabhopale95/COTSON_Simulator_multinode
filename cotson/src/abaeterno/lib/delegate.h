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

// $Id: delegate.h 11 2010-01-08 13:20:58Z paolofrb $
#ifndef DELEGATE_H
#define DELEGATE_H

//
// Simple delegate class for void member function with 1 argument
// Inspired by the "Impossibly Fast C++ Delegates" discussion at
// http://www.codeproject.com/cpp/ImpossiblyFastCppDelegate.asp
//
// For simple cases, it provides a slightly faster alternative to 
// the combination of boost::bind and boost::function
//
// Example usage:
//
//     class SomeClass {
//     public:
//         void a_func(int);
//     };
//     ...
//     SomeClass *objptr;
//     delegate<int> d = 
//         delegate<int>::bind<SomeClass,&SomeClass::a_func>(objptr);
//     ...
//     d(10); // Calls "obj->a_func(10);"
//
//
template <typename Arg1> 
class delegate
{
private:
    typedef void (*Func)(void* obj, Arg1);

public:
    delegate():obj(0),stub(0){}

    template <class T> 
	delegate(T* o, Func s):obj(o),stub(s){}

    template <class T, void (T::*MemFun)(Arg1)>
    static delegate bind(T* obj)
    {
		return delegate(obj, &func<T,MemFun>);
    }

	inline void operator()(Arg1 a1) const
    {
        return (*stub)(obj, a1);
    }

private:
    void* obj;
    Func stub;

    template <class T, void (T::*MemFun)(Arg1)>
    inline static void func(void* obj, Arg1 a1)
    {
		(static_cast<T*>(obj)->*MemFun)(a1);
    }
};
#endif
