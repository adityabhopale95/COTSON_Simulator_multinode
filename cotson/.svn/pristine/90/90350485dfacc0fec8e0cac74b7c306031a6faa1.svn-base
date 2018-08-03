.PHONY: build cleanup distclean veryclean killpsall

all: build

.PHONY: run run_%
EXAMPLES= \
	functional \
	mem_tracer mem_tracer2 trace_stats  \
	one_cpu_simple smarts dynamic multiple_cpu_interval \
	twonodes twohet \
	tracer
EXAMPLESALL=$(EXAMPLES) slirp sqlite netperf atracer custom_asm twonodes_interact twonodes_noscreen twonodes_novnc
NOINT=0

run: $(addprefix run_, $(EXAMPLES))
cleanup: $(addprefix cleanup_, $(EXAMPLESALL))
veryclean: killpsall $(addprefix cleanup_, $(EXAMPLESALL))

killpsall:
	ps axuc |awk '/vnc|screen|simnow/{print $$2}'| xargs --no-run-if-empty kill -9; 
	
run_functional:
	@echo "Simple example how to use COTSon for interactive simulation."; \
	echo "Try running some simnow commands and then 'quit' to exit simulation"; \
	echo "Type <return> to start or <ctrl-c> to abort"; [ "$(NOINT)" = "0" ] && read a; \
	if [ "$(NOINT)" = "0" ]; then ../../bin/cotson functional.in; fi

cleanup_functional:

run_slirp:
	@echo "Simple example how to use COTSon with the building slirp (NAT) interface."; \
	echo "Type <return> to start or <ctrl-c> to abort"; [ "$(NOINT)" = "0" ] && read a; \
	../../bin/cotson slirp.in

cleanup_slirp:


run_mem_tracer:
	@OUTFILE=`mktemp -u /tmp/cotson_mem_tracer_XXXXXXXXXX.txt.gz`; \
	echo "The mem_tracer outputs a memory trace to $$OUTFILE"; \
	echo "Type <return> to start or <ctrl-c> to abort"; [ "$(NOINT)" = "0" ] && read a; \
	../../bin/cotson OUTFILE=\"$$OUTFILE\" mem_tracer.in; \
	if [ "$(NOINT)" = "0" ]; then zless $$OUTFILE; fi

cleanup_mem_tracer:

run_mem_tracer2:
	@OUTFILE=`mktemp -u /tmp/cotson_mem_tracer2_XXXXXXXXXX.txt.gz`; \
	echo "The mem_tracer2 outputs a memory trace to $$OUTFILE"; \
	echo "Type <return> to start or <ctrl-c> to abort"; \
	[ "$(NOINT)" = "0" ] && { aa=""; read a; } || aa="display=\"auto\""; \
	../../bin/cotson OUTFILE=\"$$OUTFILE\" mem_tracer2.in $$aa; \
	if [ "$(NOINT)" = "0" ]; then zless $$OUTFILE; fi

cleanup_mem_tracer2:

run_trace_stats:
	@OUTFILEPREF=`mktemp -u /tmp/cotson_trace_stats_XXXXXXXXXX`; \
	echo "The trace_stats example outputs a disassembled trace to "; \
	echo "$$OUTFILEPREF.*.txt.gz (one per CPU)"; \
	echo "Type <return> to start or <ctrl-c> to abort"; [ "$(NOINT)" = "0" ] && read a; \
	../../bin/cotson OUTFILEPREF=\"$$OUTFILEPREF\" trace_stats.in; \
	if [ "$(NOINT)" = "0" ]; then zless $$OUTFILEPREF.0.txt.gz; fi

cleanup_trace_stats:

run_twonodes:
	@echo "Running a cluster of 2 nodes pinging each other"; \
	echo "Simulation runs in background -- be patient (a few minutes)"; \
	isoinfo -x '/COTSON_G.TGZ;1' -i ../../data/cotson-guest-tools.iso|tar xzf - usr/bin/cotson_tracer -O >/tmp/cotson_tracer 2>/dev/null; \
	/bin/rm -f run_twonodes.log /tmp/twonodes.db 2>/dev/null; \
	DBFILE=`mktemp -u /tmp/cotson_twonodes1_XXXXXXXXXX.db`; \
	NTFILE=`mktemp -u /tmp/cotson_twonodes1_XXXXXXXXXX-trace.gz`; \
	echo "DBFILE=\"$$DBFILE\"  NTFILE=\"$$NTFILE\""; \
	echo "Type <return> to start or <ctrl-c> to abort"; [ "$(NOINT)" = "0" ] && read a;\
	( (sleep 10; if [ -s ./twonodes-ctrl ]; then echo "Firing console view"; ./twonodes-ctrl view; fi) &); \
	../../bin/cotson DBFILE=\"$$DBFILE\" NTFILE=\"$$NTFILE\" twonodes.in > run_twonodes.log 2>&1 ; \
	if [ -s $$NTFILE ]; then echo "Dumping the network trace"; \
	../tools/traceplayer $$NTFILE 0 1; else echo "WARNING: no network-trace file."; fi

