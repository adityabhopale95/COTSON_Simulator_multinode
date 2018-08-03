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
#include "protocol.h"

using namespace std;
using namespace boost;

namespace Memory{
namespace Protocol{
MOESI_state Protocol::state(Intf_vector& prevs, const Access& m, uint64_t tstamp)
{
	for(vector<Interface*>::iterator i=prevs.begin();i!=prevs.end();++i)
	{
		MOESI_state pst=(*i)->state(m,tstamp);
		if(pst!=INVALID)
			return pst;
	}
	return INVALID;
}

void Protocol::invalidate(Intf_vector& prevs, const Access& m,uint64_t tstamp,const Trace& mt,MOESI_state ms)
{
	for(vector<Interface*>::iterator i=prevs.begin();i!=prevs.end();++i)
	{
		(*i)->invalidate(m,tstamp,mt,ms);
	}
}

}
}
