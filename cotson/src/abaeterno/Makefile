SUBDIRS= hw_model lib main mem_lib parsing sampler timer_cpu timer_dev simnow injector

LUAINC=/usr/include/lua5.1
LUALIB=-llua5.1

SUBINC = $(patsubst %,-I%,$(SUBDIRS))
INCLUDE = $(SUBINC) -I../common -I../libluabind -I.. -I$(LUAINC)
DEPINC = $(SUBINC) -I../common
XOPTS = -Werror
GCCVERS480CHECK = $(shell expr `gcc -dumpversion | cut -f1-2 -d.` \>= 4.8)
ifeq "$(GCCVERS480CHECK)" "1"  #RG small patch to compile
    XOPTS = -Werror -Wno-error=unused-but-set-variable -Wno-deprecated-declarations
    XXOPTS = -std=c++11
endif

SIMNOW_FLAGS = -Isimnow -I$(SIMNOW_DIR)/devel/monitor

all: abaeterno.so abaeterno.a aaInjector test

include  ../Make.conf

install:
	$(INSTALL) abaeterno.so $(PREFIX)/sbin

LINKOPTS = \
	-Wl,-rpath,./linuxlibs -shared \
	-Wl,--allow-multiple-definition 

AALIBS = ../common/libaacommon.a \
         ../disksim/lib/libdisksim-cotson.a \
         ../distorm/libdistorm.a \
         ../libluabind/libluabind.a \
         ../slirp/libslirp.a

ALL_AALIBS = -Wl,-whole-archive  $(AALIBS) -Wl,-no-whole-archive

INJLIBS = ../common/libaacommon.a \
         ../distorm/libdistorm.a \
         ../libluabind/libluabind.a

ALL_INJLIBS = -Wl,-whole-archive  $(INJLIBS) -Wl,-no-whole-archive

SHLIBS = -lpqxx -lsqlite3 $(LUALIB) $(THREAD_LIBS) -ldl -lz

