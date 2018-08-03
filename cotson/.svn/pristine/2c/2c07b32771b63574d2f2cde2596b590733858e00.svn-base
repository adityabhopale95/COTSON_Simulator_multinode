SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
DEPINC = -I. -I../common 
INCLUDE = $(DEPINC) -I../libluabind -I/usr/include/lua5.1
DEFS = 

GCCVERS480CHECK = $(shell expr `gcc -dumpversion | cut -f1-2 -d.` \>= 4.8)
ifeq "$(GCCVERS480CHECK)" "1"  #RG small patch to compile
	XOPTS = -Werror -Wno-error=unused-but-set-variable -Wno-deprecated-declarations
	XXOPTS = -std=c++11
endif


all: mediator

include ../Make.conf

install: mediator
	$(INSTALL) mediator $(PREFIX)/sbin

AALIBS=../common/libaacommon.a ../slirp/libslirp.a ../libluabind/libluabind.a 
mediator: $(OBJS) $(AALIBS)
	$(CXX) -o $(@) $(OBJS) \
    -Wl,--whole-archive $(AALIBS) -Wl,-no-whole-archive \
	-lpqxx -lsqlite3 -llua5.1 -lz -ldl \
	$(THREAD_LIBS) 

clean:
	$(RM) $(OBJS) mediator 

.PHONY: test
test:
	$(MAKE) -C test test

distclean: clean

# DO NOT DELETE

