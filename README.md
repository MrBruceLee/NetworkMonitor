NetworkMonitor
================
ECE 595 Project

canvasjs-1
----------
CanvasJS source code

files
------
files that system used during running

record
------
store spark streaming record files

src
----
system source file
1. [source.cpp](./src/source.cpp) -- data collection code
2. [run.cc](./src/run.cc) -- data collection code for test case
3. [network_monitor.py](./src/network_monitor.py) -- spark streaming code
4. [control.cpp](./src/control.cpp) -- data integration code
5. [chart.html](./src/chart.html) -- visualization

test
----
test case

webpage
-------
top 500 webpage that can be used by this project



Usage
=====
You need three process to run the system:

1. data collection process
```
cd /<your path>/netowrk_monitor/src
./source
```

2. data analysis process
```
 /<your spark path>/spark-1.6.1/bin/spark-submit /<your path>/netowrk_monitor/src/network_monitor.py localhost 9999
```

3. control process
```
cd /<your path>/netowrk_monitor/src
./control
```