################################################################################
MAIN = $(wildcard main/*.cpp)
TIMERCPU = $(wildcard timer_cpu/*.cpp)
MEM_LIB = $(wildcard mem_lib/*.cpp)
HW_MODEL = $(wildcard hw_model/*.cpp)
PARSING = $(wildcard parsing/*.cpp)
SAMPLER = $(wildcard sampler/*.cpp)
TIMERDEV = $(wildcard timer_dev/*.cpp)
UTILS = $(wildcard lib/*.cpp)

ABAETERNO = $(MAIN) $(PARSING) $(HW_MODEL) $(SAMPLER) $(TIMERDEV) \
	        $(TIMERCPU) $(MEM_LIB) $(UTILS) \
			simnow/proxy.o

INJECTOR  = $(TIMERCPU) $(MEM_LIB) $(UTILS)

AAOBJS = $(ABAETERNO:.cpp=.o) timer_dev/disksim_interface.o

abaeterno.a: $(AAOBJS) simnow/proxy.o
	@echo "Archiving $@";\
	$(RM) $@; $(AR) $@ $^

abaeterno.so: $(AAOBJS) simnow/proxy.o
	@echo "Linking $@";\
	$(CXX) $(LINKOPTS) -o $@ $^ $(ALL_AALIBS) $(SHLIBS)

INJOBJS = $(INJECTOR:.cpp=.o)
aaInjector: $(INJOBJS) injector/injector.o
	@echo "Linking $@";\
	$(CXX) -o $@ $^ $(ALL_INJLIBS) $(SHLIBS)

################################################################################
lib/version.o: lib/version.cpp lib/version.h lib/version_info.h

simnow/proxy.o: simnow/proxy.cpp
	@echo "$< ==> $@"; \
	$(CXX) $(CXXFLAGS) $(SIMNOW_FLAGS) -c $< -o $@

lib/version_info.h: always
	@echo "Creating lib/version.h"; \
	echo -n > $@ ; \
	echo -n "#define SVN_REVISION \""              >> $@ ; \
	svnversion -n .                                >> $@ ; \
	echo    "\""                                   >> $@ ; \
	echo -n "#define GXX_VERSION \""               >> $@ ; \
	$(CXX) --version|perl -n -e 'chomp;print;exit' >> $@ ; \
	echo    "\""                                   >> $@ ; \
	echo -n "#define BUILDHOST \""                 >> $@ ; \
	hostname | perl -n -e 'chomp;print;exit'       >> $@ ; \
	echo    "\""                                   >> $@ ; \
	echo -n "#define BUILDDATE \""                 >> $@ ; \
	date -u | perl -n -e 'chomp;print;exit'        >> $@ ; \
	echo    "\""                                   >> $@ ; \
	echo -n "#define BUILDUSER \""                 >> $@ ; \
	whoami | perl -n -e 'chomp;print;exit'         >> $@ ; \
	echo    "\""                                   >> $@ 

################################################################################
.PHONY: test clean_test
test: $(AAOBJS) test.unit/make.deps
	$(MAKE) -f test.unit/Makefile run \
	        TCXX="$(CXX)" TCXXFLAGS="$(CXXFLAGS) $(SIMNOW_FLAGS)" \
	        TLIBS="$(AALIBS) $(SHLIBS)"

test.unit/make.deps: always test.unit/testunitdep.pl
	perl test.unit/testunitdep.pl > $@

## Note: some hosts need -lboost_unit_test_framework

################################################################################
clean_test:
	@ echo "Clean test.unit"; \
	$(MAKE) -f test.unit/Makefile clean; \
	$(RM) test.unit/make.deps

clean: clean_test
	@echo "Clean abaeterno"; \
	$(RM) abaeterno.so abaeterno.a aaInjector injector/injector.o simnow/proxy.o \
	      $(AAOBJS) $(INJOBJS) $(TESTOBJS) \
	      Makefile.bak lib/version_info.h

distclean: clean

#########################
# Dependences
#########################
SRCS= $(MAIN) $(TIMERCPU) $(MEM_LIB) $(HW_MODEL) $(PARSING) $(SAMPLER) \
      $(TIMERDEV) $(UTILS) simnow/proxy.cpp injector/injector.cpp \
	  $(wildcard test.unit/*.cpp)

# DO NOT DELETE

main/abaeterno.o: lib/abaeterno_config.h main/abaeterno.h ../common/libmetric.h
main/abaeterno.o: ../common/metric.h ../common/tagged_metric.h
main/abaeterno.o: ../common/heartbeat.h ../common/aggregator.h
main/abaeterno.o: sampler/sampler.h hw_model/states.h main/profiler.h
main/abaeterno.o: lib/circbuf.h lib/hashmap.h lib/callme.h lib/logger.h
main/abaeterno.o: lib/selector.h main/interleaver.h lib/instructions.h
main/abaeterno.o: lib/instruction.h ../common/dump_gzip.h ../common/read_gzip.h
main/abaeterno.o: lib/memory_access.h lib/inline.h lib/simplevec.h
main/abaeterno.o: lib/opcodes.h hw_model/trace_needs.h lib/delegate.h
main/abaeterno.o: hw_model/state_observer.h ../common/liboptions.h
main/abaeterno.o: ../common/kill.h ../common/static_set.h
main/abaeterno.o: ../common/lua_register.h ../common/lua.hpp ../common/units.h
main/abaeterno.o: ../common/cast.h ../common/uint64_t_hash.h
main/abaeterno.o: ../common/parameters.h ../common/param_factory.h
main/abaeterno.o: ../common/options.h main/cotson.h lib/lua_export.h
main/abaeterno.o: ../common/error.h main/network_timing.h
main/abaeterno.o: ../common/timing_message.h hw_model/machine.h hw_model/cpu.h
main/abaeterno.o: timer_cpu/cpu_timer.h lib/memory_interface.h
main/abaeterno.o: lib/memory_trace.h lib/lua_helpers.h hw_model/nic.h
main/abaeterno.o: timer_dev/nic_timer.h hw_model/disk.h timer_dev/disk_timer.h
main/abaeterno.o: lib/callbacks.h lib/cpuid_call.h lib/custom_asm_regtab.h
main/abaeterno.o: ../common/fileutil.h
main/callme.o: lib/abaeterno_config.h lib/callme.h lib/logger.h lib/selector.h
main/callme.o: main/cotson.h ../common/metric.h
main/command_socket.o: lib/abaeterno_config.h main/command_socket.h
main/command_socket.o: ../common/error.h ../common/liboptions.h
main/command_socket.o: ../common/kill.h ../common/static_set.h
main/command_socket.o: ../common/lua_register.h ../common/lua.hpp
main/command_socket.o: ../common/units.h ../common/cast.h
main/command_socket.o: ../common/uint64_t_hash.h ../common/parameters.h
main/command_socket.o: ../common/param_factory.h ../common/options.h
main/command_socket.o: lib/callbacks.h main/network_timing.h
main/command_socket.o: ../common/timing_message.h ../common/libmetric.h
main/command_socket.o: ../common/metric.h ../common/tagged_metric.h
main/command_socket.o: ../common/heartbeat.h ../common/aggregator.h
main/interleaver.o: lib/abaeterno_config.h main/interleaver.h
main/interleaver.o: lib/instructions.h lib/instruction.h ../common/dump_gzip.h
main/interleaver.o: ../common/read_gzip.h lib/memory_access.h lib/inline.h
main/interleaver.o: lib/simplevec.h lib/opcodes.h lib/hashmap.h
main/interleaver.o: hw_model/trace_needs.h lib/delegate.h hw_model/states.h
main/interleaver.o: hw_model/state_observer.h ../common/liboptions.h
main/interleaver.o: ../common/kill.h ../common/static_set.h
main/interleaver.o: ../common/lua_register.h ../common/lua.hpp
main/interleaver.o: ../common/units.h ../common/cast.h
main/interleaver.o: ../common/uint64_t_hash.h ../common/parameters.h
main/interleaver.o: ../common/param_factory.h ../common/options.h lib/logger.h
main/interleaver.o: lib/selector.h main/cotson.h ../common/metric.h
main/interleaver.o: ../common/error.h lib/cpuid_call.h lib/custom_asm_regtab.h
main/lua_export.o: lib/abaeterno_config.h lib/lua_export.h ../common/lua.hpp
main/lua_export.o: ../common/liboptions.h ../common/kill.h
main/lua_export.o: ../common/static_set.h ../common/lua_register.h
main/lua_export.o: ../common/units.h ../common/cast.h ../common/uint64_t_hash.h
main/lua_export.o: ../common/parameters.h ../common/param_factory.h
main/lua_export.o: ../common/options.h hw_model/cpu.h timer_cpu/cpu_timer.h
main/lua_export.o: ../common/libmetric.h ../common/metric.h
main/lua_export.o: ../common/tagged_metric.h ../common/heartbeat.h
main/lua_export.o: ../common/aggregator.h lib/instruction.h
main/lua_export.o: ../common/dump_gzip.h ../common/read_gzip.h
main/lua_export.o: lib/memory_access.h lib/inline.h lib/simplevec.h
main/lua_export.o: lib/opcodes.h lib/hashmap.h hw_model/states.h
main/lua_export.o: hw_model/trace_needs.h lib/delegate.h lib/memory_interface.h
main/lua_export.o: lib/memory_trace.h hw_model/state_observer.h lib/logger.h
main/lua_export.o: lib/selector.h main/cotson.h lib/lua_helpers.h
main/lua_export.o: hw_model/machine.h hw_model/nic.h timer_dev/nic_timer.h
main/lua_export.o: hw_model/disk.h timer_dev/disk_timer.h
main/network_timing.o: main/network_timing.h lib/abaeterno_config.h
main/network_timing.o: ../common/timing_message.h ../common/libmetric.h
main/network_timing.o: ../common/metric.h ../common/tagged_metric.h
main/network_timing.o: ../common/heartbeat.h ../common/aggregator.h
main/network_timing.o: main/cotson.h lib/logger.h lib/selector.h
main/network_timing.o: ../common/liboptions.h ../common/kill.h
main/network_timing.o: ../common/static_set.h ../common/lua_register.h
main/network_timing.o: ../common/lua.hpp ../common/units.h ../common/cast.h
main/network_timing.o: ../common/uint64_t_hash.h ../common/parameters.h
main/network_timing.o: ../common/param_factory.h ../common/options.h
main/network_timing.o: lib/callme.h ../common/error.h lib/callbacks.h
main/network_timing.o: main/abaeterno.h sampler/sampler.h hw_model/states.h
main/network_timing.o: main/profiler.h lib/circbuf.h lib/hashmap.h
main/profiler.o: lib/abaeterno_config.h lib/logger.h lib/selector.h
main/profiler.o: main/profiler.h lib/circbuf.h lib/hashmap.h main/cotson.h
main/profiler.o: ../common/metric.h hw_model/machine.h hw_model/cpu.h
main/profiler.o: timer_cpu/cpu_timer.h ../common/libmetric.h
main/profiler.o: ../common/tagged_metric.h ../common/heartbeat.h
main/profiler.o: ../common/aggregator.h lib/instruction.h ../common/dump_gzip.h
main/profiler.o: ../common/read_gzip.h lib/memory_access.h lib/inline.h
main/profiler.o: lib/simplevec.h lib/opcodes.h hw_model/states.h
main/profiler.o: hw_model/trace_needs.h lib/delegate.h lib/memory_interface.h
main/profiler.o: lib/memory_trace.h hw_model/state_observer.h
main/profiler.o: ../common/liboptions.h ../common/kill.h ../common/static_set.h
main/profiler.o: ../common/lua_register.h ../common/lua.hpp ../common/units.h
main/profiler.o: ../common/cast.h ../common/uint64_t_hash.h
main/profiler.o: ../common/parameters.h ../common/param_factory.h
main/profiler.o: ../common/options.h lib/lua_helpers.h hw_model/nic.h
main/profiler.o: timer_dev/nic_timer.h hw_model/disk.h timer_dev/disk_timer.h
main/slirp_support.o: lib/abaeterno_config.h main/slirp_support.h
main/slirp_support.o: ../common/packet.h ../common/mac.h ../common/port.h
main/slirp_support.o: lib/logger.h lib/selector.h ../common/liboptions.h
main/slirp_support.o: ../common/kill.h ../common/static_set.h
main/slirp_support.o: ../common/lua_register.h ../common/lua.hpp
main/slirp_support.o: ../common/units.h ../common/cast.h
main/slirp_support.o: ../common/uint64_t_hash.h ../common/parameters.h
main/slirp_support.o: ../common/param_factory.h ../common/options.h
main/slirp_support.o: ../common/error.h lib/callbacks.h ../slirp/libslirp.h
timer_cpu/bandwidth.o: lib/abaeterno_config.h lib/cache.h
timer_cpu/bandwidth.o: ../common/liboptions.h ../common/kill.h
timer_cpu/bandwidth.o: ../common/static_set.h ../common/lua_register.h
timer_cpu/bandwidth.o: ../common/lua.hpp ../common/units.h ../common/cast.h
timer_cpu/bandwidth.o: ../common/uint64_t_hash.h ../common/parameters.h
timer_cpu/bandwidth.o: ../common/param_factory.h ../common/options.h
timer_cpu/bandwidth.o: lib/debug_mem.h lib/inline.h lib/hashmap.h lib/logger.h
timer_cpu/bandwidth.o: lib/selector.h ../common/error.h timer_cpu/cpu_timer.h
timer_cpu/bandwidth.o: ../common/libmetric.h ../common/metric.h
timer_cpu/bandwidth.o: ../common/tagged_metric.h ../common/heartbeat.h
timer_cpu/bandwidth.o: ../common/aggregator.h lib/instruction.h
timer_cpu/bandwidth.o: ../common/dump_gzip.h ../common/read_gzip.h
timer_cpu/bandwidth.o: lib/memory_access.h lib/simplevec.h lib/opcodes.h
timer_cpu/bandwidth.o: hw_model/states.h hw_model/trace_needs.h lib/delegate.h
timer_cpu/bandwidth.o: lib/memory_interface.h lib/memory_trace.h
timer_cpu/bandwidth.o: hw_model/state_observer.h main/cotson.h
timer_cpu/gzip_sink.o: lib/abaeterno_config.h timer_cpu/cpu_timer.h
timer_cpu/gzip_sink.o: ../common/libmetric.h ../common/metric.h
timer_cpu/gzip_sink.o: ../common/tagged_metric.h ../common/heartbeat.h
timer_cpu/gzip_sink.o: ../common/aggregator.h lib/instruction.h
timer_cpu/gzip_sink.o: ../common/dump_gzip.h ../common/read_gzip.h
timer_cpu/gzip_sink.o: lib/memory_access.h lib/inline.h lib/simplevec.h
timer_cpu/gzip_sink.o: lib/opcodes.h lib/hashmap.h hw_model/states.h
timer_cpu/gzip_sink.o: hw_model/trace_needs.h lib/delegate.h
timer_cpu/gzip_sink.o: lib/memory_interface.h lib/memory_trace.h
timer_cpu/gzip_sink.o: hw_model/state_observer.h ../common/liboptions.h
timer_cpu/gzip_sink.o: ../common/kill.h ../common/static_set.h
timer_cpu/gzip_sink.o: ../common/lua_register.h ../common/lua.hpp
timer_cpu/gzip_sink.o: ../common/units.h ../common/cast.h
timer_cpu/gzip_sink.o: ../common/uint64_t_hash.h ../common/parameters.h
timer_cpu/gzip_sink.o: ../common/param_factory.h ../common/options.h
timer_cpu/gzip_sink.o: lib/logger.h lib/selector.h main/cotson.h
timer_cpu/memory_tracer.o: lib/abaeterno_config.h lib/logger.h lib/selector.h
timer_cpu/memory_tracer.o: main/cotson.h ../common/metric.h
timer_cpu/memory_tracer.o: timer_cpu/cpu_timer.h ../common/libmetric.h
timer_cpu/memory_tracer.o: ../common/tagged_metric.h ../common/heartbeat.h
timer_cpu/memory_tracer.o: ../common/aggregator.h lib/instruction.h
timer_cpu/memory_tracer.o: ../common/dump_gzip.h ../common/read_gzip.h
timer_cpu/memory_tracer.o: lib/memory_access.h lib/inline.h lib/simplevec.h
timer_cpu/memory_tracer.o: lib/opcodes.h lib/hashmap.h hw_model/states.h
timer_cpu/memory_tracer.o: hw_model/trace_needs.h lib/delegate.h
timer_cpu/memory_tracer.o: lib/memory_interface.h lib/memory_trace.h
timer_cpu/memory_tracer.o: hw_model/state_observer.h ../common/liboptions.h
timer_cpu/memory_tracer.o: ../common/kill.h ../common/static_set.h
timer_cpu/memory_tracer.o: ../common/lua_register.h ../common/lua.hpp
timer_cpu/memory_tracer.o: ../common/units.h ../common/cast.h
timer_cpu/memory_tracer.o: ../common/uint64_t_hash.h ../common/parameters.h
timer_cpu/memory_tracer.o: ../common/param_factory.h ../common/options.h
timer_cpu/memory_tracer.o: mem_lib/storage_many.h mem_lib/storage.h lib/cache.h
timer_cpu/memory_tracer.o: lib/debug_mem.h mem_lib/cache_line.h
timer_cpu/simpoint_profile.o: lib/abaeterno_config.h timer_cpu/cpu_timer.h
timer_cpu/simpoint_profile.o: ../common/libmetric.h ../common/metric.h
timer_cpu/simpoint_profile.o: ../common/tagged_metric.h ../common/heartbeat.h
timer_cpu/simpoint_profile.o: ../common/aggregator.h lib/instruction.h
timer_cpu/simpoint_profile.o: ../common/dump_gzip.h ../common/read_gzip.h
timer_cpu/simpoint_profile.o: lib/memory_access.h lib/inline.h lib/simplevec.h
timer_cpu/simpoint_profile.o: lib/opcodes.h lib/hashmap.h hw_model/states.h
timer_cpu/simpoint_profile.o: hw_model/trace_needs.h lib/delegate.h
timer_cpu/simpoint_profile.o: lib/memory_interface.h lib/memory_trace.h
timer_cpu/simpoint_profile.o: hw_model/state_observer.h ../common/liboptions.h
timer_cpu/simpoint_profile.o: ../common/kill.h ../common/static_set.h
timer_cpu/simpoint_profile.o: ../common/lua_register.h ../common/lua.hpp
timer_cpu/simpoint_profile.o: ../common/units.h ../common/cast.h
timer_cpu/simpoint_profile.o: ../common/uint64_t_hash.h ../common/parameters.h
timer_cpu/simpoint_profile.o: ../common/param_factory.h ../common/options.h
timer_cpu/simpoint_profile.o: lib/logger.h lib/selector.h main/cotson.h
timer_cpu/timer0.o: lib/abaeterno_config.h lib/logger.h lib/selector.h
timer_cpu/timer0.o: timer_cpu/cpu_timer.h ../common/libmetric.h
timer_cpu/timer0.o: ../common/metric.h ../common/tagged_metric.h
timer_cpu/timer0.o: ../common/heartbeat.h ../common/aggregator.h
timer_cpu/timer0.o: lib/instruction.h ../common/dump_gzip.h
timer_cpu/timer0.o: ../common/read_gzip.h lib/memory_access.h lib/inline.h
timer_cpu/timer0.o: lib/simplevec.h lib/opcodes.h lib/hashmap.h
timer_cpu/timer0.o: hw_model/states.h hw_model/trace_needs.h lib/delegate.h
timer_cpu/timer0.o: lib/memory_interface.h lib/memory_trace.h
timer_cpu/timer0.o: hw_model/state_observer.h ../common/liboptions.h
timer_cpu/timer0.o: ../common/kill.h ../common/static_set.h
timer_cpu/timer0.o: ../common/lua_register.h ../common/lua.hpp
timer_cpu/timer0.o: ../common/units.h ../common/cast.h
timer_cpu/timer0.o: ../common/uint64_t_hash.h ../common/parameters.h
timer_cpu/timer0.o: ../common/param_factory.h ../common/options.h main/cotson.h
timer_cpu/timer0.o: lib/twolev.h lib/cache.h lib/debug_mem.h ../common/error.h
timer_cpu/timer1.o: lib/abaeterno_config.h ../common/error.h
timer_cpu/timer1.o: ../common/liboptions.h ../common/kill.h
timer_cpu/timer1.o: ../common/static_set.h ../common/lua_register.h
timer_cpu/timer1.o: ../common/lua.hpp ../common/units.h ../common/cast.h
timer_cpu/timer1.o: ../common/uint64_t_hash.h ../common/parameters.h
timer_cpu/timer1.o: ../common/param_factory.h ../common/options.h lib/logger.h
timer_cpu/timer1.o: lib/selector.h timer_cpu/cpu_timer.h ../common/libmetric.h
timer_cpu/timer1.o: ../common/metric.h ../common/tagged_metric.h
timer_cpu/timer1.o: ../common/heartbeat.h ../common/aggregator.h
timer_cpu/timer1.o: lib/instruction.h ../common/dump_gzip.h
timer_cpu/timer1.o: ../common/read_gzip.h lib/memory_access.h lib/inline.h
timer_cpu/timer1.o: lib/simplevec.h lib/opcodes.h lib/hashmap.h
timer_cpu/timer1.o: hw_model/states.h hw_model/trace_needs.h lib/delegate.h
timer_cpu/timer1.o: lib/memory_interface.h lib/memory_trace.h
timer_cpu/timer1.o: hw_model/state_observer.h main/cotson.h lib/twolev.h
timer_cpu/timer1.o: lib/cache.h lib/debug_mem.h lib/btb.h lib/bb.h lib/ras.h
timer_cpu/timer_dep.o: lib/abaeterno_config.h lib/logger.h lib/selector.h
timer_cpu/timer_dep.o: timer_cpu/cpu_timer.h ../common/libmetric.h
timer_cpu/timer_dep.o: ../common/metric.h ../common/tagged_metric.h
timer_cpu/timer_dep.o: ../common/heartbeat.h ../common/aggregator.h
timer_cpu/timer_dep.o: lib/instruction.h ../common/dump_gzip.h
timer_cpu/timer_dep.o: ../common/read_gzip.h lib/memory_access.h lib/inline.h
timer_cpu/timer_dep.o: lib/simplevec.h lib/opcodes.h lib/hashmap.h
timer_cpu/timer_dep.o: hw_model/states.h hw_model/trace_needs.h lib/delegate.h
timer_cpu/timer_dep.o: lib/memory_interface.h lib/memory_trace.h
timer_cpu/timer_dep.o: hw_model/state_observer.h ../common/liboptions.h
timer_cpu/timer_dep.o: ../common/kill.h ../common/static_set.h
timer_cpu/timer_dep.o: ../common/lua_register.h ../common/lua.hpp
timer_cpu/timer_dep.o: ../common/units.h ../common/cast.h
timer_cpu/timer_dep.o: ../common/uint64_t_hash.h ../common/parameters.h
timer_cpu/timer_dep.o: ../common/param_factory.h ../common/options.h
timer_cpu/timer_dep.o: main/cotson.h lib/twolev.h lib/cache.h lib/debug_mem.h
timer_cpu/timer_dep.o: ../common/error.h
timer_cpu/timer_random.o: lib/abaeterno_config.h lib/logger.h lib/selector.h
timer_cpu/timer_random.o: timer_cpu/cpu_timer.h ../common/libmetric.h
timer_cpu/timer_random.o: ../common/metric.h ../common/tagged_metric.h
timer_cpu/timer_random.o: ../common/heartbeat.h ../common/aggregator.h
timer_cpu/timer_random.o: lib/instruction.h ../common/dump_gzip.h
timer_cpu/timer_random.o: ../common/read_gzip.h lib/memory_access.h
timer_cpu/timer_random.o: lib/inline.h lib/simplevec.h lib/opcodes.h
timer_cpu/timer_random.o: lib/hashmap.h hw_model/states.h
timer_cpu/timer_random.o: hw_model/trace_needs.h lib/delegate.h
timer_cpu/timer_random.o: lib/memory_interface.h lib/memory_trace.h
timer_cpu/timer_random.o: hw_model/state_observer.h ../common/liboptions.h
timer_cpu/timer_random.o: ../common/kill.h ../common/static_set.h
timer_cpu/timer_random.o: ../common/lua_register.h ../common/lua.hpp
timer_cpu/timer_random.o: ../common/units.h ../common/cast.h
timer_cpu/timer_random.o: ../common/uint64_t_hash.h ../common/parameters.h
timer_cpu/timer_random.o: ../common/param_factory.h ../common/options.h
timer_cpu/timer_random.o: main/cotson.h lib/twolev.h lib/cache.h
timer_cpu/timer_random.o: lib/debug_mem.h ../common/error.h
timer_cpu/trace_stats.o: lib/abaeterno_config.h ../common/liboptions.h
timer_cpu/trace_stats.o: ../common/kill.h ../common/static_set.h
timer_cpu/trace_stats.o: ../common/lua_register.h ../common/lua.hpp
timer_cpu/trace_stats.o: ../common/units.h ../common/cast.h
timer_cpu/trace_stats.o: ../common/uint64_t_hash.h ../common/parameters.h
timer_cpu/trace_stats.o: ../common/param_factory.h ../common/options.h
timer_cpu/trace_stats.o: timer_cpu/cpu_timer.h ../common/libmetric.h
timer_cpu/trace_stats.o: ../common/metric.h ../common/tagged_metric.h
timer_cpu/trace_stats.o: ../common/heartbeat.h ../common/aggregator.h
timer_cpu/trace_stats.o: lib/instruction.h ../common/dump_gzip.h
timer_cpu/trace_stats.o: ../common/read_gzip.h lib/memory_access.h lib/inline.h
timer_cpu/trace_stats.o: lib/simplevec.h lib/opcodes.h lib/hashmap.h
timer_cpu/trace_stats.o: hw_model/states.h hw_model/trace_needs.h
timer_cpu/trace_stats.o: lib/delegate.h lib/memory_interface.h
timer_cpu/trace_stats.o: lib/memory_trace.h hw_model/state_observer.h
timer_cpu/trace_stats.o: lib/logger.h lib/selector.h main/cotson.h
mem_lib/bus_lua.o: lib/abaeterno_config.h lib/lua_helpers.h ../common/lua.hpp
mem_lib/bus_lua.o: lib/memory_interface.h lib/memory_access.h
mem_lib/bus_lua.o: ../common/dump_gzip.h ../common/read_gzip.h lib/inline.h
mem_lib/bus_lua.o: lib/memory_trace.h ../common/libmetric.h ../common/metric.h
mem_lib/bus_lua.o: ../common/tagged_metric.h ../common/heartbeat.h
mem_lib/bus_lua.o: ../common/aggregator.h hw_model/state_observer.h
mem_lib/bus_lua.o: hw_model/states.h ../common/liboptions.h ../common/kill.h
mem_lib/bus_lua.o: ../common/static_set.h ../common/lua_register.h
mem_lib/bus_lua.o: ../common/units.h ../common/cast.h ../common/uint64_t_hash.h
mem_lib/bus_lua.o: ../common/parameters.h ../common/param_factory.h
mem_lib/bus_lua.o: ../common/options.h lib/logger.h lib/selector.h
mem_lib/bus_lua.o: main/cotson.h mem_lib/bus.h mem_lib/timewheel.h
mem_lib/bus_lua.o: mem_lib/cache_line.h lib/hashmap.h ../common/error.h
mem_lib/bus_lua.o: mem_lib/protocol.h mem_lib/mesi.h mem_lib/moesi.h
mem_lib/bus_lua.o: mem_lib/simple_protocol.h mem_lib/futurebus.h
mem_lib/cache_lua.o: lib/abaeterno_config.h lib/lua_helpers.h ../common/lua.hpp
mem_lib/cache_lua.o: lib/memory_interface.h lib/memory_access.h
mem_lib/cache_lua.o: ../common/dump_gzip.h ../common/read_gzip.h lib/inline.h
mem_lib/cache_lua.o: lib/memory_trace.h ../common/libmetric.h
mem_lib/cache_lua.o: ../common/metric.h ../common/tagged_metric.h
mem_lib/cache_lua.o: ../common/heartbeat.h ../common/aggregator.h
mem_lib/cache_lua.o: hw_model/state_observer.h hw_model/states.h
mem_lib/cache_lua.o: ../common/liboptions.h ../common/kill.h
mem_lib/cache_lua.o: ../common/static_set.h ../common/lua_register.h
mem_lib/cache_lua.o: ../common/units.h ../common/cast.h
mem_lib/cache_lua.o: ../common/uint64_t_hash.h ../common/parameters.h
mem_lib/cache_lua.o: ../common/param_factory.h ../common/options.h lib/logger.h
mem_lib/cache_lua.o: lib/selector.h main/cotson.h mem_lib/storage_one_way.h
mem_lib/cache_lua.o: mem_lib/storage.h lib/cache.h lib/debug_mem.h
mem_lib/cache_lua.o: lib/hashmap.h mem_lib/cache_line.h
mem_lib/cache_lua.o: mem_lib/storage_two_way.h mem_lib/storage_four_way.h
mem_lib/cache_lua.o: mem_lib/storage_many.h mem_lib/storage_basic.h
mem_lib/cache_lua.o: mem_lib/cache_timing_basic.h mem_lib/cache_timing_l2.h
mem_lib/cache_lua.o: timer_cpu/cpu_timer.h lib/instruction.h lib/simplevec.h
mem_lib/cache_lua.o: lib/opcodes.h hw_model/trace_needs.h lib/delegate.h
mem_lib/cache_lua.o: mem_lib/cache_timing_mainmem.h mem_lib/cache_impl.h
mem_lib/cache_lua.o: mem_lib/timewheel.h hw_model/machine.h hw_model/cpu.h
mem_lib/cache_lua.o: hw_model/nic.h timer_dev/nic_timer.h hw_model/disk.h
mem_lib/cache_lua.o: timer_dev/disk_timer.h ../common/error.h
mem_lib/cache_timing_l2.o: mem_lib/cache_timing_l2.h lib/abaeterno_config.h
mem_lib/cache_timing_l2.o: ../common/libmetric.h ../common/metric.h
mem_lib/cache_timing_l2.o: ../common/tagged_metric.h ../common/heartbeat.h
mem_lib/cache_timing_l2.o: ../common/aggregator.h ../common/liboptions.h
mem_lib/cache_timing_l2.o: ../common/kill.h ../common/static_set.h
mem_lib/cache_timing_l2.o: ../common/lua_register.h ../common/lua.hpp
mem_lib/cache_timing_l2.o: ../common/units.h ../common/cast.h
mem_lib/cache_timing_l2.o: ../common/uint64_t_hash.h ../common/parameters.h
mem_lib/cache_timing_l2.o: ../common/param_factory.h ../common/options.h
mem_lib/cache_timing_l2.o: lib/memory_interface.h lib/memory_access.h
mem_lib/cache_timing_l2.o: ../common/dump_gzip.h ../common/read_gzip.h
mem_lib/cache_timing_l2.o: lib/inline.h lib/memory_trace.h
mem_lib/cache_timing_l2.o: hw_model/state_observer.h hw_model/states.h
mem_lib/cache_timing_l2.o: lib/logger.h lib/selector.h main/cotson.h
mem_lib/cache_timing_l2.o: timer_cpu/cpu_timer.h lib/instruction.h
mem_lib/cache_timing_l2.o: lib/simplevec.h lib/opcodes.h lib/hashmap.h
mem_lib/cache_timing_l2.o: hw_model/trace_needs.h lib/delegate.h
mem_lib/cache_timing_l2.o: ../common/error.h lib/cache.h lib/debug_mem.h
mem_lib/cache_timing_mainmem.o: mem_lib/cache_timing_mainmem.h
mem_lib/cache_timing_mainmem.o: lib/abaeterno_config.h ../common/libmetric.h
mem_lib/cache_timing_mainmem.o: ../common/metric.h ../common/tagged_metric.h
mem_lib/cache_timing_mainmem.o: ../common/heartbeat.h ../common/aggregator.h
mem_lib/cache_timing_mainmem.o: ../common/liboptions.h ../common/kill.h
mem_lib/cache_timing_mainmem.o: ../common/static_set.h ../common/lua_register.h
mem_lib/cache_timing_mainmem.o: ../common/lua.hpp ../common/units.h
mem_lib/cache_timing_mainmem.o: ../common/cast.h ../common/uint64_t_hash.h
mem_lib/cache_timing_mainmem.o: ../common/parameters.h
mem_lib/cache_timing_mainmem.o: ../common/param_factory.h ../common/options.h
mem_lib/cache_timing_mainmem.o: lib/memory_interface.h lib/memory_access.h
mem_lib/cache_timing_mainmem.o: ../common/dump_gzip.h ../common/read_gzip.h
mem_lib/cache_timing_mainmem.o: lib/inline.h lib/memory_trace.h
mem_lib/cache_timing_mainmem.o: hw_model/state_observer.h hw_model/states.h
mem_lib/cache_timing_mainmem.o: lib/logger.h lib/selector.h main/cotson.h
mem_lib/cache_timing_mainmem.o: timer_cpu/cpu_timer.h lib/instruction.h
mem_lib/cache_timing_mainmem.o: lib/simplevec.h lib/opcodes.h lib/hashmap.h
mem_lib/cache_timing_mainmem.o: hw_model/trace_needs.h lib/delegate.h
mem_lib/cache_timing_mainmem.o: ../common/error.h lib/cache.h lib/debug_mem.h
mem_lib/debug_mem.o: lib/abaeterno_config.h lib/debug_mem.h lib/inline.h
mem_lib/debug_mem.o: lib/hashmap.h mem_lib/cache_line.h lib/memory_interface.h
mem_lib/debug_mem.o: lib/memory_access.h ../common/dump_gzip.h
mem_lib/debug_mem.o: ../common/read_gzip.h lib/memory_trace.h
mem_lib/debug_mem.o: ../common/libmetric.h ../common/metric.h
mem_lib/debug_mem.o: ../common/tagged_metric.h ../common/heartbeat.h
mem_lib/debug_mem.o: ../common/aggregator.h hw_model/state_observer.h
mem_lib/debug_mem.o: hw_model/states.h ../common/liboptions.h ../common/kill.h
mem_lib/debug_mem.o: ../common/static_set.h ../common/lua_register.h
mem_lib/debug_mem.o: ../common/lua.hpp ../common/units.h ../common/cast.h
mem_lib/debug_mem.o: ../common/uint64_t_hash.h ../common/parameters.h
mem_lib/debug_mem.o: ../common/param_factory.h ../common/options.h lib/logger.h
mem_lib/debug_mem.o: lib/selector.h main/cotson.h mem_lib/storage_two_way.h
mem_lib/debug_mem.o: mem_lib/storage.h lib/cache.h mem_lib/storage_four_way.h
mem_lib/futurebus.o: lib/abaeterno_config.h mem_lib/futurebus.h
mem_lib/futurebus.o: lib/memory_interface.h lib/memory_access.h
mem_lib/futurebus.o: ../common/dump_gzip.h ../common/read_gzip.h lib/inline.h
mem_lib/futurebus.o: lib/memory_trace.h ../common/libmetric.h
mem_lib/futurebus.o: ../common/metric.h ../common/tagged_metric.h
mem_lib/futurebus.o: ../common/heartbeat.h ../common/aggregator.h
mem_lib/futurebus.o: hw_model/state_observer.h hw_model/states.h
mem_lib/futurebus.o: ../common/liboptions.h ../common/kill.h
mem_lib/futurebus.o: ../common/static_set.h ../common/lua_register.h
mem_lib/futurebus.o: ../common/lua.hpp ../common/units.h ../common/cast.h
mem_lib/futurebus.o: ../common/uint64_t_hash.h ../common/parameters.h
mem_lib/futurebus.o: ../common/param_factory.h ../common/options.h lib/logger.h
mem_lib/futurebus.o: lib/selector.h main/cotson.h mem_lib/protocol.h
mem_lib/hybrid_memory_cache.o: lib/abaeterno_config.h
mem_lib/hybrid_memory_cache.o: mem_lib/hybrid_memory_cache.h
mem_lib/hybrid_memory_cache.o: lib/memory_interface.h lib/memory_access.h
mem_lib/hybrid_memory_cache.o: ../common/dump_gzip.h ../common/read_gzip.h
mem_lib/hybrid_memory_cache.o: lib/inline.h lib/memory_trace.h
mem_lib/hybrid_memory_cache.o: ../common/libmetric.h ../common/metric.h
mem_lib/hybrid_memory_cache.o: ../common/tagged_metric.h ../common/heartbeat.h
mem_lib/hybrid_memory_cache.o: ../common/aggregator.h hw_model/state_observer.h
mem_lib/hybrid_memory_cache.o: hw_model/states.h ../common/liboptions.h
mem_lib/hybrid_memory_cache.o: ../common/kill.h ../common/static_set.h
mem_lib/hybrid_memory_cache.o: ../common/lua_register.h ../common/lua.hpp
mem_lib/hybrid_memory_cache.o: ../common/units.h ../common/cast.h
mem_lib/hybrid_memory_cache.o: ../common/uint64_t_hash.h ../common/parameters.h
mem_lib/hybrid_memory_cache.o: ../common/param_factory.h ../common/options.h
mem_lib/hybrid_memory_cache.o: lib/logger.h lib/selector.h main/cotson.h
mem_lib/hybrid_memory_cache.o: lib/cache.h lib/debug_mem.h lib/hashmap.h
mem_lib/hybrid_memory_cache.o: mem_lib/hybrid_memory.h
mem_lib/hybrid_memory.o: lib/abaeterno_config.h mem_lib/hybrid_memory.h
mem_lib/hybrid_memory.o: lib/memory_interface.h lib/memory_access.h
mem_lib/hybrid_memory.o: ../common/dump_gzip.h ../common/read_gzip.h
mem_lib/hybrid_memory.o: lib/inline.h lib/memory_trace.h ../common/libmetric.h
mem_lib/hybrid_memory.o: ../common/metric.h ../common/tagged_metric.h
mem_lib/hybrid_memory.o: ../common/heartbeat.h ../common/aggregator.h
mem_lib/hybrid_memory.o: hw_model/state_observer.h hw_model/states.h
mem_lib/hybrid_memory.o: ../common/liboptions.h ../common/kill.h
mem_lib/hybrid_memory.o: ../common/static_set.h ../common/lua_register.h
mem_lib/hybrid_memory.o: ../common/lua.hpp ../common/units.h ../common/cast.h
mem_lib/hybrid_memory.o: ../common/uint64_t_hash.h ../common/parameters.h
mem_lib/hybrid_memory.o: ../common/param_factory.h ../common/options.h
mem_lib/hybrid_memory.o: lib/logger.h lib/selector.h main/cotson.h
mem_lib/hybrid_memory_lua.o: lib/abaeterno_config.h lib/lua_helpers.h
mem_lib/hybrid_memory_lua.o: ../common/lua.hpp lib/memory_interface.h
mem_lib/hybrid_memory_lua.o: lib/memory_access.h ../common/dump_gzip.h
mem_lib/hybrid_memory_lua.o: ../common/read_gzip.h lib/inline.h
mem_lib/hybrid_memory_lua.o: lib/memory_trace.h ../common/libmetric.h
mem_lib/hybrid_memory_lua.o: ../common/metric.h ../common/tagged_metric.h
mem_lib/hybrid_memory_lua.o: ../common/heartbeat.h ../common/aggregator.h
mem_lib/hybrid_memory_lua.o: hw_model/state_observer.h hw_model/states.h
mem_lib/hybrid_memory_lua.o: ../common/liboptions.h ../common/kill.h
mem_lib/hybrid_memory_lua.o: ../common/static_set.h ../common/lua_register.h
mem_lib/hybrid_memory_lua.o: ../common/units.h ../common/cast.h
mem_lib/hybrid_memory_lua.o: ../common/uint64_t_hash.h ../common/parameters.h
mem_lib/hybrid_memory_lua.o: ../common/param_factory.h ../common/options.h
mem_lib/hybrid_memory_lua.o: lib/logger.h lib/selector.h main/cotson.h
mem_lib/hybrid_memory_map.o: lib/abaeterno_config.h mem_lib/hybrid_memory_map.h
mem_lib/hybrid_memory_map.o: lib/memory_interface.h lib/memory_access.h
mem_lib/hybrid_memory_map.o: ../common/dump_gzip.h ../common/read_gzip.h
mem_lib/hybrid_memory_map.o: lib/inline.h lib/memory_trace.h
mem_lib/hybrid_memory_map.o: ../common/libmetric.h ../common/metric.h
mem_lib/hybrid_memory_map.o: ../common/tagged_metric.h ../common/heartbeat.h
mem_lib/hybrid_memory_map.o: ../common/aggregator.h hw_model/state_observer.h
mem_lib/hybrid_memory_map.o: hw_model/states.h ../common/liboptions.h
mem_lib/hybrid_memory_map.o: ../common/kill.h ../common/static_set.h
mem_lib/hybrid_memory_map.o: ../common/lua_register.h ../common/lua.hpp
mem_lib/hybrid_memory_map.o: ../common/units.h ../common/cast.h
mem_lib/hybrid_memory_map.o: ../common/uint64_t_hash.h ../common/parameters.h
mem_lib/hybrid_memory_map.o: ../common/param_factory.h ../common/options.h
mem_lib/hybrid_memory_map.o: lib/logger.h lib/selector.h main/cotson.h
mem_lib/hybrid_memory_map.o: lib/cache.h lib/debug_mem.h lib/hashmap.h
mem_lib/hybrid_memory_map.o: mem_lib/hybrid_memory.h
mem_lib/main_memory_lua.o: lib/abaeterno_config.h lib/lua_helpers.h
mem_lib/main_memory_lua.o: ../common/lua.hpp lib/memory_interface.h
mem_lib/main_memory_lua.o: lib/memory_access.h ../common/dump_gzip.h
mem_lib/main_memory_lua.o: ../common/read_gzip.h lib/inline.h
mem_lib/main_memory_lua.o: lib/memory_trace.h ../common/libmetric.h
mem_lib/main_memory_lua.o: ../common/metric.h ../common/tagged_metric.h
mem_lib/main_memory_lua.o: ../common/heartbeat.h ../common/aggregator.h
mem_lib/main_memory_lua.o: hw_model/state_observer.h hw_model/states.h
mem_lib/main_memory_lua.o: ../common/liboptions.h ../common/kill.h
mem_lib/main_memory_lua.o: ../common/static_set.h ../common/lua_register.h
mem_lib/main_memory_lua.o: ../common/units.h ../common/cast.h
mem_lib/main_memory_lua.o: ../common/uint64_t_hash.h ../common/parameters.h
mem_lib/main_memory_lua.o: ../common/param_factory.h ../common/options.h
mem_lib/main_memory_lua.o: lib/logger.h lib/selector.h main/cotson.h
mem_lib/main_memory_lua.o: mem_lib/main_memory.h mem_lib/cache_timing_l2.h
mem_lib/main_memory_lua.o: timer_cpu/cpu_timer.h lib/instruction.h
mem_lib/main_memory_lua.o: lib/simplevec.h lib/opcodes.h lib/hashmap.h
mem_lib/main_memory_lua.o: hw_model/trace_needs.h lib/delegate.h
mem_lib/main_memory_lua.o: mem_lib/cache_timing_basic.h
mem_lib/main_memory_lua.o: mem_lib/cache_timing_mainmem.h ../common/error.h
mem_lib/mem_bintracer.o: lib/abaeterno_config.h mem_lib/mem_bintracer.h
mem_lib/mem_bintracer.o: lib/memory_interface.h lib/memory_access.h
mem_lib/mem_bintracer.o: ../common/dump_gzip.h ../common/read_gzip.h
mem_lib/mem_bintracer.o: lib/inline.h lib/memory_trace.h ../common/libmetric.h
mem_lib/mem_bintracer.o: ../common/metric.h ../common/tagged_metric.h
mem_lib/mem_bintracer.o: ../common/heartbeat.h ../common/aggregator.h
mem_lib/mem_bintracer.o: hw_model/state_observer.h hw_model/states.h
mem_lib/mem_bintracer.o: ../common/liboptions.h ../common/kill.h
mem_lib/mem_bintracer.o: ../common/static_set.h ../common/lua_register.h
mem_lib/mem_bintracer.o: ../common/lua.hpp ../common/units.h ../common/cast.h
mem_lib/mem_bintracer.o: ../common/uint64_t_hash.h ../common/parameters.h
mem_lib/mem_bintracer.o: ../common/param_factory.h ../common/options.h
mem_lib/mem_bintracer.o: lib/logger.h lib/selector.h main/cotson.h
mem_lib/mem_bintracer_lua.o: lib/abaeterno_config.h lib/lua_helpers.h
mem_lib/mem_bintracer_lua.o: ../common/lua.hpp lib/memory_interface.h
mem_lib/mem_bintracer_lua.o: lib/memory_access.h ../common/dump_gzip.h
mem_lib/mem_bintracer_lua.o: ../common/read_gzip.h lib/inline.h
mem_lib/mem_bintracer_lua.o: lib/memory_trace.h ../common/libmetric.h
mem_lib/mem_bintracer_lua.o: ../common/metric.h ../common/tagged_metric.h
mem_lib/mem_bintracer_lua.o: ../common/heartbeat.h ../common/aggregator.h
mem_lib/mem_bintracer_lua.o: hw_model/state_observer.h hw_model/states.h
mem_lib/mem_bintracer_lua.o: ../common/liboptions.h ../common/kill.h
mem_lib/mem_bintracer_lua.o: ../common/static_set.h ../common/lua_register.h
mem_lib/mem_bintracer_lua.o: ../common/units.h ../common/cast.h
mem_lib/mem_bintracer_lua.o: ../common/uint64_t_hash.h ../common/parameters.h
mem_lib/mem_bintracer_lua.o: ../common/param_factory.h ../common/options.h
mem_lib/mem_bintracer_lua.o: lib/logger.h lib/selector.h main/cotson.h
mem_lib/mem_tracer.o: lib/abaeterno_config.h lib/memory_interface.h
mem_lib/mem_tracer.o: lib/memory_access.h ../common/dump_gzip.h
mem_lib/mem_tracer.o: ../common/read_gzip.h lib/inline.h lib/memory_trace.h
mem_lib/mem_tracer.o: ../common/libmetric.h ../common/metric.h
mem_lib/mem_tracer.o: ../common/tagged_metric.h ../common/heartbeat.h
mem_lib/mem_tracer.o: ../common/aggregator.h hw_model/state_observer.h
mem_lib/mem_tracer.o: hw_model/states.h ../common/liboptions.h ../common/kill.h
mem_lib/mem_tracer.o: ../common/static_set.h ../common/lua_register.h
mem_lib/mem_tracer.o: ../common/lua.hpp ../common/units.h ../common/cast.h
mem_lib/mem_tracer.o: ../common/uint64_t_hash.h ../common/parameters.h
mem_lib/mem_tracer.o: ../common/param_factory.h ../common/options.h
mem_lib/mem_tracer.o: lib/logger.h lib/selector.h main/cotson.h
mem_lib/mem_tracer.o: lib/lua_helpers.h timer_cpu/cpu_timer.h lib/instruction.h
mem_lib/mem_tracer.o: lib/simplevec.h lib/opcodes.h lib/hashmap.h
mem_lib/mem_tracer.o: hw_model/trace_needs.h lib/delegate.h
mem_lib/mesi.o: lib/abaeterno_config.h mem_lib/mesi.h lib/memory_interface.h
mem_lib/mesi.o: lib/memory_access.h ../common/dump_gzip.h ../common/read_gzip.h
mem_lib/mesi.o: lib/inline.h lib/memory_trace.h ../common/libmetric.h
mem_lib/mesi.o: ../common/metric.h ../common/tagged_metric.h
mem_lib/mesi.o: ../common/heartbeat.h ../common/aggregator.h
mem_lib/mesi.o: hw_model/state_observer.h hw_model/states.h
mem_lib/mesi.o: ../common/liboptions.h ../common/kill.h ../common/static_set.h
mem_lib/mesi.o: ../common/lua_register.h ../common/lua.hpp ../common/units.h
mem_lib/mesi.o: ../common/cast.h ../common/uint64_t_hash.h
mem_lib/mesi.o: ../common/parameters.h ../common/param_factory.h
mem_lib/mesi.o: ../common/options.h lib/logger.h lib/selector.h main/cotson.h
mem_lib/mesi.o: mem_lib/protocol.h
mem_lib/moesi.o: lib/abaeterno_config.h mem_lib/moesi.h lib/memory_interface.h
mem_lib/moesi.o: lib/memory_access.h ../common/dump_gzip.h
mem_lib/moesi.o: ../common/read_gzip.h lib/inline.h lib/memory_trace.h
mem_lib/moesi.o: ../common/libmetric.h ../common/metric.h
mem_lib/moesi.o: ../common/tagged_metric.h ../common/heartbeat.h
mem_lib/moesi.o: ../common/aggregator.h hw_model/state_observer.h
mem_lib/moesi.o: hw_model/states.h ../common/liboptions.h ../common/kill.h
mem_lib/moesi.o: ../common/static_set.h ../common/lua_register.h
mem_lib/moesi.o: ../common/lua.hpp ../common/units.h ../common/cast.h
mem_lib/moesi.o: ../common/uint64_t_hash.h ../common/parameters.h
mem_lib/moesi.o: ../common/param_factory.h ../common/options.h lib/logger.h
mem_lib/moesi.o: lib/selector.h main/cotson.h mem_lib/protocol.h
mem_lib/protocol.o: lib/abaeterno_config.h mem_lib/protocol.h
mem_lib/protocol.o: lib/memory_interface.h lib/memory_access.h
mem_lib/protocol.o: ../common/dump_gzip.h ../common/read_gzip.h lib/inline.h
mem_lib/protocol.o: lib/memory_trace.h ../common/libmetric.h ../common/metric.h
mem_lib/protocol.o: ../common/tagged_metric.h ../common/heartbeat.h
mem_lib/protocol.o: ../common/aggregator.h hw_model/state_observer.h
mem_lib/protocol.o: hw_model/states.h ../common/liboptions.h ../common/kill.h
mem_lib/protocol.o: ../common/static_set.h ../common/lua_register.h
mem_lib/protocol.o: ../common/lua.hpp ../common/units.h ../common/cast.h
mem_lib/protocol.o: ../common/uint64_t_hash.h ../common/parameters.h
mem_lib/protocol.o: ../common/param_factory.h ../common/options.h lib/logger.h
mem_lib/protocol.o: lib/selector.h main/cotson.h
mem_lib/simple_protocol.o: lib/abaeterno_config.h mem_lib/simple_protocol.h
mem_lib/simple_protocol.o: lib/memory_interface.h lib/memory_access.h
mem_lib/simple_protocol.o: ../common/dump_gzip.h ../common/read_gzip.h
mem_lib/simple_protocol.o: lib/inline.h lib/memory_trace.h
mem_lib/simple_protocol.o: ../common/libmetric.h ../common/metric.h
mem_lib/simple_protocol.o: ../common/tagged_metric.h ../common/heartbeat.h
mem_lib/simple_protocol.o: ../common/aggregator.h hw_model/state_observer.h
mem_lib/simple_protocol.o: hw_model/states.h ../common/liboptions.h
mem_lib/simple_protocol.o: ../common/kill.h ../common/static_set.h
mem_lib/simple_protocol.o: ../common/lua_register.h ../common/lua.hpp
mem_lib/simple_protocol.o: ../common/units.h ../common/cast.h
mem_lib/simple_protocol.o: ../common/uint64_t_hash.h ../common/parameters.h
mem_lib/simple_protocol.o: ../common/param_factory.h ../common/options.h
mem_lib/simple_protocol.o: lib/logger.h lib/selector.h main/cotson.h
mem_lib/simple_protocol.o: mem_lib/protocol.h ../common/error.h
hw_model/cpu.o: lib/abaeterno_config.h hw_model/cpu.h timer_cpu/cpu_timer.h
hw_model/cpu.o: ../common/libmetric.h ../common/metric.h
hw_model/cpu.o: ../common/tagged_metric.h ../common/heartbeat.h
hw_model/cpu.o: ../common/aggregator.h lib/instruction.h ../common/dump_gzip.h
hw_model/cpu.o: ../common/read_gzip.h lib/memory_access.h lib/inline.h
hw_model/cpu.o: lib/simplevec.h lib/opcodes.h lib/hashmap.h hw_model/states.h
hw_model/cpu.o: hw_model/trace_needs.h lib/delegate.h lib/memory_interface.h
hw_model/cpu.o: lib/memory_trace.h hw_model/state_observer.h
hw_model/cpu.o: ../common/liboptions.h ../common/kill.h ../common/static_set.h
hw_model/cpu.o: ../common/lua_register.h ../common/lua.hpp ../common/units.h
hw_model/cpu.o: ../common/cast.h ../common/uint64_t_hash.h
hw_model/cpu.o: ../common/parameters.h ../common/param_factory.h
hw_model/cpu.o: ../common/options.h lib/logger.h lib/selector.h main/cotson.h
hw_model/cpu.o: lib/lua_helpers.h ../common/error.h parsing/cpu_token_parser.h
hw_model/cpu.o: parsing/token_parser.h lib/instructions.h parsing/token_queue.h
hw_model/cpu.o: lib/predictor.h lib/circbuf.h lib/series.h
hw_model/disk.o: lib/abaeterno_config.h hw_model/disk.h timer_dev/disk_timer.h
hw_model/disk.o: ../common/libmetric.h ../common/metric.h
hw_model/disk.o: ../common/tagged_metric.h ../common/heartbeat.h
hw_model/disk.o: ../common/aggregator.h hw_model/states.h ../common/lua.hpp
hw_model/disk.o: parsing/disk_token_parser.h parsing/token_parser.h
hw_model/disk.o: parsing/circular_token_queue.h parsing/token_queue.h
hw_model/disk.o: lib/logger.h lib/selector.h ../common/error.h
hw_model/disk.o: ../common/liboptions.h ../common/kill.h ../common/static_set.h
hw_model/disk.o: ../common/lua_register.h ../common/units.h ../common/cast.h
hw_model/disk.o: ../common/uint64_t_hash.h ../common/parameters.h
hw_model/disk.o: ../common/param_factory.h ../common/options.h main/cotson.h
hw_model/machine.o: lib/abaeterno_config.h hw_model/machine.h hw_model/cpu.h
hw_model/machine.o: timer_cpu/cpu_timer.h ../common/libmetric.h
hw_model/machine.o: ../common/metric.h ../common/tagged_metric.h
hw_model/machine.o: ../common/heartbeat.h ../common/aggregator.h
hw_model/machine.o: lib/instruction.h ../common/dump_gzip.h
hw_model/machine.o: ../common/read_gzip.h lib/memory_access.h lib/inline.h
hw_model/machine.o: lib/simplevec.h lib/opcodes.h lib/hashmap.h
hw_model/machine.o: hw_model/states.h hw_model/trace_needs.h lib/delegate.h
hw_model/machine.o: lib/memory_interface.h lib/memory_trace.h
hw_model/machine.o: hw_model/state_observer.h ../common/liboptions.h
hw_model/machine.o: ../common/kill.h ../common/static_set.h
hw_model/machine.o: ../common/lua_register.h ../common/lua.hpp
hw_model/machine.o: ../common/units.h ../common/cast.h
hw_model/machine.o: ../common/uint64_t_hash.h ../common/parameters.h
hw_model/machine.o: ../common/param_factory.h ../common/options.h lib/logger.h
hw_model/machine.o: lib/selector.h main/cotson.h lib/lua_helpers.h
hw_model/machine.o: hw_model/nic.h timer_dev/nic_timer.h hw_model/disk.h
hw_model/machine.o: timer_dev/disk_timer.h ../common/error.h
hw_model/machine.o: parsing/token_queue.h parsing/token_parser.h
hw_model/nic.o: lib/abaeterno_config.h hw_model/nic.h timer_dev/nic_timer.h
hw_model/nic.o: ../common/libmetric.h ../common/metric.h
hw_model/nic.o: ../common/tagged_metric.h ../common/heartbeat.h
hw_model/nic.o: ../common/aggregator.h hw_model/states.h ../common/lua.hpp
hw_model/nic.o: parsing/nic_token_parser.h parsing/token_parser.h
hw_model/nic.o: parsing/circular_token_queue.h parsing/token_queue.h
hw_model/nic.o: lib/logger.h lib/selector.h ../common/error.h
hw_model/nic.o: ../common/liboptions.h ../common/kill.h ../common/static_set.h
hw_model/nic.o: ../common/lua_register.h ../common/units.h ../common/cast.h
hw_model/nic.o: ../common/uint64_t_hash.h ../common/parameters.h
hw_model/nic.o: ../common/param_factory.h ../common/options.h main/cotson.h
hw_model/nic.o: main/network_timing.h ../common/timing_message.h
hw_model/predictor.o: lib/abaeterno_config.h ../common/liboptions.h
hw_model/predictor.o: ../common/kill.h ../common/static_set.h
hw_model/predictor.o: ../common/lua_register.h ../common/lua.hpp
hw_model/predictor.o: ../common/units.h ../common/cast.h
hw_model/predictor.o: ../common/uint64_t_hash.h ../common/parameters.h
hw_model/predictor.o: ../common/param_factory.h ../common/options.h
hw_model/predictor.o: lib/predictor.h lib/circbuf.h lib/series.h
parsing/circular_token_queue.o: lib/abaeterno_config.h
parsing/circular_token_queue.o: parsing/circular_token_queue.h
parsing/circular_token_queue.o: parsing/token_queue.h ../common/error.h
parsing/circular_token_queue.o: ../common/liboptions.h ../common/kill.h
parsing/circular_token_queue.o: ../common/static_set.h ../common/lua_register.h
parsing/circular_token_queue.o: ../common/lua.hpp ../common/units.h
parsing/circular_token_queue.o: ../common/cast.h ../common/uint64_t_hash.h
parsing/circular_token_queue.o: ../common/parameters.h
parsing/circular_token_queue.o: ../common/param_factory.h ../common/options.h
parsing/cpu_token_parser.o: lib/abaeterno_config.h parsing/cpu_token_parser.h
parsing/cpu_token_parser.o: parsing/token_parser.h hw_model/trace_needs.h
parsing/cpu_token_parser.o: lib/delegate.h hw_model/states.h lib/opcodes.h
parsing/cpu_token_parser.o: lib/hashmap.h lib/instructions.h lib/instruction.h
parsing/cpu_token_parser.o: ../common/dump_gzip.h ../common/read_gzip.h
parsing/cpu_token_parser.o: lib/memory_access.h lib/inline.h lib/simplevec.h
parsing/cpu_token_parser.o: hw_model/state_observer.h ../common/liboptions.h
parsing/cpu_token_parser.o: ../common/kill.h ../common/static_set.h
parsing/cpu_token_parser.o: ../common/lua_register.h ../common/lua.hpp
parsing/cpu_token_parser.o: ../common/units.h ../common/cast.h
parsing/cpu_token_parser.o: ../common/uint64_t_hash.h ../common/parameters.h
parsing/cpu_token_parser.o: ../common/param_factory.h ../common/options.h
parsing/cpu_token_parser.o: lib/logger.h lib/selector.h main/cotson.h
parsing/cpu_token_parser.o: ../common/metric.h parsing/token_queue.h
parsing/cpu_token_parser.o: parsing/tokens.h ../common/error.h
parsing/cpu_token_parser.o: main/interleaver.h lib/cpuid_call.h
parsing/cpu_token_parser.o: lib/custom_asm_regtab.h
parsing/disk_token_parser.o: lib/abaeterno_config.h parsing/disk_token_parser.h
parsing/disk_token_parser.o: parsing/token_parser.h
parsing/disk_token_parser.o: parsing/circular_token_queue.h
parsing/disk_token_parser.o: parsing/token_queue.h parsing/tokens.h
parsing/disk_token_parser.o: lib/logger.h lib/selector.h ../common/error.h
parsing/disk_token_parser.o: ../common/liboptions.h ../common/kill.h
parsing/disk_token_parser.o: ../common/static_set.h ../common/lua_register.h
parsing/disk_token_parser.o: ../common/lua.hpp ../common/units.h
parsing/disk_token_parser.o: ../common/cast.h ../common/uint64_t_hash.h
parsing/disk_token_parser.o: ../common/parameters.h ../common/param_factory.h
parsing/disk_token_parser.o: ../common/options.h
parsing/nic_token_parser.o: lib/abaeterno_config.h parsing/nic_token_parser.h
parsing/nic_token_parser.o: parsing/token_parser.h
parsing/nic_token_parser.o: parsing/circular_token_queue.h
parsing/nic_token_parser.o: parsing/token_queue.h lib/logger.h lib/selector.h
parsing/nic_token_parser.o: ../common/error.h ../common/liboptions.h
parsing/nic_token_parser.o: ../common/kill.h ../common/static_set.h
parsing/nic_token_parser.o: ../common/lua_register.h ../common/lua.hpp
parsing/nic_token_parser.o: ../common/units.h ../common/cast.h
parsing/nic_token_parser.o: ../common/uint64_t_hash.h ../common/parameters.h
parsing/nic_token_parser.o: ../common/param_factory.h ../common/options.h
sampler/dynamic.o: lib/abaeterno_config.h ../common/error.h
sampler/dynamic.o: ../common/liboptions.h ../common/kill.h
sampler/dynamic.o: ../common/static_set.h ../common/lua_register.h
sampler/dynamic.o: ../common/lua.hpp ../common/units.h ../common/cast.h
sampler/dynamic.o: ../common/uint64_t_hash.h ../common/parameters.h
sampler/dynamic.o: ../common/param_factory.h ../common/options.h
sampler/dynamic.o: hw_model/machine.h hw_model/cpu.h timer_cpu/cpu_timer.h
sampler/dynamic.o: ../common/libmetric.h ../common/metric.h
sampler/dynamic.o: ../common/tagged_metric.h ../common/heartbeat.h
sampler/dynamic.o: ../common/aggregator.h lib/instruction.h
sampler/dynamic.o: ../common/dump_gzip.h ../common/read_gzip.h
sampler/dynamic.o: lib/memory_access.h lib/inline.h lib/simplevec.h
sampler/dynamic.o: lib/opcodes.h lib/hashmap.h hw_model/states.h
sampler/dynamic.o: hw_model/trace_needs.h lib/delegate.h lib/memory_interface.h
sampler/dynamic.o: lib/memory_trace.h hw_model/state_observer.h lib/logger.h
sampler/dynamic.o: lib/selector.h main/cotson.h lib/lua_helpers.h
sampler/dynamic.o: hw_model/nic.h timer_dev/nic_timer.h hw_model/disk.h
sampler/dynamic.o: timer_dev/disk_timer.h sampler/sampler.h lib/series.h
sampler/dynamic.o: lib/circbuf.h
sampler/exp_random.o: lib/abaeterno_config.h ../common/error.h
sampler/exp_random.o: ../common/liboptions.h ../common/kill.h
sampler/exp_random.o: ../common/static_set.h ../common/lua_register.h
sampler/exp_random.o: ../common/lua.hpp ../common/units.h ../common/cast.h
sampler/exp_random.o: ../common/uint64_t_hash.h ../common/parameters.h
sampler/exp_random.o: ../common/param_factory.h ../common/options.h
sampler/exp_random.o: sampler/sampler.h hw_model/states.h
sampler/interval.o: lib/abaeterno_config.h ../common/error.h
sampler/interval.o: ../common/liboptions.h ../common/kill.h
sampler/interval.o: ../common/static_set.h ../common/lua_register.h
sampler/interval.o: ../common/lua.hpp ../common/units.h ../common/cast.h
sampler/interval.o: ../common/uint64_t_hash.h ../common/parameters.h
sampler/interval.o: ../common/param_factory.h ../common/options.h
sampler/interval.o: sampler/sampler.h hw_model/states.h
sampler/no_timing.o: lib/abaeterno_config.h sampler/sampler.h hw_model/states.h
sampler/no_timing.o: ../common/liboptions.h ../common/kill.h
sampler/no_timing.o: ../common/static_set.h ../common/lua_register.h
sampler/no_timing.o: ../common/lua.hpp ../common/units.h ../common/cast.h
sampler/no_timing.o: ../common/uint64_t_hash.h ../common/parameters.h
sampler/no_timing.o: ../common/param_factory.h ../common/options.h
sampler/random.o: lib/abaeterno_config.h ../common/error.h
sampler/random.o: ../common/liboptions.h ../common/kill.h
sampler/random.o: ../common/static_set.h ../common/lua_register.h
sampler/random.o: ../common/lua.hpp ../common/units.h ../common/cast.h
sampler/random.o: ../common/uint64_t_hash.h ../common/parameters.h
sampler/random.o: ../common/param_factory.h ../common/options.h
sampler/random.o: sampler/sampler.h hw_model/states.h
sampler/selective.o: lib/abaeterno_config.h main/abaeterno.h
sampler/selective.o: ../common/libmetric.h ../common/metric.h
sampler/selective.o: ../common/tagged_metric.h ../common/heartbeat.h
sampler/selective.o: ../common/aggregator.h sampler/sampler.h hw_model/states.h
sampler/selective.o: main/profiler.h lib/circbuf.h lib/hashmap.h lib/logger.h
sampler/selective.o: lib/selector.h ../common/error.h ../common/liboptions.h
sampler/selective.o: ../common/kill.h ../common/static_set.h
sampler/selective.o: ../common/lua_register.h ../common/lua.hpp
sampler/selective.o: ../common/units.h ../common/cast.h
sampler/selective.o: ../common/uint64_t_hash.h ../common/parameters.h
sampler/selective.o: ../common/param_factory.h ../common/options.h
sampler/selective.o: main/cotson.h lib/cpuid_call.h lib/instruction.h
sampler/selective.o: ../common/dump_gzip.h ../common/read_gzip.h
sampler/selective.o: lib/memory_access.h lib/inline.h lib/simplevec.h
sampler/selective.o: lib/opcodes.h lib/custom_asm_regtab.h
sampler/simple.o: lib/abaeterno_config.h sampler/sampler.h hw_model/states.h
sampler/simple.o: ../common/liboptions.h ../common/kill.h
sampler/simple.o: ../common/static_set.h ../common/lua_register.h
sampler/simple.o: ../common/lua.hpp ../common/units.h ../common/cast.h
sampler/simple.o: ../common/uint64_t_hash.h ../common/parameters.h
sampler/simple.o: ../common/param_factory.h ../common/options.h
sampler/simpoint_reader.o: lib/abaeterno_config.h ../common/error.h
sampler/simpoint_reader.o: ../common/liboptions.h ../common/kill.h
sampler/simpoint_reader.o: ../common/static_set.h ../common/lua_register.h
sampler/simpoint_reader.o: ../common/lua.hpp ../common/units.h ../common/cast.h
sampler/simpoint_reader.o: ../common/uint64_t_hash.h ../common/parameters.h
sampler/simpoint_reader.o: ../common/param_factory.h ../common/options.h
sampler/simpoint_reader.o: sampler/sampler.h hw_model/states.h
sampler/smarts.o: lib/abaeterno_config.h sampler/sampler.h hw_model/states.h
sampler/smarts.o: ../common/liboptions.h ../common/kill.h
sampler/smarts.o: ../common/static_set.h ../common/lua_register.h
sampler/smarts.o: ../common/lua.hpp ../common/units.h ../common/cast.h
sampler/smarts.o: ../common/uint64_t_hash.h ../common/parameters.h
sampler/smarts.o: ../common/param_factory.h ../common/options.h
timer_dev/disksim.o: lib/abaeterno_config.h lib/logger.h lib/selector.h
timer_dev/disksim.o: main/cotson.h ../common/metric.h timer_dev/disk_timer.h
timer_dev/disksim.o: ../common/libmetric.h ../common/tagged_metric.h
timer_dev/disksim.o: ../common/heartbeat.h ../common/aggregator.h
timer_dev/disksim.o: ../common/liboptions.h ../common/kill.h
timer_dev/disksim.o: ../common/static_set.h ../common/lua_register.h
timer_dev/disksim.o: ../common/lua.hpp ../common/units.h ../common/cast.h
timer_dev/disksim.o: ../common/uint64_t_hash.h ../common/parameters.h
timer_dev/disksim.o: ../common/param_factory.h ../common/options.h
timer_dev/disksim.o: timer_dev/disksim_interface.h
timer_dev/disksim.o: ../disksim/src/disksim_reqflags.h
timer_dev/disk_tracer.o: lib/abaeterno_config.h lib/logger.h lib/selector.h
timer_dev/disk_tracer.o: main/cotson.h ../common/metric.h
timer_dev/disk_tracer.o: timer_dev/disk_timer.h ../common/libmetric.h
timer_dev/disk_tracer.o: ../common/tagged_metric.h ../common/heartbeat.h
timer_dev/disk_tracer.o: ../common/aggregator.h ../common/liboptions.h
timer_dev/disk_tracer.o: ../common/kill.h ../common/static_set.h
timer_dev/disk_tracer.o: ../common/lua_register.h ../common/lua.hpp
timer_dev/disk_tracer.o: ../common/units.h ../common/cast.h
timer_dev/disk_tracer.o: ../common/uint64_t_hash.h ../common/parameters.h
timer_dev/disk_tracer.o: ../common/param_factory.h ../common/options.h
timer_dev/perfect_disk.o: lib/abaeterno_config.h lib/logger.h lib/selector.h
timer_dev/perfect_disk.o: ../common/error.h ../common/liboptions.h
timer_dev/perfect_disk.o: ../common/kill.h ../common/static_set.h
timer_dev/perfect_disk.o: ../common/lua_register.h ../common/lua.hpp
timer_dev/perfect_disk.o: ../common/units.h ../common/cast.h
timer_dev/perfect_disk.o: ../common/uint64_t_hash.h ../common/parameters.h
timer_dev/perfect_disk.o: ../common/param_factory.h ../common/options.h
timer_dev/perfect_disk.o: timer_dev/disk_timer.h ../common/libmetric.h
timer_dev/perfect_disk.o: ../common/metric.h ../common/tagged_metric.h
timer_dev/perfect_disk.o: ../common/heartbeat.h ../common/aggregator.h
timer_dev/simple_disk.o: lib/abaeterno_config.h lib/logger.h lib/selector.h
timer_dev/simple_disk.o: ../common/error.h ../common/liboptions.h
timer_dev/simple_disk.o: ../common/kill.h ../common/static_set.h
timer_dev/simple_disk.o: ../common/lua_register.h ../common/lua.hpp
timer_dev/simple_disk.o: ../common/units.h ../common/cast.h
timer_dev/simple_disk.o: ../common/uint64_t_hash.h ../common/parameters.h
timer_dev/simple_disk.o: ../common/param_factory.h ../common/options.h
timer_dev/simple_disk.o: timer_dev/disk_timer.h ../common/libmetric.h
timer_dev/simple_disk.o: ../common/metric.h ../common/tagged_metric.h
timer_dev/simple_disk.o: ../common/heartbeat.h ../common/aggregator.h
timer_dev/simple_nic.o: lib/abaeterno_config.h lib/logger.h lib/selector.h
timer_dev/simple_nic.o: timer_dev/nic_timer.h ../common/libmetric.h
timer_dev/simple_nic.o: ../common/metric.h ../common/tagged_metric.h
timer_dev/simple_nic.o: ../common/heartbeat.h ../common/aggregator.h
timer_dev/simple_nic.o: ../common/liboptions.h ../common/kill.h
timer_dev/simple_nic.o: ../common/static_set.h ../common/lua_register.h
timer_dev/simple_nic.o: ../common/lua.hpp ../common/units.h ../common/cast.h
timer_dev/simple_nic.o: ../common/uint64_t_hash.h ../common/parameters.h
timer_dev/simple_nic.o: ../common/param_factory.h ../common/options.h
lib/btb.o: lib/abaeterno_config.h lib/btb.h lib/cache.h ../common/liboptions.h
lib/btb.o: ../common/kill.h ../common/static_set.h ../common/lua_register.h
lib/btb.o: ../common/lua.hpp ../common/units.h ../common/cast.h
lib/btb.o: ../common/uint64_t_hash.h ../common/parameters.h
lib/btb.o: ../common/param_factory.h ../common/options.h lib/debug_mem.h
lib/btb.o: lib/inline.h lib/hashmap.h lib/bb.h ../common/libmetric.h
lib/btb.o: ../common/metric.h ../common/tagged_metric.h ../common/heartbeat.h
lib/btb.o: ../common/aggregator.h lib/logger.h lib/selector.h ../common/error.h
lib/instruction.o: lib/abaeterno_config.h lib/instruction.h
lib/instruction.o: ../common/dump_gzip.h ../common/read_gzip.h
lib/instruction.o: lib/memory_access.h lib/inline.h lib/simplevec.h
lib/instruction.o: lib/opcodes.h lib/hashmap.h ../common/error.h
lib/instruction.o: ../common/liboptions.h ../common/kill.h
lib/instruction.o: ../common/static_set.h ../common/lua_register.h
lib/instruction.o: ../common/lua.hpp ../common/units.h ../common/cast.h
lib/instruction.o: ../common/uint64_t_hash.h ../common/parameters.h
lib/instruction.o: ../common/param_factory.h ../common/options.h
lib/instruction.o: ../distorm/distorm.h
lib/instructions.o: lib/abaeterno_config.h lib/instructions.h lib/instruction.h
lib/instructions.o: ../common/dump_gzip.h ../common/read_gzip.h
lib/instructions.o: lib/memory_access.h lib/inline.h lib/simplevec.h
lib/instructions.o: lib/opcodes.h lib/hashmap.h lib/logger.h lib/selector.h
lib/lua_helpers.o: lib/abaeterno_config.h lib/lua_helpers.h ../common/lua.hpp
lib/lua_helpers.o: lib/memory_interface.h lib/memory_access.h
lib/lua_helpers.o: ../common/dump_gzip.h ../common/read_gzip.h lib/inline.h
lib/lua_helpers.o: lib/memory_trace.h ../common/libmetric.h ../common/metric.h
lib/lua_helpers.o: ../common/tagged_metric.h ../common/heartbeat.h
lib/lua_helpers.o: ../common/aggregator.h hw_model/state_observer.h
lib/lua_helpers.o: hw_model/states.h ../common/liboptions.h ../common/kill.h
lib/lua_helpers.o: ../common/static_set.h ../common/lua_register.h
lib/lua_helpers.o: ../common/units.h ../common/cast.h ../common/uint64_t_hash.h
lib/lua_helpers.o: ../common/parameters.h ../common/param_factory.h
lib/lua_helpers.o: ../common/options.h lib/logger.h lib/selector.h
lib/lua_helpers.o: main/cotson.h
lib/memory_access.o: lib/abaeterno_config.h lib/memory_access.h
lib/memory_access.o: ../common/dump_gzip.h ../common/read_gzip.h lib/inline.h
lib/memory_interface.o: lib/abaeterno_config.h lib/memory_interface.h
lib/memory_interface.o: lib/memory_access.h ../common/dump_gzip.h
lib/memory_interface.o: ../common/read_gzip.h lib/inline.h lib/memory_trace.h
lib/memory_interface.o: ../common/libmetric.h ../common/metric.h
lib/memory_interface.o: ../common/tagged_metric.h ../common/heartbeat.h
lib/memory_interface.o: ../common/aggregator.h hw_model/state_observer.h
lib/memory_interface.o: hw_model/states.h ../common/liboptions.h
lib/memory_interface.o: ../common/kill.h ../common/static_set.h
lib/memory_interface.o: ../common/lua_register.h ../common/lua.hpp
lib/memory_interface.o: ../common/units.h ../common/cast.h
lib/memory_interface.o: ../common/uint64_t_hash.h ../common/parameters.h
lib/memory_interface.o: ../common/param_factory.h ../common/options.h
lib/memory_interface.o: lib/logger.h lib/selector.h main/cotson.h
lib/memory_interface.o: ../common/error.h timer_cpu/cpu_timer.h
lib/memory_interface.o: lib/instruction.h lib/simplevec.h lib/opcodes.h
lib/memory_interface.o: lib/hashmap.h hw_model/trace_needs.h lib/delegate.h
lib/opcodes.o: lib/abaeterno_config.h lib/opcodes.h lib/hashmap.h lib/logger.h
lib/opcodes.o: lib/selector.h
lib/twolev.o: lib/abaeterno_config.h lib/twolev.h lib/cache.h
lib/twolev.o: ../common/liboptions.h ../common/kill.h ../common/static_set.h
lib/twolev.o: ../common/lua_register.h ../common/lua.hpp ../common/units.h
lib/twolev.o: ../common/cast.h ../common/uint64_t_hash.h ../common/parameters.h
lib/twolev.o: ../common/param_factory.h ../common/options.h lib/debug_mem.h
lib/twolev.o: lib/inline.h lib/hashmap.h ../common/libmetric.h
lib/twolev.o: ../common/metric.h ../common/tagged_metric.h
lib/twolev.o: ../common/heartbeat.h ../common/aggregator.h ../common/error.h
lib/twolev.o: lib/logger.h lib/selector.h
lib/version.o: lib/abaeterno_config.h lib/version.h lib/version_info.h
simnow/proxy.o: lib/abaeterno_config.h main/abaeterno.h ../common/libmetric.h
simnow/proxy.o: ../common/metric.h ../common/tagged_metric.h
simnow/proxy.o: ../common/heartbeat.h ../common/aggregator.h sampler/sampler.h
simnow/proxy.o: hw_model/states.h main/profiler.h lib/circbuf.h lib/hashmap.h
simnow/proxy.o: main/cotson.h lib/callbacks.h ../common/liboptions.h
simnow/proxy.o: ../common/kill.h ../common/static_set.h
simnow/proxy.o: ../common/lua_register.h ../common/lua.hpp ../common/units.h
simnow/proxy.o: ../common/cast.h ../common/uint64_t_hash.h
simnow/proxy.o: ../common/parameters.h ../common/param_factory.h
simnow/proxy.o: ../common/options.h lib/version.h lib/selector.h
simnow/proxy.o: ../common/error.h main/command_socket.h lib/callme.h
simnow/proxy.o: hw_model/machine.h hw_model/cpu.h timer_cpu/cpu_timer.h
simnow/proxy.o: lib/instruction.h ../common/dump_gzip.h ../common/read_gzip.h
simnow/proxy.o: lib/memory_access.h lib/inline.h lib/simplevec.h lib/opcodes.h
simnow/proxy.o: hw_model/trace_needs.h lib/delegate.h lib/memory_interface.h
simnow/proxy.o: lib/memory_trace.h hw_model/state_observer.h lib/logger.h
simnow/proxy.o: lib/lua_helpers.h hw_model/nic.h timer_dev/nic_timer.h
simnow/proxy.o: hw_model/disk.h timer_dev/disk_timer.h parsing/tokens.h
simnow/proxy.o: parsing/token_queue.h
injector/injector.o: lib/abaeterno_config.h lib/logger.h lib/selector.h
injector/injector.o: ../common/error.h ../common/liboptions.h ../common/kill.h
injector/injector.o: ../common/static_set.h ../common/lua_register.h
injector/injector.o: ../common/lua.hpp ../common/units.h ../common/cast.h
injector/injector.o: ../common/uint64_t_hash.h ../common/parameters.h
injector/injector.o: ../common/param_factory.h ../common/options.h
injector/injector.o: lib/lua_helpers.h lib/memory_interface.h
injector/injector.o: lib/memory_access.h ../common/dump_gzip.h
injector/injector.o: ../common/read_gzip.h lib/inline.h lib/memory_trace.h
injector/injector.o: ../common/libmetric.h ../common/metric.h
injector/injector.o: ../common/tagged_metric.h ../common/heartbeat.h
injector/injector.o: ../common/aggregator.h hw_model/state_observer.h
injector/injector.o: hw_model/states.h main/cotson.h timer_cpu/cpu_timer.h
injector/injector.o: lib/instruction.h lib/simplevec.h lib/opcodes.h
injector/injector.o: lib/hashmap.h hw_model/trace_needs.h lib/delegate.h
injector/injector.o: lib/instructions.h ../common/fileutil.h
injector/injector.o: mem_lib/mem_bintracer.h ../common/memgate.h
test.unit/test_cache.o: lib/cache.h ../common/liboptions.h ../common/kill.h
test.unit/test_cache.o: ../common/static_set.h ../common/lua_register.h
test.unit/test_cache.o: ../common/lua.hpp ../common/units.h ../common/cast.h
test.unit/test_cache.o: ../common/uint64_t_hash.h ../common/parameters.h
test.unit/test_cache.o: ../common/param_factory.h ../common/options.h
test.unit/test_cache.o: lib/debug_mem.h lib/inline.h lib/hashmap.h
test.unit/test_cache_hierarchy.o: ../common/lua.hpp lib/abaeterno_config.h
test.unit/test_cache_hierarchy.o: mem_lib/main_memory.h lib/memory_interface.h
test.unit/test_cache_hierarchy.o: lib/memory_access.h ../common/dump_gzip.h
test.unit/test_cache_hierarchy.o: ../common/read_gzip.h lib/inline.h
test.unit/test_cache_hierarchy.o: lib/memory_trace.h ../common/libmetric.h
test.unit/test_cache_hierarchy.o: ../common/metric.h ../common/tagged_metric.h
test.unit/test_cache_hierarchy.o: ../common/heartbeat.h ../common/aggregator.h
test.unit/test_cache_hierarchy.o: hw_model/state_observer.h hw_model/states.h
test.unit/test_cache_hierarchy.o: ../common/liboptions.h ../common/kill.h
test.unit/test_cache_hierarchy.o: ../common/static_set.h
test.unit/test_cache_hierarchy.o: ../common/lua_register.h ../common/units.h
test.unit/test_cache_hierarchy.o: ../common/cast.h ../common/uint64_t_hash.h
test.unit/test_cache_hierarchy.o: ../common/parameters.h
test.unit/test_cache_hierarchy.o: ../common/param_factory.h ../common/options.h
test.unit/test_cache_hierarchy.o: lib/logger.h lib/selector.h main/cotson.h
test.unit/test_cache_hierarchy.o: mem_lib/cache_timing_l2.h
test.unit/test_cache_hierarchy.o: timer_cpu/cpu_timer.h lib/instruction.h
test.unit/test_cache_hierarchy.o: lib/simplevec.h lib/opcodes.h lib/hashmap.h
test.unit/test_cache_hierarchy.o: hw_model/trace_needs.h lib/delegate.h
test.unit/test_cache_hierarchy.o: mem_lib/cache_timing_basic.h
test.unit/test_cache_hierarchy.o: mem_lib/cache_timing_mainmem.h
test.unit/test_cache_hierarchy.o: ../common/error.h mem_lib/storage_basic.h
test.unit/test_cache_hierarchy.o: mem_lib/storage.h lib/cache.h lib/debug_mem.h
test.unit/test_cache_hierarchy.o: mem_lib/cache_line.h mem_lib/cache_impl.h
test.unit/test_cache_hierarchy.o: mem_lib/timewheel.h hw_model/machine.h
test.unit/test_cache_hierarchy.o: hw_model/cpu.h lib/lua_helpers.h
test.unit/test_cache_hierarchy.o: hw_model/nic.h timer_dev/nic_timer.h
test.unit/test_cache_hierarchy.o: hw_model/disk.h timer_dev/disk_timer.h
test.unit/test_cache_hierarchy.o: mem_lib/protocol.h mem_lib/bus.h
test.unit/test_cache_hierarchy.o: mem_lib/simple_protocol.h
test.unit/test_cache_impl.o: ../common/lua.hpp lib/abaeterno_config.h
test.unit/test_cache_impl.o: mem_lib/main_memory.h lib/memory_interface.h
test.unit/test_cache_impl.o: lib/memory_access.h ../common/dump_gzip.h
test.unit/test_cache_impl.o: ../common/read_gzip.h lib/inline.h
test.unit/test_cache_impl.o: lib/memory_trace.h ../common/libmetric.h
test.unit/test_cache_impl.o: ../common/metric.h ../common/tagged_metric.h
test.unit/test_cache_impl.o: ../common/heartbeat.h ../common/aggregator.h
test.unit/test_cache_impl.o: hw_model/state_observer.h hw_model/states.h
test.unit/test_cache_impl.o: ../common/liboptions.h ../common/kill.h
test.unit/test_cache_impl.o: ../common/static_set.h ../common/lua_register.h
test.unit/test_cache_impl.o: ../common/units.h ../common/cast.h
test.unit/test_cache_impl.o: ../common/uint64_t_hash.h ../common/parameters.h
test.unit/test_cache_impl.o: ../common/param_factory.h ../common/options.h
test.unit/test_cache_impl.o: lib/logger.h lib/selector.h main/cotson.h
test.unit/test_cache_impl.o: mem_lib/cache_timing_l2.h timer_cpu/cpu_timer.h
test.unit/test_cache_impl.o: lib/instruction.h lib/simplevec.h lib/opcodes.h
test.unit/test_cache_impl.o: lib/hashmap.h hw_model/trace_needs.h
test.unit/test_cache_impl.o: lib/delegate.h mem_lib/cache_timing_basic.h
test.unit/test_cache_impl.o: mem_lib/cache_timing_mainmem.h ../common/error.h
test.unit/test_cache_impl.o: mem_lib/storage_one_way.h mem_lib/storage.h
test.unit/test_cache_impl.o: lib/cache.h lib/debug_mem.h mem_lib/cache_line.h
test.unit/test_cache_impl.o: mem_lib/storage_two_way.h
test.unit/test_cache_impl.o: mem_lib/storage_four_way.h mem_lib/storage_many.h
test.unit/test_cache_impl.o: mem_lib/storage_basic.h mem_lib/cache_impl.h
test.unit/test_cache_impl.o: mem_lib/timewheel.h hw_model/machine.h
test.unit/test_cache_impl.o: hw_model/cpu.h lib/lua_helpers.h hw_model/nic.h
test.unit/test_cache_impl.o: timer_dev/nic_timer.h hw_model/disk.h
test.unit/test_cache_impl.o: timer_dev/disk_timer.h mem_lib/protocol.h
test.unit/test_cache_impl.o: mem_lib/bus.h mem_lib/simple_protocol.h
test.unit/test_cache_impl_wb.o: ../common/lua.hpp lib/abaeterno_config.h
test.unit/test_cache_impl_wb.o: mem_lib/main_memory.h lib/memory_interface.h
test.unit/test_cache_impl_wb.o: lib/memory_access.h ../common/dump_gzip.h
test.unit/test_cache_impl_wb.o: ../common/read_gzip.h lib/inline.h
test.unit/test_cache_impl_wb.o: lib/memory_trace.h ../common/libmetric.h
test.unit/test_cache_impl_wb.o: ../common/metric.h ../common/tagged_metric.h
test.unit/test_cache_impl_wb.o: ../common/heartbeat.h ../common/aggregator.h
test.unit/test_cache_impl_wb.o: hw_model/state_observer.h hw_model/states.h
test.unit/test_cache_impl_wb.o: ../common/liboptions.h ../common/kill.h
test.unit/test_cache_impl_wb.o: ../common/static_set.h ../common/lua_register.h
test.unit/test_cache_impl_wb.o: ../common/units.h ../common/cast.h
test.unit/test_cache_impl_wb.o: ../common/uint64_t_hash.h
test.unit/test_cache_impl_wb.o: ../common/parameters.h
test.unit/test_cache_impl_wb.o: ../common/param_factory.h ../common/options.h
test.unit/test_cache_impl_wb.o: lib/logger.h lib/selector.h main/cotson.h
test.unit/test_cache_impl_wb.o: mem_lib/cache_timing_l2.h timer_cpu/cpu_timer.h
test.unit/test_cache_impl_wb.o: lib/instruction.h lib/simplevec.h lib/opcodes.h
test.unit/test_cache_impl_wb.o: lib/hashmap.h hw_model/trace_needs.h
test.unit/test_cache_impl_wb.o: lib/delegate.h mem_lib/cache_timing_basic.h
test.unit/test_cache_impl_wb.o: mem_lib/cache_timing_mainmem.h
test.unit/test_cache_impl_wb.o: ../common/error.h mem_lib/storage_one_way.h
test.unit/test_cache_impl_wb.o: mem_lib/storage.h lib/cache.h lib/debug_mem.h
test.unit/test_cache_impl_wb.o: mem_lib/cache_line.h mem_lib/storage_two_way.h
test.unit/test_cache_impl_wb.o: mem_lib/storage_four_way.h
test.unit/test_cache_impl_wb.o: mem_lib/storage_many.h mem_lib/storage_basic.h
test.unit/test_cache_impl_wb.o: mem_lib/cache_impl.h mem_lib/timewheel.h
test.unit/test_cache_impl_wb.o: hw_model/machine.h hw_model/cpu.h
test.unit/test_cache_impl_wb.o: lib/lua_helpers.h hw_model/nic.h
test.unit/test_cache_impl_wb.o: timer_dev/nic_timer.h hw_model/disk.h
test.unit/test_cache_impl_wb.o: timer_dev/disk_timer.h
test.unit/test_circbuf.o: lib/circbuf.h
test.unit/test_mesi_2levels.o: ../common/lua.hpp lib/abaeterno_config.h
test.unit/test_mesi_2levels.o: mem_lib/main_memory.h lib/memory_interface.h
test.unit/test_mesi_2levels.o: lib/memory_access.h ../common/dump_gzip.h
test.unit/test_mesi_2levels.o: ../common/read_gzip.h lib/inline.h
test.unit/test_mesi_2levels.o: lib/memory_trace.h ../common/libmetric.h
test.unit/test_mesi_2levels.o: ../common/metric.h ../common/tagged_metric.h
test.unit/test_mesi_2levels.o: ../common/heartbeat.h ../common/aggregator.h
test.unit/test_mesi_2levels.o: hw_model/state_observer.h hw_model/states.h
test.unit/test_mesi_2levels.o: ../common/liboptions.h ../common/kill.h
test.unit/test_mesi_2levels.o: ../common/static_set.h ../common/lua_register.h
test.unit/test_mesi_2levels.o: ../common/units.h ../common/cast.h
test.unit/test_mesi_2levels.o: ../common/uint64_t_hash.h ../common/parameters.h
test.unit/test_mesi_2levels.o: ../common/param_factory.h ../common/options.h
test.unit/test_mesi_2levels.o: lib/logger.h lib/selector.h main/cotson.h
test.unit/test_mesi_2levels.o: mem_lib/cache_timing_l2.h timer_cpu/cpu_timer.h
test.unit/test_mesi_2levels.o: lib/instruction.h lib/simplevec.h lib/opcodes.h
test.unit/test_mesi_2levels.o: lib/hashmap.h hw_model/trace_needs.h
test.unit/test_mesi_2levels.o: lib/delegate.h mem_lib/cache_timing_basic.h
test.unit/test_mesi_2levels.o: mem_lib/cache_timing_mainmem.h ../common/error.h
test.unit/test_mesi_2levels.o: mem_lib/storage_basic.h mem_lib/storage.h
test.unit/test_mesi_2levels.o: lib/cache.h lib/debug_mem.h mem_lib/cache_line.h
test.unit/test_mesi_2levels.o: mem_lib/storage_two_way.h
test.unit/test_mesi_2levels.o: mem_lib/storage_four_way.h mem_lib/cache_impl.h
test.unit/test_mesi_2levels.o: mem_lib/timewheel.h hw_model/machine.h
test.unit/test_mesi_2levels.o: hw_model/cpu.h lib/lua_helpers.h hw_model/nic.h
test.unit/test_mesi_2levels.o: timer_dev/nic_timer.h hw_model/disk.h
test.unit/test_mesi_2levels.o: timer_dev/disk_timer.h mem_lib/protocol.h
test.unit/test_mesi_2levels.o: mem_lib/bus.h mem_lib/mesi.h
test.unit/test_moesi_2levels.o: ../common/lua.hpp lib/abaeterno_config.h
test.unit/test_moesi_2levels.o: mem_lib/main_memory.h lib/memory_interface.h
test.unit/test_moesi_2levels.o: lib/memory_access.h ../common/dump_gzip.h
test.unit/test_moesi_2levels.o: ../common/read_gzip.h lib/inline.h
test.unit/test_moesi_2levels.o: lib/memory_trace.h ../common/libmetric.h
test.unit/test_moesi_2levels.o: ../common/metric.h ../common/tagged_metric.h
test.unit/test_moesi_2levels.o: ../common/heartbeat.h ../common/aggregator.h
test.unit/test_moesi_2levels.o: hw_model/state_observer.h hw_model/states.h
test.unit/test_moesi_2levels.o: ../common/liboptions.h ../common/kill.h
test.unit/test_moesi_2levels.o: ../common/static_set.h ../common/lua_register.h
test.unit/test_moesi_2levels.o: ../common/units.h ../common/cast.h
test.unit/test_moesi_2levels.o: ../common/uint64_t_hash.h
test.unit/test_moesi_2levels.o: ../common/parameters.h
test.unit/test_moesi_2levels.o: ../common/param_factory.h ../common/options.h
test.unit/test_moesi_2levels.o: lib/logger.h lib/selector.h main/cotson.h
test.unit/test_moesi_2levels.o: mem_lib/cache_timing_l2.h timer_cpu/cpu_timer.h
test.unit/test_moesi_2levels.o: lib/instruction.h lib/simplevec.h lib/opcodes.h
test.unit/test_moesi_2levels.o: lib/hashmap.h hw_model/trace_needs.h
test.unit/test_moesi_2levels.o: lib/delegate.h mem_lib/cache_timing_basic.h
test.unit/test_moesi_2levels.o: mem_lib/cache_timing_mainmem.h
test.unit/test_moesi_2levels.o: ../common/error.h mem_lib/storage_basic.h
test.unit/test_moesi_2levels.o: mem_lib/storage.h lib/cache.h lib/debug_mem.h
test.unit/test_moesi_2levels.o: mem_lib/cache_line.h mem_lib/cache_impl.h
test.unit/test_moesi_2levels.o: mem_lib/timewheel.h hw_model/machine.h
test.unit/test_moesi_2levels.o: hw_model/cpu.h lib/lua_helpers.h hw_model/nic.h
test.unit/test_moesi_2levels.o: timer_dev/nic_timer.h hw_model/disk.h
test.unit/test_moesi_2levels.o: timer_dev/disk_timer.h mem_lib/protocol.h
test.unit/test_moesi_2levels.o: mem_lib/bus.h mem_lib/moesi.h
test.unit/test_ras.o: lib/ras.h ../common/error.h ../common/liboptions.h
test.unit/test_ras.o: ../common/kill.h ../common/static_set.h
test.unit/test_ras.o: ../common/lua_register.h ../common/lua.hpp
test.unit/test_ras.o: ../common/units.h ../common/cast.h
test.unit/test_ras.o: ../common/uint64_t_hash.h ../common/parameters.h
test.unit/test_ras.o: ../common/param_factory.h ../common/options.h
test.unit/test_series.o: lib/series.h lib/circbuf.h
test.unit/test_timewheel.o: mem_lib/timewheel.h ../common/liboptions.h
test.unit/test_timewheel.o: ../common/kill.h ../common/static_set.h
test.unit/test_timewheel.o: ../common/lua_register.h ../common/lua.hpp
test.unit/test_timewheel.o: ../common/units.h ../common/cast.h
test.unit/test_timewheel.o: ../common/uint64_t_hash.h ../common/parameters.h
test.unit/test_timewheel.o: ../common/param_factory.h ../common/options.h
test.unit/test_timewheel.o: lib/memory_interface.h lib/memory_access.h
test.unit/test_timewheel.o: ../common/dump_gzip.h ../common/read_gzip.h
test.unit/test_timewheel.o: lib/inline.h lib/memory_trace.h
test.unit/test_timewheel.o: ../common/libmetric.h ../common/metric.h
test.unit/test_timewheel.o: ../common/tagged_metric.h ../common/heartbeat.h
test.unit/test_timewheel.o: ../common/aggregator.h hw_model/state_observer.h
test.unit/test_timewheel.o: hw_model/states.h lib/logger.h lib/selector.h
test.unit/test_timewheel.o: main/cotson.h mem_lib/cache_line.h lib/hashmap.h
test.unit/test_token_queue.o: parsing/token_queue.h
test.unit/test_tokens.o: parsing/tokens.h
