# COTSON_Simulator_multinode
This repository explains use of a full system simulator to simulate benchmarks in a multinode environment

# What is COTSon?

COTSon is a full system simulation infrastructure developed by HP Labs to model complete computing systems ranging from multicore nodes up to scale-out clusters with complete network simulation. It is a pluggable architecture, in which end users can can develop their own modules, thus providing a very flexible simulation platform for a wide variety of modeling needs. The functional emulation is based on AMD's SimNow and supports x86 and x86_64 platforms.

# Installation of packages

Firstly, install cotson and AMD SimNow packages by following the steps in COTSON_USER_GUIDE.

After the installation is complete use the following commands to check if cotson is installed properly:
```
$cd cotson/src/examples
$make run_functional
```

If the packages are installed properly, a AMD SimNOW window is invoked, wherein an option to run the simulation is present at the top.

# Trying out examples

In the directory *cotson/src/examples* there are various examples present in order to get acquainted with the various applications cotson can be used for.
To name a few, following examples can be tried out:
- functional.in
- one_cpu_simple.in
- multiple_cpu_interval.in
To run these examples, use:
```
$make run_<file_name> //eg: make run_one_cpu_simple
```
These examples show the functional simulation properly, and also help understand the timing models of cotson. Various kinds of timers available in cotson are used in these examples.
Also, the examples above simulate with respect to a single machine i.e *single node*. 

## Observations for the above examples:
Looking into these files in a text editor, it can be observed that
