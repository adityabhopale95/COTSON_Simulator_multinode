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
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "memory_interface.h"
#include "liboptions.h"

namespace Memory{
namespace Protocol{

typedef std::vector<Interface*> Intf_vector;
typedef boost::shared_ptr<Interface> Shared;

class Protocol : public metric
{
	public:
		Protocol() {}
		virtual ~Protocol() {}
		virtual MemState
			read(Interface*, Shared&, Intf_vector&,const Access& m,uint64_t tstamp,Trace& mt,MOESI_state ms)=0;
		virtual MemState
			readx(Interface*, Shared&, Intf_vector&, const Access& m,uint64_t tstamp,Trace& mt,MOESI_state ms)=0;
		virtual MemState
			write(Interface*, Shared&, Intf_vector& ,const Access& m,uint64_t tstamp,Trace& mt,MOESI_state ms)=0;

		MOESI_state state(Intf_vector& prevs, const Access& m, uint64_t tstamp);
		void invalidate(Intf_vector& prevs, const Access& m,uint64_t tstamp,const Trace& mt,MOESI_state ms);
	//private:
	//	uint64_t snoops_;
	//	uint64_t invalidates_;
};

}
}
#endif
	       
