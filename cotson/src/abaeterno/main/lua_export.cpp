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

// $Id: lua_export.cpp 11 2010-01-08 13:20:58Z paolofrb $
#include "abaeterno_config.h"
#include "lua_export.h"
#include "liboptions.h"
#include "cpu.h"
#include "machine.h"

luabind::scope definition()
{
	luabind::scope a=(luabind::def("cpus", &Machine::cpus),
		luabind::def("get_cpu", &Machine::cpu),
		luabind::def("disks", &Machine::disks),
		luabind::def("get_disk", &Machine::disk),
		luabind::def("nics", &Machine::nics),
		luabind::def("get_nic", &Machine::nic));

	typedef StaticSet<luabind::scope (*)()> Funcs;
	for(Funcs::iterator i=Funcs::begin();i!=Funcs::end(); ++i)
		a=a,(*i)();

	return 
	    luabind::class_<Cpu>("Cpu")
		    .def("timer",             &Cpu::timer)
		    .def("instruction_cache", &Cpu::instruction_cache)
		    .def("data_cache",        &Cpu::data_cache)
		    .def("instruction_tlb",   &Cpu::instruction_tlb)
		    .def("data_tlb",          &Cpu::data_tlb),
	    luabind::class_<Disk>("Disk")
		    .def("timer",             &Disk::timer),
	    luabind::class_<Nic>("Nic")
		    .def("timer",             &Nic::timer),
	    luabind::class_<Memory::lua>("Memory")
		    .def("next", &Memory::lua::next),
        a;
}

