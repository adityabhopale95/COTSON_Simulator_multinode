#!/bin/sh

NODE=$1
TOTAL_NODES=$2
xget /tmp/cotson_tracer cotson_tracer
chmod +x cotson_tracer

############## NODE 1 ###############
if [ $NODE -eq 1 ] ; then
    echo "Node #1"
    i="1"
    ping -i 0.2 -c 3 n2 | tee /tmp/ping.log
    ping -i 0.2 -c 3 n3 | tee -a /tmp/ping.log
    ./cotson_tracer 100 200 300
    ping -i 0.2 -c 4 n2 | tee -a /tmp/ping.log
    ping -i 0.2 -c 4 n3 | tee -a /tmp/ping.log
elif [ $NODE -eq 2 ] ; then
    echo "Node #$1"
    ping -c 2 n1
    ping -c 2 n3
    while true; do
	echo "still working at `date`"
	sleep 1
    done
else
    echo "Node #$1"
    ping -c 1 n1
    ping -c 1 n2
    while true; do
	echo "still working at `date`"
	sleep 1
    done
fi
