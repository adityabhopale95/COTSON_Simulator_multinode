ROOT=$(shell cd ../..; pwd)

all: custom_asm.so asm_example
	echo "xget $(ROOT)/examples/custom_asm/asm_example asm_example; chmod +x asm_example; ./asm_example" > run.sh; \
	$(ROOT)/../bin/cotson example.lua; \
	cat node.1.stdout.log

INCLUDE = -I$(ROOT)/common -I$(ROOT)/libluabind \
          -I$(ROOT)/abaeterno/lib -I$(ROOT)/abaeterno/main \
		  -I$(ROOT)/abaeterno/hw_model \
		  -I$(ROOT)/abaeterno/timer_cpu -I$(ROOT)/abaeterno/timer_dev \
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

custom_asm.o: custom_asm.cpp custom_asm.h regtab.h

asm_example: asm_example.cpp custom_asm.h
	$(CXX) -O3 -o $@ asm_example.cpp

custom_asm.so: custom_asm.o $(ROOT)/abaeterno/abaeterno.a 
	@echo "Linking $@";\
	$(CXX) $(LINKOPTS) -o $@ $^ $(ALL_AALIBS) $(SHLIBS)

clean:
	$(RM) custom_asm.o custom_asm.so asm_example.o asm_example *.log run.sh

cleanup: clean

# DO NOT DELETE
