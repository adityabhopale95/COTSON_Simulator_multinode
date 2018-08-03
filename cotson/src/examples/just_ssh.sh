#!/bin/sh/

NODE=$1
TOTAL_NODES=$2

if [ $NODE -eq 1 ] ; then
   ssh axiom@n2
fi
