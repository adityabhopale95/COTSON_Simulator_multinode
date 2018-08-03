SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
INCLUDE = -I.
DEPINC = -I.

all: libslirp.a

include ../Make.conf

install:

libslirp.a: $(OBJS)
	$(RM) $@; $(AR) $@ $(OBJS)

clean:
	$(RM) $(OBJS) libslirp.a

distclean: clean

# DO NOT DELETE

bootp.o: ./slirp.h config.h slirp_config.h debug.h ip.h tcp.h tcp_var.h tcpip.h
bootp.o: tcp_timer.h udp.h icmp_var.h mbuf.h sbuf.h socket.h if.h main.h misc.h
bootp.o: ctl.h bootp.h tftp.h libslirp.h
cksum.o: ./slirp.h config.h slirp_config.h debug.h ip.h tcp.h tcp_var.h tcpip.h
cksum.o: tcp_timer.h udp.h icmp_var.h mbuf.h sbuf.h socket.h if.h main.h misc.h
cksum.o: ctl.h bootp.h tftp.h libslirp.h
debug.o: ./slirp.h config.h slirp_config.h debug.h ip.h tcp.h tcp_var.h tcpip.h
debug.o: tcp_timer.h udp.h icmp_var.h mbuf.h sbuf.h socket.h if.h main.h misc.h
debug.o: ctl.h bootp.h tftp.h libslirp.h
if.o: ./slirp.h config.h slirp_config.h debug.h ip.h tcp.h tcp_var.h tcpip.h
if.o: tcp_timer.h udp.h icmp_var.h mbuf.h sbuf.h socket.h if.h main.h misc.h
if.o: ctl.h bootp.h tftp.h libslirp.h
ip_icmp.o: ./slirp.h config.h slirp_config.h debug.h ip.h tcp.h tcp_var.h
ip_icmp.o: tcpip.h tcp_timer.h udp.h icmp_var.h mbuf.h sbuf.h socket.h if.h
ip_icmp.o: main.h misc.h ctl.h bootp.h tftp.h libslirp.h ip_icmp.h
ip_input.o: ./slirp.h config.h slirp_config.h debug.h ip.h tcp.h tcp_var.h
ip_input.o: tcpip.h tcp_timer.h udp.h icmp_var.h mbuf.h sbuf.h socket.h if.h
ip_input.o: main.h misc.h ctl.h bootp.h tftp.h libslirp.h ip_icmp.h
ip_output.o: ./slirp.h config.h slirp_config.h debug.h ip.h tcp.h tcp_var.h
ip_output.o: tcpip.h tcp_timer.h udp.h icmp_var.h mbuf.h sbuf.h socket.h if.h
ip_output.o: main.h misc.h ctl.h bootp.h tftp.h libslirp.h
mbuf.o: ./slirp.h config.h slirp_config.h debug.h ip.h tcp.h tcp_var.h tcpip.h
mbuf.o: tcp_timer.h udp.h icmp_var.h mbuf.h sbuf.h socket.h if.h main.h misc.h
mbuf.o: ctl.h bootp.h tftp.h libslirp.h
misc.o: ./slirp.h config.h slirp_config.h debug.h ip.h tcp.h tcp_var.h tcpip.h
misc.o: tcp_timer.h udp.h icmp_var.h mbuf.h sbuf.h socket.h if.h main.h misc.h
misc.o: ctl.h bootp.h tftp.h libslirp.h
sbuf.o: ./slirp.h config.h slirp_config.h debug.h ip.h tcp.h tcp_var.h tcpip.h
sbuf.o: tcp_timer.h udp.h icmp_var.h mbuf.h sbuf.h socket.h if.h main.h misc.h
sbuf.o: ctl.h bootp.h tftp.h libslirp.h
slirp.o: ./slirp.h config.h slirp_config.h debug.h ip.h tcp.h tcp_var.h tcpip.h
slirp.o: tcp_timer.h udp.h icmp_var.h mbuf.h sbuf.h socket.h if.h main.h misc.h
slirp.o: ctl.h bootp.h tftp.h libslirp.h
socket.o: ./slirp.h config.h slirp_config.h debug.h ip.h tcp.h tcp_var.h
socket.o: tcpip.h tcp_timer.h udp.h icmp_var.h mbuf.h sbuf.h socket.h if.h
socket.o: main.h misc.h ctl.h bootp.h tftp.h libslirp.h ip_icmp.h
tcp_input.o: ./slirp.h config.h slirp_config.h debug.h ip.h tcp.h tcp_var.h
tcp_input.o: tcpip.h tcp_timer.h udp.h icmp_var.h mbuf.h sbuf.h socket.h if.h
tcp_input.o: main.h misc.h ctl.h bootp.h tftp.h libslirp.h ip_icmp.h
tcp_output.o: ./slirp.h config.h slirp_config.h debug.h ip.h tcp.h tcp_var.h
tcp_output.o: tcpip.h tcp_timer.h udp.h icmp_var.h mbuf.h sbuf.h socket.h if.h
tcp_output.o: main.h misc.h ctl.h bootp.h tftp.h libslirp.h
tcp_subr.o: ./slirp.h config.h slirp_config.h debug.h ip.h tcp.h tcp_var.h
tcp_subr.o: tcpip.h tcp_timer.h udp.h icmp_var.h mbuf.h sbuf.h socket.h if.h
tcp_subr.o: main.h misc.h ctl.h bootp.h tftp.h libslirp.h
tcp_timer.o: ./slirp.h config.h slirp_config.h debug.h ip.h tcp.h tcp_var.h
tcp_timer.o: tcpip.h tcp_timer.h udp.h icmp_var.h mbuf.h sbuf.h socket.h if.h
tcp_timer.o: main.h misc.h ctl.h bootp.h tftp.h libslirp.h
tftp.o: ./slirp.h config.h slirp_config.h debug.h ip.h tcp.h tcp_var.h tcpip.h
tftp.o: tcp_timer.h udp.h icmp_var.h mbuf.h sbuf.h socket.h if.h main.h misc.h
tftp.o: ctl.h bootp.h tftp.h libslirp.h
udp.o: ./slirp.h config.h slirp_config.h debug.h ip.h tcp.h tcp_var.h tcpip.h
udp.o: tcp_timer.h udp.h icmp_var.h mbuf.h sbuf.h socket.h if.h main.h misc.h
udp.o: ctl.h bootp.h tftp.h libslirp.h ip_icmp.h