cleanup_twonodes:
	@echo "Cleanup twonodes example..."; \
	/bin/rm -rf twonodes-* twonodes.me* twonodes.no* run_twonodes.log err.med.*log err.node.*log twonodes.*log /tmp/twonodes.db /tmp/net-trace.gz /tmp/vnc* /tmp/cotson_twonodes1_* /tmp/X* /tmp/.X* 2>/dev/null; \
        echo "...done."

run_twonodes_novnc:
	@echo "Running a cluster of 2 nodes pinging each other (NO VNC)"; \
	echo "Simulation runs in background -- be patient (a few minutes)"; \
	isoinfo -x '/COTSON_G.TGZ;1' -i ../../data/cotson-guest-tools.iso|tar xzf - usr/bin/cotson_tracer -O >/tmp/cotson_tracer 2>/dev/null; \
	/bin/rm -f run_twonodes_novnc.log /tmp/twonodes_novnc.db 2>/dev/null; \
	DBFILE=`mktemp -u /tmp/cotson_twonodes_novnc_XXXXXXXXXX.db`; \
	NTFILE=`mktemp -u /tmp/cotson_twonodes_novnc_XXXXXXXXXX-trace.gz`; \
	echo "DBFILE=\"$$DBFILE\"  NTFILE=\"$$NTFILE\""; \
	echo "Type <return> to start or <ctrl-c> to abort"; [ "$(NOINT)" = "0" ] && read a; \
	( (sleep 10; if [ -s ./twonodes_novnc-ctrl ]; then echo "Firing console view"; ./twonodes_novnc-ctrl; fi) &); \
	../../bin/cotson DBFILE=\"$$DBFILE\" NTFILE=\"$$NTFILE\" twonodes_novnc.in > run_twonodes_novnc.log 2>&1 ; \
	if [ -s $$NTFILE ]; then echo "Dumping the network trace"; \
	../tools/traceplayer $$NTFILE 0 1; else echo "WARNING: no network-trace file."; fi

cleanup_twonodes_novnc:
	@echo "Cleanup twonodes_novnc example..."; \
	/bin/rm -rf twonodes_novnc_* twonodes_novnc-* twonodes_novnc.me* twonodes_novnc.no* run_twonodes_novnc.log err.med.*log err.node.*log twonodes_novnc.*log /tmp/twonodes_novnc.db /tmp/net-trace.gz /tmp/vnc* /tmp/cotson_twonodes_novnc* /tmp/X* /tmp/.X* 2>/dev/null; \
        echo "...done."

run_twonodes_noscreen:
	@echo "Running a cluster of 2 nodes pinging each other (NO SCREEN)"; \
	echo "Simulation runs in background -- be patient (a few minutes)"; \
	isoinfo -x '/COTSON_G.TGZ;1' -i ../../data/cotson-guest-tools.iso|tar xzf - usr/bin/cotson_tracer -O >/tmp/cotson_tracer 2>/dev/null; \
	/bin/rm -f run_twonodes_noscreen.log /tmp/twonodes_noscreen.db 2>/dev/null; \
	DBFILE=`mktemp -u /tmp/cotson_twonodes_noscreen_XXXXXXXXXX.db`; \
	NTFILE=`mktemp -u /tmp/cotson_twonodes_noscreen_XXXXXXXXXX-trace.gz`; \
	echo "DBFILE=\"$$DBFILE\"  NTFILE=\"$$NTFILE\""; \
	echo "Type <return> to start or <ctrl-c> to abort"; [ "$(NOINT)" = "0" ] && read a; \
	( (sleep 10; if [ -s ./twonodes_noscreen-ctrl ]; then echo "Firing console view"; ./twonodes_noscreen-ctrl; fi) &); \
	../../bin/cotson DBFILE=\"$$DBFILE\" NTFILE=\"$$NTFILE\" twonodes_noscreen.in > run_twonodes_noscreen.log 2>&1 ; \
	if [ -s $$NTFILE ]; then echo "Dumping the network trace"; \
	../tools/traceplayer $$NTFILE 0 1; else echo "WARNING: no network-trace file."; fi

