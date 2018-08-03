#!/bin/bash
CPORT=50000
DPORT=50001
MPORT=50002
MCIP=239.200.1.1
NODES=10
TIMESTAMPS=10
mode=$1
debug=$2
/bin/rm -f /tmp/test.db

echo "########### Running test $mode ###############"
../mediator CPORT=$CPORT DPORT=$DPORT MPORT=$MPORT med.in > med$mode.log 2>&1 & 
sleep 1

if [[ $debug -eq 1 ]] ; then
    echo "b main" > gdb.in
    echo "r localhost $CPORT $MCIP $MPORT $NODES $TIMESTAMPS $mode" >> gdb.in
	gdb -x gdb.in medtester
else
    ./medtester localhost $CPORT $MCIP $MPORT $NODES $TIMESTAMPS $mode > test$mode.log 2>&1
fi
wait

c0=`grep 'CTRL' med$mode.log | wc -l`
d0=`grep 'DATA.*Peer' med$mode.log | wc -l`
c1=`grep CTRL test$mode.log | awk '{print $2}'`
d1=`grep DATA test$mode.log | awk '{print $2}'`

rv=0
if [[ $c0 -eq $c1 ]]; then 
    echo "Ctrl test $mode passed"
else
    echo "Ctrl test $mode failed: ctrl packs $c0 != $c1"
	rv=1
fi
if [[ $d0 -eq $d1 ]]; then 
    echo "Data test $mode passed"
else
    echo "Data test $mode failed: data packs $d0 != $d1"
	rv=1
fi
exit $rv

