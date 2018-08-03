TSRC=$(wildcard test.unit/*.cpp)
TEST = $(TSRC:.cpp=)
TOBJ = $(TSRC:.cpp=.o)

run: $(TEST)

clean:
	$(RM) $(TSRC:.cpp=) $(TSRC:.cpp=.log) $(TSRC:.cpp=.o) 

$(TEST):
	@echo "#### Linking test $@"; \
	$(TCXX) -o $@ $^ $(TLIBS); \
	echo "#### Running test $@ (output in $@.log)"; \
	$@ > $@.log

%.o: %.cpp
	@echo "$< ==> $@"; \
	$(TCXX) $(TCXXFLAGS) -c $< -o $@

include test.unit/make.deps

# DO NOT DELETE

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
test.unit/test_cache_hierarchy.o: lib/simplevec.h hw_model/trace_needs.h
test.unit/test_cache_hierarchy.o: lib/delegate.h mem_lib/storage_basic.h
test.unit/test_cache_hierarchy.o: mem_lib/storage.h lib/cache.h lib/debug_mem.h
test.unit/test_cache_hierarchy.o: lib/hashmap.h mem_lib/cache_line.h
test.unit/test_cache_hierarchy.o: mem_lib/cache_timing_basic.h
test.unit/test_cache_hierarchy.o: mem_lib/cache_impl.h mem_lib/timewheel.h
test.unit/test_cache_hierarchy.o: hw_model/machine.h hw_model/cpu.h
test.unit/test_cache_hierarchy.o: lib/lua_helpers.h hw_model/nic.h
test.unit/test_cache_hierarchy.o: timer_dev/nic_timer.h hw_model/disk.h
test.unit/test_cache_hierarchy.o: timer_dev/disk_timer.h ../common/error.h
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
test.unit/test_cache_impl.o: lib/instruction.h lib/simplevec.h
test.unit/test_cache_impl.o: hw_model/trace_needs.h lib/delegate.h
test.unit/test_cache_impl.o: mem_lib/storage_one_way.h mem_lib/storage.h
test.unit/test_cache_impl.o: lib/cache.h lib/debug_mem.h lib/hashmap.h
test.unit/test_cache_impl.o: mem_lib/cache_line.h mem_lib/storage_two_way.h
test.unit/test_cache_impl.o: mem_lib/storage_four_way.h mem_lib/storage_many.h
test.unit/test_cache_impl.o: mem_lib/storage_basic.h
test.unit/test_cache_impl.o: mem_lib/cache_timing_basic.h mem_lib/cache_impl.h
test.unit/test_cache_impl.o: mem_lib/timewheel.h hw_model/machine.h
test.unit/test_cache_impl.o: hw_model/cpu.h lib/lua_helpers.h hw_model/nic.h
test.unit/test_cache_impl.o: timer_dev/nic_timer.h hw_model/disk.h
test.unit/test_cache_impl.o: timer_dev/disk_timer.h ../common/error.h
test.unit/test_cache_impl.o: mem_lib/protocol.h mem_lib/bus.h
test.unit/test_cache_impl.o: mem_lib/simple_protocol.h
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
test.unit/test_cache_impl_wb.o: lib/instruction.h lib/simplevec.h
test.unit/test_cache_impl_wb.o: hw_model/trace_needs.h lib/delegate.h
test.unit/test_cache_impl_wb.o: mem_lib/storage_one_way.h mem_lib/storage.h
test.unit/test_cache_impl_wb.o: lib/cache.h lib/debug_mem.h lib/hashmap.h
test.unit/test_cache_impl_wb.o: mem_lib/cache_line.h mem_lib/storage_two_way.h
test.unit/test_cache_impl_wb.o: mem_lib/storage_four_way.h
test.unit/test_cache_impl_wb.o: mem_lib/storage_many.h mem_lib/storage_basic.h
test.unit/test_cache_impl_wb.o: mem_lib/cache_timing_basic.h
test.unit/test_cache_impl_wb.o: mem_lib/cache_impl.h mem_lib/timewheel.h
test.unit/test_cache_impl_wb.o: hw_model/machine.h hw_model/cpu.h
test.unit/test_cache_impl_wb.o: lib/lua_helpers.h hw_model/nic.h
test.unit/test_cache_impl_wb.o: timer_dev/nic_timer.h hw_model/disk.h
test.unit/test_cache_impl_wb.o: timer_dev/disk_timer.h ../common/error.h
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
test.unit/test_mesi_2levels.o: lib/instruction.h lib/simplevec.h
test.unit/test_mesi_2levels.o: hw_model/trace_needs.h lib/delegate.h
test.unit/test_mesi_2levels.o: mem_lib/storage_basic.h mem_lib/storage.h
test.unit/test_mesi_2levels.o: lib/cache.h lib/debug_mem.h lib/hashmap.h
test.unit/test_mesi_2levels.o: mem_lib/cache_line.h
test.unit/test_mesi_2levels.o: mem_lib/cache_timing_basic.h
test.unit/test_mesi_2levels.o: mem_lib/cache_impl.h mem_lib/timewheel.h
test.unit/test_mesi_2levels.o: hw_model/machine.h hw_model/cpu.h
test.unit/test_mesi_2levels.o: lib/lua_helpers.h hw_model/nic.h
test.unit/test_mesi_2levels.o: timer_dev/nic_timer.h hw_model/disk.h
test.unit/test_mesi_2levels.o: timer_dev/disk_timer.h ../common/error.h
test.unit/test_mesi_2levels.o: mem_lib/protocol.h mem_lib/bus.h mem_lib/mesi.h
test.unit/test_metric.o: ../common/libmetric.h ../common/metric.h
test.unit/test_metric.o: ../common/tagged_metric.h ../common/heartbeat.h
test.unit/test_metric.o: ../common/aggregator.h
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
test.unit/test_moesi_2levels.o: lib/instruction.h lib/simplevec.h
test.unit/test_moesi_2levels.o: hw_model/trace_needs.h lib/delegate.h
test.unit/test_moesi_2levels.o: mem_lib/storage_basic.h mem_lib/storage.h
test.unit/test_moesi_2levels.o: lib/cache.h lib/debug_mem.h lib/hashmap.h
test.unit/test_moesi_2levels.o: mem_lib/cache_line.h
test.unit/test_moesi_2levels.o: mem_lib/cache_timing_basic.h
test.unit/test_moesi_2levels.o: mem_lib/cache_impl.h mem_lib/timewheel.h
test.unit/test_moesi_2levels.o: hw_model/machine.h hw_model/cpu.h
test.unit/test_moesi_2levels.o: lib/lua_helpers.h hw_model/nic.h
test.unit/test_moesi_2levels.o: timer_dev/nic_timer.h hw_model/disk.h
test.unit/test_moesi_2levels.o: timer_dev/disk_timer.h ../common/error.h
test.unit/test_moesi_2levels.o: mem_lib/protocol.h mem_lib/bus.h
test.unit/test_moesi_2levels.o: mem_lib/moesi.h
test.unit/test_options.o: ../common/lua.hpp ../common/liboptions.h
test.unit/test_options.o: ../common/kill.h ../common/static_set.h
test.unit/test_options.o: ../common/lua_register.h ../common/units.h
test.unit/test_options.o: ../common/cast.h ../common/uint64_t_hash.h
test.unit/test_options.o: ../common/parameters.h ../common/param_factory.h
test.unit/test_options.o: ../common/options.h
test.unit/test_ras.o: lib/ras.h ../common/error.h ../common/liboptions.h
test.unit/test_ras.o: ../common/kill.h ../common/static_set.h
test.unit/test_ras.o: ../common/lua_register.h ../common/lua.hpp
test.unit/test_ras.o: ../common/units.h ../common/cast.h
test.unit/test_ras.o: ../common/uint64_t_hash.h ../common/parameters.h
test.unit/test_ras.o: ../common/param_factory.h ../common/options.h
test.unit/test_series.o: lib/series.h lib/circbuf.h
test.unit/test_tagged_metrics.o: ../common/libmetric.h ../common/metric.h
test.unit/test_tagged_metrics.o: ../common/tagged_metric.h
test.unit/test_tagged_metrics.o: ../common/heartbeat.h ../common/aggregator.h
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
