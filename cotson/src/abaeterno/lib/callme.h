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

// $Id: callme.h 11 2010-01-08 13:20:58Z paolofrb $
#ifndef TIMEWHEEL_H
#define TIMEWHEEL_H

class CallMe 
{
public:
	typedef boost::function<void(uint64_t)> Callback;
		
	static void later(uint64_t,Callback);
	static void hey(uint64_t);

private:
	CallMe() {}

	inline static CallMe& get()
	{
		static CallMe singleton;
		return singleton;
	}

	static void subscribe();

	typedef std::multimap<uint64_t,Callback> Wheel;
	Wheel wheel;
};
#endif
