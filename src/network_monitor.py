from __future__ import print_function

import sys

from pyspark import SparkContext
from pyspark.streaming import StreamingContext

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: network_monitor.py <hostname> <port>", file=sys.stderr)
        exit(-1)

    sc = SparkContext(appName="NetworkMonitor")
    ssc = StreamingContext(sc, 7)
        
    lines = ssc.socketTextStream(sys.argv[1], int(sys.argv[2]))

    filteredStream = lines.filter(lambda line: "icmp_seq=" in line)
        
    def parseInformation(s):
        idxStart = s.find("from ") + 5
        idxEnd = idxStart
        while s[idxEnd] != ':':
            idxEnd += 1
        IP = s[idxStart:idxEnd]
            
        idxStart = s.find("ttl=") + 4
        idxEnd = idxStart
        while s[idxEnd] != ' ':
            idxEnd += 1
        TTL = s[idxStart:idxEnd]
            
        idxStart = s.find("time=") + 5
        idxEnd = idxStart
        while s[idxEnd] != ' ':
            idxEnd += 1
        RTT = s[idxStart:idxEnd]

        return IP + '@' + TTL + '@' + RTT + '@'

    mappedInformation = filteredStream.map(parseInformation)
    mappedInformation.saveAsTextFiles("/Users/lilinzhe/Desktop/netowrk_monitor/record/Ping");
    mappedInformation.pprint()
    
    ssc.start()
    ssc.awaitTermination()
