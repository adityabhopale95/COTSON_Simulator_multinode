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
#include "abaeterno_config.h"
#include "memory_interface.h"
#include "error.h"

using namespace boost;


namespace Memory {
void Interface::setNext(Shared n)
{
	next=n;
	next->setPrev(this);
	add(next->Name()+".",*next);
}

void Interface::setPrev(Interface* p)
{
	prevs.push_back(p);
}

} // namespace Memory

std::ostream& operator<<(std::ostream& os,const Memory::MOESI_state m)
{
	switch(m) 
	{
	    case Memory::NOT_FOUND: os << "NOTFOUND"; break;
	    case Memory::MODIFIED:  os << "M"; break;
	    case Memory::OWNER:     os << "O"; break;
	    case Memory::EXCLUSIVE: os << "E"; break;
	    case Memory::SHARED:    os << "S"; break;
	    case Memory::INVALID:   os << "I"; break;
		case Memory::MOESI_STATES_: ERROR("invalid MOESI state");
	}
	return os;
}

#include <cpu_timer.h>
std::ostream& operator<<(std::ostream& os,const Memory::Trace& mt)
{
    const CpuTimer* cpu = mt.getCpu();
	os << "Trace:[";
	if (cpu)
	    os << "cpu" << cpu->id();
    for (int i=0;i<mt.size();++i) 
    {
		const Memory::Interface *mi = mt.getMem(i);
        os <<  "(" << (mi ? mi->Name() : "(null)");
	    switch(mt.getType(i))
	    {
	        case Memory::ND:    os << " ND"; break;
	        case Memory::READ:  os << " R"; break;
	        case Memory::READX: os << " RX"; break;
	        case Memory::WRITE: os << " W"; break;
	    }
	    os << ")";
    }
	os << "]";
    return os;
}

