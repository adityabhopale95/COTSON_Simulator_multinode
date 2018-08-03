SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
INCLUDE = -I.
DEPINC = -I.
DEFS = -DSUPPORT_64BIT_OFFSET -D_DLL

all: libdistorm.a

include ../Make.conf

install:

libdistorm.a: $(OBJS)
	$(RM) $@; $(AR) $@ $(OBJS)

clean:
	$(RM) $(OBJS) libdistorm.a

distclean: clean

# DO NOT DELETE

decoder.o: decoder.h distorm_config.h wstring.h prefix.h textdefs.h x86defs.h
decoder.o: insts.h dis_instructions.h operands.h
dis_instructions.o: dis_instructions.h distorm_config.h decoder.h wstring.h
dis_instructions.o: prefix.h insts.h textdefs.h x86defs.h
distorm.o: distorm_config.h decoder.h wstring.h x86defs.h textdefs.h insts.h
distorm.o: dis_instructions.h prefix.h
insts.o: distorm_config.h insts.h dis_instructions.h decoder.h wstring.h
insts.o: prefix.h
operands.o: distorm_config.h operands.h decoder.h wstring.h prefix.h
operands.o: dis_instructions.h x86defs.h textdefs.h insts.h
prefix.o: prefix.h distorm_config.h decoder.h wstring.h textdefs.h x86defs.h
prefix.o: insts.h dis_instructions.h
textdefs.o: textdefs.h distorm_config.h wstring.h
wstring.o: wstring.h distorm_config.h
x86defs.o: x86defs.h distorm_config.h textdefs.h wstring.h insts.h
x86defs.o: dis_instructions.h decoder.h prefix.h
