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

// $Id: memory_access.cpp 11 2010-01-08 13:20:58Z paolofrb $
#include "abaeterno_config.h"
#include "memory_access.h"

using namespace std;
using namespace boost;

ostream& operator<<(ostream& o,const Memory::Access& m)
{
	if(m.virt==m.phys)
		return o << format("0x%016x [%02d]") 
			% m.virt % m.length;
	else
		return o << format("0x%016x 0x%016x [%02d]") 
			% m.virt % m.phys % m.length;
}

DumpGzip& operator<<(DumpGzip& gz,const Memory::Access& m)
{
	gz << m.phys << m.virt << m.length;
	return gz;
}

ReadGzip& operator>>(ReadGzip& gz,Memory::Access& m)
{
	gz >> m.phys >> m.virt >> m.length;
	return gz;
}
