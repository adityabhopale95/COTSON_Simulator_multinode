#!/bin/sh 

NODE=$1
TOTAL_NODES=$2

################# ANY NODE (loop forever) ##################
echo "Node $1"
while true; do
    echo "loop at `date`"
    sleep 1 
done
