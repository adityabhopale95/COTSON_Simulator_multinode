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

// $Id: simple_protocol.h 11 2010-01-08 13:20:58Z paolofrb $
#ifndef SIMPLE_PROTOCOL_H
#define SIMPLE_PROTOCOL_H

#include "memory_interface.h"
#include "liboptions.h"
#include "protocol.h"

//This is a Valid/Invalid protocol suited for write-through caches
//Valid is SHARED
//Invalid is INVALID

namespace Memory{
namespace Protocol{

class SIMPLE : public Protocol 
{
	public:
		SIMPLE() : Protocol() {}
		MemState
			read(Interface*, Shared&, Intf_vector&, 
				const Access&,uint64_t,Trace& ,MOESI_state);
		MemState
			readx(Interface*, Shared&, Intf_vector&, 
				const Access&,uint64_t,Trace&,MOESI_state);
		MemState
			write(Interface*, Shared&, Intf_vector&, 
				const Access&,uint64_t,Trace&,MOESI_state);
};

}
}

#endif
