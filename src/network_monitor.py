#   /Users/lilinzhe/Documents/spark-1.6.1/bin/spark-submit /Users/lilinzhe/Desktop/netowrk_monitor/src/network_monitor.py localhost 9999

from __future__ import print_function

from pyspark import SparkContext
from pyspark.streaming import StreamingContext

import sys

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: network_monitor.py <hostname> <port>", file=sys.stderr)
        exit(-1)

    sc = SparkContext(appName="NetworkMonitor")
    ssc = StreamingContext(sc, 5)
        
    lines = ssc.socketTextStream(sys.argv[1], int(sys.argv[2]))




    recordsPing = lines.filter(lambda line: "icmp_seq=" in line)

    def parseIPtoRTT(s):
        idxStart = s.find("from ") + 5
        idxEnd = idxStart
        while s[idxEnd] != ':':
            idxEnd += 1
        IP = s[idxStart:idxEnd]
        
        idxStart = s.find("time=") + 5
        idxEnd = idxStart
        while s[idxEnd] != ' ':
            idxEnd += 1
        RTT = s[idxStart:idxEnd]
            
        return (IP, float(RTT))

    IP_RTT_Record = recordsPing.map(parseIPtoRTT).map(lambda x: ("PING_RTT", x))
    #IP_RTT_Record.saveAsTextFiles("/Users/lilinzhe/Desktop/netowrk_monitor/record")
    IP_RTT_Record.pprint()

    IP_RTT_Record_Windowed = recordsPing.window(60, 5) \
                                        .map(parseIPtoRTT)
    IP_RTT_Average = IP_RTT_Record_Windowed.mapValues(lambda x: (x, 1)) \
                                           .reduceByKey(lambda x, y: (x[0] + y[0], x[1] + y[1])) \
                                           .mapValues(lambda x: x[0] / x[1])

    IP_RTT_Deviation = IP_RTT_Record_Windowed.join(IP_RTT_Average) \
                                             .mapValues(lambda x: ((x[0] - x[1])**2, 1))\
                                             .reduceByKey(lambda x, y: (x[0] + y[0], x[1] + y[1])) \
                                             .mapValues(lambda x: (x[0] / x[1])**(0.5)) \
                                             .map(lambda x: ("PING_RTT_Deviation", x))
    #IP_RTT_Deviation.saveAsTextFiles("/Users/lilinzhe/Desktop/netowrk_monitor/record")
    IP_RTT_Deviation.pprint()






            
    def parseIPtoTTL(s):
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
    
        return (IP, float(TTL))
    
    IP_TTL_Record = recordsPing.map(parseIPtoTTL).map(lambda x: ("PING_TTL", x))
    #IP_TTL_Record.saveAsTextFiles("/Users/lilinzhe/Desktop/netowrk_monitor/record");
    IP_TTL_Record.pprint()

    IP_TTL_Record_Windowed = recordsPing.window(60, 5) \
                                        .map(parseIPtoTTL)
    IP_TTL_Average = IP_TTL_Record_Windowed.mapValues(lambda x: (x, 1)) \
                                           .reduceByKey(lambda x, y: (x[0] + y[0], x[1] + y[1])) \
                                           .mapValues(lambda x: x[0] / x[1])
    IP_TTL_Deviation = IP_TTL_Record_Windowed.join(IP_TTL_Average) \
                                             .mapValues(lambda x: ((x[0] - x[1])**2, 1)) \
                                             .reduceByKey(lambda x, y: (x[0] + y[0], x[1] + y[1])) \
                                             .mapValues(lambda x: (x[0] / x[1])**(0.5)) \
                                             .map(lambda x: ("PING_TTL_Deviation", x))
    #IP_TTL_Deviation.saveAsTextFiles("/Users/lilinzhe/Desktop/netowrk_monitor/record");
    IP_TTL_Deviation.pprint();




    def parseIPSent(s):
        idxStart = s.find("(") + 1
        idxEnd = s.find(")")
        return (s[idxStart:idxEnd], 1)

    IP_Sent = lines.filter(lambda line: "PING" in line) \
                        .window(60, 5) \
                        .map(parseIPSent) \
                        .reduceByKey(lambda x, y: x + y)
    #IP_Sent.map(lambda x : ("sent", x)).pprint()


    def parseIPRecieved(s):
        idxStart = s.find("from ") + 5
        idxEnd = s.find(":")

        return (s[idxStart:idxEnd], 1)

    IP_Recieved = recordsPing.window(60, 5) \
                             .map(parseIPRecieved) \
                             .reduceByKey(lambda x, y: x + y)
    #IP_Recieved.map(lambda x : ("recieved", x)).pprint()

    IP_LossRate = IP_Sent.join(IP_Recieved) \
                         .mapValues(lambda x: (x[0] - x[1]) * 100 / x[0]) \
                         .map(lambda x: ("PING_LOSSRATE", x))
    #IP_LossRate.saveAsTextFiles("/Users/lilinzhe/Desktop/netowrk_monitor/record");
    IP_LossRate.pprint()





    
    recordsTraceRoute = lines.filter(lambda line: "  " in line)
    
    def parseTraceRouteInformation(s):
        idxStart = s.find(" (") + 2
        idxEnd = idxStart
        while s[idxEnd] != ')':
            idxEnd += 1
        IP = s[idxStart:idxEnd]

        idxStart = s.find(")  ") + 3

        return (IP, float(s[idxStart:idxStart+5]))

    TraceRoute_record = recordsTraceRoute.map(parseTraceRouteInformation) \
                                         .map(lambda x: ("TRACEROUTE_RTT", x))
    #TraceRoute_record.saveAsTextFiles("/Users/lilinzhe/Desktop/netowrk_monitor/record");
    TraceRoute_record.pprint()




    ssc.start()
    ssc.awaitTermination()
