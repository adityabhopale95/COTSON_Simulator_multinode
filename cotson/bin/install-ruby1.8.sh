#!/bin/bash
apt-get install software-properties-common
apt-add-repository ppa:brightbox/ruby-ng
apt-get update
apt-get install ruby1.8 rubygems1.8