cleanup_twonodes_noscreen:
	@echo "Cleanup twonodes_noscreen example..."; \
	/bin/rm -rf twonodes_noscreen_* twonodes_noscreen-* twonodes_noscreen.me* twonodes_noscreen.no* run_twonodes_noscreen.log err.med.*log err.node.*log twonodes_noscreen.*log /tmp/twonodes_noscreen.db /tmp/net-trace.gz /tmp/vnc* /tmp/cotson_twonodes_noscreen* /tmp/X* /tmp/.X* 2>/dev/null; \
        echo "...done."

run_twohet:
	@echo "Running a cluster of 2 heterogeneous nodes pinging each other"; \
	echo "Simulation runs in background -- be patient (a few minutes)"; \
	isoinfo -x '/COTSON_G.TGZ;1' -i ../../data/cotson-guest-tools.iso|tar xzf - usr/bin/cotson_tracer -O >/tmp/cotson_tracer 2>/dev/null; \
	/bin/rm -f run_twohet.log /tmp/twohet.db 2>/dev/null; \
	DBFILE=`mktemp -u /tmp/cotson_twohet_XXXXXXXXXX.db`; \
	NTFILE=`mktemp -u /tmp/cotson_twohet_XXXXXXXXXX-trace.gz`; \
	echo "DBFILE=\"$$DBFILE\"  NTFILE=\"$$NTFILE\""; \
	echo "Type <return> to start or <ctrl-c> to abort"; [ "$(NOINT)" = "0" ] && read a; \
	( (sleep 10; if [ -s ./twohet-ctrl ]; then echo "Firing console view"; ./twohet-ctrl view; fi) &); \
	../../bin/cotson DBFILE=\"$$DBFILE\" NTFILE=\"$$NTFILE\" twohet.in > run_twohet.log 2>&1 ; \
	if [ -s $$NTFILE ]; then echo "Dumping the network trace"; \
	../tools/traceplayer $$NTFILE 0 1; else echo "WARNING: no network-trace file."; fi

cleanup_twohet:
	@echo "Cleanup twohet example..."; \
	/bin/rm -rf twohet-* run_twohet.log err.med.*log err.node.*log twohet.*log /tmp/twohet.db /tmp/net-trace.gz /tmp/vnc* /tmp/cotson_twohet* /tmp/X* /tmp/.X* 2>/dev/null; \
        echo "...done."

run_twonodes_interact:
	@echo "Running a cluster of 2 nodes (INTERACTIVE)"; \
	echo "Simulation runs in background -- be patient (a few minutes)"; \
	isoinfo -x '/COTSON_G.TGZ;1' -i ../../data/cotson-guest-tools.iso|tar xzf - usr/bin/cotson_tracer -O >/tmp/cotson_tracer 2>/dev/null; \
	/bin/rm -f run_twonodes_interact.log /tmp/twonodes_interact.db 2>/dev/null; \
	DBFILE=`mktemp -u /tmp/cotson_twonodes_interact_XXXXXXXXXX.db`; \
	NTFILE=`mktemp -u /tmp/cotson_twonodes_interact_XXXXXXXXXX-trace.gz`; \
	echo "DBFILE=\"$$DBFILE\"  NTFILE=\"$$NTFILE\""; \
	echo "Type <return> to start or <ctrl-c> to abort"; [ "$(NOINT)" = "0" ] && read a; \
	( (sleep 10; if [ -s ./twonodes_interact-ctrl ]; then echo "Firing console view"; ./twonodes_interact-ctrl view; fi) &); \
	../../bin/cotson DBFILE=\"$$DBFILE\" NTFILE=\"$$NTFILE\" twonodes_interact.in > run_twonodes_interact.log 2>&1 ; \
	if [ -s $$NTFILE ]; then echo "Dumping the network trace"; \
	../tools/traceplayer $$NTFILE 0 1; else echo "WARNING: no network-trace file."; fi

cleanup_twonodes_interact:
	@echo "Cleanup twonodes_interact example..."; \
	/bin/rm -rf twonodes_interact-* twonodes_interact-me* twonodes_interact-no* run_twonodes_interact.log err.med.*log err.node.*log twonodes_interact.*log /tmp/cotson_twonodes_interact.db /tmp/cotson_twonodes_interact-trace.gz /tmp/vnc* /tmp/X* /tmp/.X*  2>/dev/null; \
        echo "...done."


