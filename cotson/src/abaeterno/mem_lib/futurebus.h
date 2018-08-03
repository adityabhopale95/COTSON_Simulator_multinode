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

// $Id: futurebus.h 11 2010-01-08 13:20:58Z paolofrb $
#ifndef FUTUREBUS_H
#define FUTUREBUS_H

#include "memory_interface.h"
#include "liboptions.h"

#include "protocol.h"

namespace Memory{
namespace Protocol{

class FUTUREBUS : public Protocol 
{
	public:
		FUTUREBUS() : Protocol() {}
		MemState read(Interface*, Shared&, Intf_vector& prevs, 
				const Access&,uint64_t,Trace&,MOESI_state);
		MemState readx(Interface*, Shared&, Intf_vector& prevs, 
				const Access&,uint64_t,Trace&,MOESI_state);
		MemState write(Interface*, Shared&, Intf_vector& prevs, 
				const Access&,uint64_t ,Trace&,MOESI_state);
};

}
}

#endif
