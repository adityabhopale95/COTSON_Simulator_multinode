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

# Trying out examples for single node

In the directory *cotson/src/examples* there are various examples present in order to get acquainted with the various applications cotson can be used for.
To name a few, following examples can be tried out:
- functional.in
- one_cpu_simple.in
- multiple_cpu_interval.in
To run these examples, use:
```
$make run_<file_name> //eg: make run_one_cpu_simple
```
These examples show the functional simulation properly, and also help understand the timing models of cotson. Various kinds of samplers available in cotson are used in these examples.
Also, the examples above simulate with respect to a single machine i.e *single node*. 

## Observations for the above examples:
Looking into these files in a text editor, it can be observed that the entire configuration of the simulated system can be defined in these files. The configuration includes img to be used, number of processors in a machine, commands to be executed, memory hierarchy for the system, coherence protocol used, cpu and nic timers, etc. For details, check the COTSON_USER_GUIDE of how this script is written. Hence same executions can be used for various configurations.

### Problems encountered for these examples:
The simulation seems to be slow for examples involving use of various kinds of samplers, since it becomes dependent on the processing power of the host machine on which the simulator runs. The way to resolve this problem might be to profile the execution and accordingly make changes in the source code to reduce the overheads.

# Trying out examples for multinode:
In the directory *cotson/src/examples* there are a few examples on multinode simulation.
First, lets try the twonodes.in example:
```
$make run_twonodes
```
This execution aims to creation of two cotson nodes and execute a script which pings between these two nodes in specific intervals, and after the execution is completed, a complete trace of this communication is logged.

If the execution goes properly, two windows would be invoked, each representing a single cotson node.

## Observations for the twonodes example:
Looking into the configuration file *twonodes.in*, it can be seen that a few commands are different, and those are specific to a multinode simulation. The configuration for hardware specifications are similar to as seen in previous examples. Along with that, if *slirp* is set to *true*, the nodes can be *NAT* with the external network.

### Problems encountered for this example:
Many a times, the shell script isn't executed even if the nodes are invoked. Thus, repeatability of execution is not guaranteed. The only solution I found is rebooting the host system.

# Benchmarking Systems

## MPI benchmark:
The following MPI based benchmark was considered:
https://github.com/LLNL/mpiBench

### Testing of MPI on native system:
This benchmark was tested on a native system on a single machine at first. The MPI package *mpich* was installed on the native system. *mpirun* command was used, wherein attributes, such as *number of processes*, was used, wherein, number of processes tried were 1,2,4 and 8. 
Later on, this benchmark was run on two independent systems which were present on a single network. So, the benchmark was run on a cluster of 2 nodes. For this, **passwordless ssh** was set between the two machines by exchanging *ssh keys*. Then, a directory was shared between these two systems using **NFS**(Network File System). Also, hostnames were configured on both these machines.
Using the same *mpirun* command on one of the machines and using the attribute *hosts*, the hostnames of theses machines are specified on which the benchmark was run. Also, 1,2,4 and 8 processes per node were benchmarked.

### Running MPI Benchmark on COTSon:
- In order to run this benchmark on COTSon, first all the packages related to MPI needs to be installed on the images that would be run on COTSon nodes. Then, it's important to set passwordless connection between the two nodes that need to be connected. The images that are already present in the downloaded COTSon package are outdated (Ubuntu 9.10), thus, new images were needed to be downloaded.
- After these images were downloaded, MPI packages were installed 
