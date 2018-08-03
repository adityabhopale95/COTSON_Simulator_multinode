#!/bin/sh

NODE=$1
TOTAL_NODES=$2

################ NODE 1 ###################

if [ $NODE -eq 1 ] ; then
   mpirun -n 2 --hosts n1,n2 ./mpiBench-master/mpiBench
fi