run_netperf:
	@echo "Running the netperf benchmark (two nodes)"; \
	echo "Simulation runs in background -- be patient (a few minutes)"; \
	rm -rf run_netperf.log /tmp/netperf.db 2>/dev/null; \
	NPDIR=`mktemp -d /tmp/cotson_netperf_XXXXXXXX`; \
	DBFILE=`mktemp -u /tmp/cotson_netperf_XXXXXXX.db`; \
	echo "DBFILE=\"$$DBFILE\"  NPDIR=\"$$NPDIR\""; \
	echo "Type <return> to start or <ctrl-c> to abort"; [ "$(NOINT)" = "0" ] && read a; \
	tar xfj np.tbz -C $$NPDIR; \
	../../bin/cotson DBFILE=\"$$DBFILE\" NPDIR=\"$$NPDIR\" netperf.in > run_netperf.log 2>&1; \
	../tools/ipcplot file:$$DBFILE 1 -g 0.1 -mm ipc -r 0 -x11; \
	../tools/ipcplot file:$$DBFILE 1 -g 0.1 -mm packets -r 0 -x11

cleanup_netperf:
	@echo "Cleanup netperf example..."; \
	/bin/rm -rf np-* np-me* np-no* run_np.log err.med.*log err.node.*log np.*log /tmp/np.db /tmp/net-trace.gz /tmp/vnc* /tmp/cotson_netperf* /tmp/X* /tmp/.X* 2>/dev/null; \
        echo "...done."

run_sqlite:
	@echo "Running 3 times an experiment (different samplers) writing to sqlite DB"; \
	echo "At the end, the IPC is plotted"; \
	/bin/rm -f /tmp/test.db 2> /dev/null;  \
	TMP_DB_NAME=`mktemp -u /tmp/cotson_sqlite_XXXXXXXXXX.txt.gz`; \
	echo "TMP_DB_NAME=\"$$TMP_DB_NAME\""; \
	echo "Type <return> to start or <ctrl-c> to abort"; [ "$(NOINT)" = "0" ] && read a; \
	(( ../../bin/cotson DBFILE=\"$$TMP_DB_NAME\" EXP=1 sqlite.in; \
	  if [ -s $$TMP_DB_NAME ]; then ../tools/ipcplot file:$$TMP_DB_NAME 1 -g 0.01 -r 0 -x11; fi ) & );  \
	(( ../../bin/cotson DBFILE=\"$$TMP_DB_NAME\" EXP=2 sqlite.in; \
	  if [ -s $$TMP_DB_NAME ]; then ../tools/ipcplot file:$$TMP_DB_NAME 2 -g 0.01 -r 0 -x11; fi ) & ); \
	(( ../../bin/cotson DBFILE=\"$$TMP_DB_NAME\" EXP=3 sqlite.in; \
	  if [ -s $$TMP_DB_NAME ]; then ../tools/ipcplot file:$$TMP_DB_NAME 3 -g 0.01 -r 0 -x11; fi ) & ); \
	  wait

cleanup_sqlite:
	@echo "Cleanup sqlite example..."; \
	/bin/rm -rf /tmp/cotson_sqlite_* sqlite*log exp-* 2>/dev/null; \
	echo "...done."

run_%:
	@echo "Running the $* example"; \
	echo "When done, look at node.*.$*.log*"; \
	echo "Type <return> to start or <ctrl-c> to abort"; [ "$(NOINT)" = "0" ] && read a; \
	../../bin/cotson $*.in

cleanup_%:
	@echo "Cleanup $* example..."; \
	/bin/rm -rf /tmp/cotson_$*_* node*$**log_hb* *$**log abaeterno.so exp-*.gp exp-*.dat twonodes-*
	echo "...done"

run_tracer:
	$(MAKE) -C tracer

cleanup_tracer:
	@echo "Cleanup tracer..."; \
	$(MAKE) -C tracer cleanup; \
	echo "...done"

run_atracer:
	$(MAKE) -C atracer

cleanup_atracer:
	@echo "Cleanup atracer..."; \
	$(MAKE) -C atracer cleanup; \
	echo "...done"

run_custom_asm:
	@echo "Running the custom_asm example"; \
	echo "Type <return> to start or <ctrl-c> to abort"; [ "$(NOINT)" = "0" ] && read a; \
	$(MAKE) -C custom_asm

cleanup_custom_asm:
	@echo "Cleanup custom_asm..."; \
	$(MAKE) -C custom_asm cleanup; \
	echo "...done"

build: ../abaeterno/abaeterno.so
	ln -fs ../abaeterno/abaeterno.so .

install: 

dep:

clean: cleanup

distclean: cleanup