control_packet_processor.o: control_packet_processor.h packet_processor.h
control_packet_processor.o: event_handler.h mpacket.h switch.h net_typedefs.h
control_packet_processor.o: ../common/mac.h node.h ../common/metric.h
control_packet_processor.o: net_timing_model.h ../common/timing_message.h
control_packet_processor.o: stats.h log.h
data_packet_processor.o: data_packet_processor.h packet_processor.h
data_packet_processor.o: event_handler.h mpacket.h switch.h net_typedefs.h
data_packet_processor.o: ../common/mac.h node.h ../common/metric.h
data_packet_processor.o: net_timing_model.h ../common/timing_message.h stats.h
data_packet_processor.o: log.h
mediator.o: ../common/liboptions.h ../common/kill.h ../common/static_set.h
mediator.o: ../common/lua_register.h ../common/lua.hpp ../common/units.h
mediator.o: ../common/cast.h ../common/uint64_t_hash.h ../common/parameters.h
mediator.o: ../common/param_factory.h ../common/options.h mediator_server.h
mediator.o: reactor.h event_handler.h switch.h net_typedefs.h ../common/mac.h
mediator.o: node.h mpacket.h ../common/metric.h net_timing_model.h stats.h
mediator.o: packet_acceptor.h control_packet_processor.h packet_processor.h
mediator.o: ../common/timing_message.h data_packet_processor.h slirp_acceptor.h
mediator.o: vde_acceptor.h scheduler.h
mediator_server.o: mediator_server.h reactor.h event_handler.h switch.h
mediator_server.o: net_typedefs.h ../common/mac.h node.h mpacket.h
mediator_server.o: ../common/metric.h net_timing_model.h stats.h
mediator_server.o: packet_acceptor.h control_packet_processor.h
mediator_server.o: packet_processor.h ../common/timing_message.h
mediator_server.o: data_packet_processor.h slirp_acceptor.h vde_acceptor.h
mediator_server.o: scheduler.h ../common/liboptions.h ../common/kill.h
mediator_server.o: ../common/static_set.h ../common/lua_register.h
mediator_server.o: ../common/lua.hpp ../common/units.h ../common/cast.h
mediator_server.o: ../common/uint64_t_hash.h ../common/parameters.h
mediator_server.o: ../common/param_factory.h ../common/options.h log.h
med_lua_export.o: ../common/lua.hpp
net_constant.o: net_timing_model.h net_typedefs.h ../common/mac.h
net_constant.o: ../common/metric.h ../common/param_factory.h
net_constant.o: ../common/parameters.h ../common/cast.h ../common/units.h
net_constant.o: ../common/lua.hpp ../common/kill.h
net_dest_based.o: net_timing_model.h net_typedefs.h ../common/mac.h
net_dest_based.o: ../common/metric.h ../common/param_factory.h
net_dest_based.o: ../common/parameters.h ../common/cast.h ../common/units.h
net_dest_based.o: ../common/lua.hpp ../common/kill.h
net_new_dest_based.o: net_timing_model.h net_typedefs.h ../common/mac.h
net_new_dest_based.o: ../common/metric.h ../common/param_factory.h
net_new_dest_based.o: ../common/parameters.h ../common/cast.h ../common/units.h
net_new_dest_based.o: ../common/lua.hpp ../common/kill.h
net_simple_switch.o: net_timing_model.h net_typedefs.h ../common/mac.h
net_simple_switch.o: ../common/metric.h ../common/liboptions.h ../common/kill.h
net_simple_switch.o: ../common/static_set.h ../common/lua_register.h
net_simple_switch.o: ../common/lua.hpp ../common/units.h ../common/cast.h
net_simple_switch.o: ../common/uint64_t_hash.h ../common/parameters.h
net_simple_switch.o: ../common/param_factory.h ../common/options.h
net_timing_model.o: net_timing_model.h net_typedefs.h ../common/mac.h
net_timing_model.o: ../common/metric.h ../common/kill.h
node.o: node.h net_typedefs.h ../common/mac.h stats.h ../common/metric.h
packet_acceptor.o: reactor.h event_handler.h packet_processor.h mpacket.h
packet_acceptor.o: switch.h net_typedefs.h ../common/mac.h node.h
packet_acceptor.o: ../common/metric.h net_timing_model.h
packet_acceptor.o: ../common/timing_message.h packet_acceptor.h
packet_processor.o: packet_processor.h event_handler.h mpacket.h switch.h
packet_processor.o: net_typedefs.h ../common/mac.h node.h ../common/metric.h
packet_processor.o: net_timing_model.h ../common/timing_message.h
reactor.o: reactor.h event_handler.h stats.h ../common/metric.h
scheduler.o: scheduler.h net_typedefs.h ../common/mac.h mpacket.h node.h
scheduler.o: switch.h ../common/metric.h net_timing_model.h stats.h log.h
slirp_acceptor.o: reactor.h event_handler.h packet_processor.h mpacket.h
slirp_acceptor.o: switch.h net_typedefs.h ../common/mac.h node.h
slirp_acceptor.o: ../common/metric.h net_timing_model.h
slirp_acceptor.o: ../common/timing_message.h slirp_acceptor.h
slirp_acceptor.o: ../slirp/libslirp.h
stats.o: stats.h ../common/metric.h
switch.o: switch.h net_typedefs.h ../common/mac.h node.h mpacket.h
switch.o: ../common/metric.h net_timing_model.h data_packet_processor.h
switch.o: packet_processor.h event_handler.h ../common/timing_message.h
switch.o: scheduler.h stats.h log.h ../common/dump_gzip.h
switch.o: ../common/liboptions.h ../common/kill.h ../common/static_set.h
switch.o: ../common/lua_register.h ../common/lua.hpp ../common/units.h
switch.o: ../common/cast.h ../common/uint64_t_hash.h ../common/parameters.h
switch.o: ../common/param_factory.h ../common/options.h ../common/libmetric.h
switch.o: ../common/tagged_metric.h ../common/heartbeat.h
switch.o: ../common/aggregator.h
vde_acceptor.o: reactor.h event_handler.h packet_processor.h mpacket.h switch.h
vde_acceptor.o: net_typedefs.h ../common/mac.h node.h ../common/metric.h
vde_acceptor.o: net_timing_model.h ../common/timing_message.h vde_acceptor.h
