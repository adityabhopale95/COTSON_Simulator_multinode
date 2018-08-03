INCLUDE = -I. -I../common
DEPINC = -I. -I../common -I../network
SRCS = traceplayer.cpp pg_experiment.cpp memtrace.cpp

all: traceplayer pg_experiment memtrace

include ../Make.conf

install: traceplayer pg_experiment ipcplot
	$(INSTALL) traceplayer $(PREFIX)/sbin; \
	$(INSTALL) pg_experiment $(PREFIX)/sbin; \
	$(CP) ipcplot $(PREFIX)/sbin

traceplayer: traceplayer.cpp
	$(CXX) $(CXXFLAGS) -I ../network -o traceplayer $< -lz

pg_experiment: pg_experiment.cpp
	$(CXX) $(CXXFLAGS) -o pg_experiment $< -lpqxx

memtrace: memtrace.cpp ../common/read_gzip.h
	$(CXX) $(CXXFLAGS) -I ../network -o memtrace $< -lz

clean:
	$(RM) $(OBJS) pg_experiment traceplayer

distclean: clean

# DO NOT DELETE

traceplayer.o: ../common/read_gzip.h ../network/net_typedefs.h ../common/mac.h
memtrace.o: ../common/read_gzip.h
