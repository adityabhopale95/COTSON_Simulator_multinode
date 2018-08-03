#!/bin/sh 

NODE=$1
TOTAL_NODES=$2

################# NODE 1 ##################
if [ $NODE -eq 1 ] ; then
   echo "Node #1"
   i="1"
   n=`expr $2 - 1`
   while [ "$i" -le "$n" ]; do
      i=`expr $i + 1`
      ping -i 0.2 -c 3 n$i | tee /tmp/ping.log
   done
   ./cotson_tracer 100 200 300 ## Example cpuid sent across the network 
   ping -i 0.2 -c 3 n$2 | tee -a /tmp/ping.log
else
################# NODE 2..N ##################
   echo "Node #$1"
   ii=`expr $1 + 1`
   if [ "$ii" -gt "$2" ]; then ii="1"; fi
   ping -c 2 n$ii | tee /tmp/ping.log
   while true; do
      echo "still working at `date`"
      sleep 1 
   done
fi
