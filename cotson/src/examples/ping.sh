#!/bin/sh 

NODE=$1
TOTAL_NODES=$2
xget /tmp/cotson_tracer cotson_tracer
chmod +x cotson_tracer
wget http://www.mpich.org/static/downloads/3.2.1/mpich-3.2.1.tar.gz
tar -xzf mpich-3.2.1 

################# NODE 1 ##################
if [ $NODE -eq 1 ] ; then
   echo "Node #1"
   echo "Before starting ping for node #1"
   ping -i 0.2 -c 2 n2 | tee /tmp/ping.log
   ./cotson_tracer 100 200 300 ## Example cpuid sent across the network 
   ping -i 0.2 -c 3 n2 | tee -a /tmp/ping.log
else
################# NODE 2..N ##################
   echo "Node #2"
   echo "Before starting ping for node #2"   
   ping -c 1 n1
   while true; do
      echo "still working at `date`"
      sleep 1 
   done
fi
