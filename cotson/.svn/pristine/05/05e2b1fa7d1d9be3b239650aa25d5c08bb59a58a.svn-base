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
#ifndef CALLBACKS_H
#define CALLBACKS_H

typedef boost::function<void(void)> Func0;

class Call : public boost::noncopyable
{
	std::vector<Func0> funcs;

	public:
				
	static Call& init()      { static Call init;      return init; }
	static Call& terminate() { static Call terminate; return terminate; }
	static Call& start()     { static Call start;     return start; }
	static Call& stop()      { static Call stop;      return stop; }
	static Call& prepare()   { static Call prepare;   return prepare; }

	void add(Func0& p) {
		funcs.push_back(p);
	}

	void operator()() {
		for(std::vector<Func0>::iterator i=funcs.begin();i!=funcs.end();++i)
			(*i)();
	}
};

struct run_at_init
{
	run_at_init(void(*p)())
	{
		Func0 f(p);
		Call::init().add(f);
	}
};

struct run_at_terminate
{
	run_at_terminate(void(*p)())
	{
		Func0 f(p);
		Call::terminate().add(f);
	}
};

struct run_at_start
{
	run_at_start(void(*p)())
	{
		Func0 f(p);
		Call::start().add(f);
	}
};

struct run_at_stop
{
	run_at_stop(void(*p)())
	{
		Func0 f(p);
		Call::stop().add(f);
	}
};

struct run_at_prepare
{
	run_at_prepare(void(*p)())
	{
		Func0 f(p);
		Call::prepare().add(f);
	}
};
#endif
