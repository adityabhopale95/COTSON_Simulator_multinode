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

#include "mem_bintracer.h"
#include "cotson.h"

using namespace std;
using namespace boost;

namespace Memory {

registerClass<Interface,MemBinTracer> mem_bintracer_c("mem_bintracer");

MemBinTracer::MemBinTracer(const Parameters& p) :
	gz(p.get<string>("trace_file", "/tmp/mem_trace.tar.gz")),
	linesize_(p.get<uint32_t>("linesize", "64", false)) // no track
{
	name=p.get<string>("name");
	latency=p.get<uint32_t>("latency");

	add("read", read_);
	add("write", write_);

	clear_metrics();
	
	std::cout << "Start tracing..." << std::endl;
}

MemState MemBinTracer::read(const Access& m, uint64_t tstamp, Trace& mt, MOESI_state ms) 
{
	read_++;
	TraceMem trace(true, m.phys, tstamp, m.length, Cotson::nanos());
	
	//cout << Name() << " - " << trace << endl;
	gz << trace;
	return  next->read(m,tstamp,mt,ms);;
}

MemState MemBinTracer::readx(const Access& m, uint64_t tstamp, Trace& mt, MOESI_state ms) 
{
	if (ms != NOT_FOUND)
		return MemState(0,MODIFIED);  // write hit, returns dirty

	mt.add(this,READ);
	read_++;
	TraceMem trace(true, m.phys, tstamp, m.length, Cotson::nanos());
//	cout << "Access at " << trace.getNanos() << "ns " << trace.getTstamp() << "  cycles mem: " <<trace.getPhys()
//		<< " read: "<< trace.isRead() << " - " << trace.getLength() << " bytes" << endl;
	gz << trace;
	return MemState(latency,EXCLUSIVE);
}

MemState MemBinTracer::write(const Access& m, uint64_t tstamp, Trace& mt, MOESI_state ms) 
{
	write_++;

	TraceMem trace(false, m.phys, tstamp, m.length, Cotson::nanos());

	//cout << Name() << " - " << trace << endl;
	gz << trace;
	
 	return next->write(m,tstamp,mt,ms);
}

MOESI_state MemBinTracer::state(const Access& m, uint64_t tstamp) 
{
	// call state in prevs
	//cout << "STATE" << endl; 
	for(vector<Interface*>::iterator i=prevs.begin();i!=prevs.end();++i)
	{
		MOESI_state pst=(*i)->state(m,tstamp);
		if(pst!=INVALID)
			return pst;
	}
	return INVALID;

}

void MemBinTracer::invalidate(const Access& m, uint64_t tstamp, const Trace& mt, MOESI_state ms) 
{
	//call invalidates in prevs
	//cout << "INVALIDATE" << endl;
	vector<Interface*>::iterator i = prevs.begin();
	for(; i != prevs.end(); ++i)
		(*i)->invalidate(m,tstamp,mt,ms);
}
}

ostream& operator<<(ostream& o,const Memory::TraceMem& m)
{
	if(m.phys>>63?true:false)
		o << "Read( ";
	else
		o << "Write( ";
	
	o<< hex << ((uint64_t) m.phys & 0x7fffffffffffffff) << dec << " , " << m.tstamp << " , " << m.length << " , " << m.nanos << " )";
	return o;
}

DumpGzip& operator<<(DumpGzip& gz,const Memory::TraceMem& m)
{
	gz << m.phys << m.tstamp << m.length << m.nanos;
	return gz;
}

ReadGzip& operator>>(ReadGzip& gz,Memory::TraceMem& m)
{
	gz >> m.phys >> m.tstamp >> m.length >> m.nanos;
	return gz;
}
