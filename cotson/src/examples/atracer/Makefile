ROOT=$(shell cd ../..; pwd)

all: add abaeterno-atracer.so
	echo "export D=`pwd`" > run.sh; \
	cat run.sh.in >> run.sh; \
	$(ROOT)/../bin/cotson atracer.in; \
	if [ -s node.1.atracer.log ]; then \
	    egrep "(timer.cycles|timer.inst)" node.1.atracer.log; \
	fi

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

add: add.c atracer.h
	$(CC) -o $@ add.c

abaeterno-atracer.so: $(ROOT)/abaeterno/abaeterno.a 
	@echo "Linking $@";\
	$(CXX) $(LINKOPTS) -o $@ $^ $(ALL_AALIBS) $(SHLIBS)

clean:
	$(RM) abaeterno-atracer.so add add.o *.log run.sh
# DO NOT DELETE
