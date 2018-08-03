#!/bin/sh

exec kvm -m 256 -smp 1 -drive file=karmic64.img -net nic,model=e1000 -net user -cdrom cotson-guest-tools.iso "$@"
