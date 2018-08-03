SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
# ROOT= $(shell cd ../..; pwd)
ROOT= ../..
DEPINC = -I. -I$(ROOT)/common
INCLUDE = $(DEPINC)
DEFS = 

all: medtester

include $(ROOT)/Make.conf

install: 

.PHONY: test test0 test1 test2 test3

test: test_0 test_1 test_2 test_3

test_%: medtester
	@./run.sh $*; sleep 1

medtester: $(OBJS)
	$(CXX) -o $(@) $(OBJS) \
	$(ROOT)/common/libaacommon.a

clean:
	$(RM) $(OBJS) medtester med*.log test*.log gdb.in

distclean: clean

# DO NOT DELETE

tester.o: ../../common/timing_message.h
