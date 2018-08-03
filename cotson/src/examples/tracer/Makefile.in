ROOT=$(shell cd ../..; pwd)

all: abaeterno-tracer.so tracer_app
	echo "export D=`pwd`" > run.sh; \
	cat run.sh.in >> run.sh; \
	$(ROOT)/../bin/cotson cotson_tracer.in

INCLUDE = -I$(ROOT)/common -I$(ROOT)/libluabind \
          -I$(ROOT)/abaeterno/lib -I$(ROOT)/abaeterno/main \
          -I/usr/include/lua5.1

include  $(ROOT)/Make.conf

AALIBS = $(ROOT)/common/libaacommon.a \
         $(ROOT)/disksim/lib/libdisksim-cotson.a \
         $(ROOT)/distorm/libdistorm.a \
         $(ROOT)/libluabind/libluabind.a \
         $(ROOT)/slirp/libslirp.a

ALL_AALIBS = -Wl,-whole-archive  $(AALIBS) -Wl,-no-whole-archive

SHLIBS = -lpqxx -lsqlite3 -llua5.1 $(THREAD_LIBS) 

LINKOPTS = \
	-Wl,-rpath,./linuxlibs -shared \
	-Wl,--allow-multiple-definition \
    -Wl,-whole-archive

tracer_example.o: tracer_example.cpp

tracer_app: cotson_tracer.cpp cotson_tracer.h
	$(CXX) -o $@ cotson_tracer.cpp

abaeterno-tracer.so: tracer_example.o $(ROOT)/abaeterno/abaeterno.a 
	@echo "Linking $@";\
	$(CXX) $(LINKOPTS) -o $@ $^ $(ALL_AALIBS) $(SHLIBS)

clean:
	$(RM) abaeterno-tracer.so tracer_example.o cotson_tracer.o tracer_app *.log cotson_tracer run.sh

cleanup: clean
# DO NOT DELETE
