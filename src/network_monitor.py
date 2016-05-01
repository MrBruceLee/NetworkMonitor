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
    ssc = StreamingContext(sc, 7)
        
    lines = ssc.socketTextStream(sys.argv[1], int(sys.argv[2]))

    records = lines.filter(lambda line: "icmp_seq=" in line)

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
            
    IP_RTT_Record = records.map(parseIPtoRTT)
    IP_RTT_Average = IP_RTT_Record.mapValues(lambda x: (x, 1)) \
                                  .reduceByKey(lambda x, y: (x[0] + y[0], x[1] + y[1])) \
                                  .mapValues(lambda x: x[0] / x[1])


    IP_RTT_Deviation = IP_RTT_Record.join(IP_RTT_Average) \
                                    .mapValues(lambda x: ((x[0] - x[1])**2, 1))\
                                    .reduceByKey(lambda x, y: (x[0] + y[0], x[1] + y[1])) \
                                    .mapValues(lambda x: (x[0] / x[1])**(0.5))

    IP_RTT_Deviation.pprint()

    IP_RTT_LargerCnt = IP_RTT_Record.join(IP_RTT_Average) \
                                    .mapValues(lambda x: (x[0] - x[1])) \
                                    .filter(lambda x: x[1] > 5) \
                                    .mapValues(lambda x: 1) \
                                    .reduceByKey(lambda x, y: x + y)
                                               
    IP_RTT_LargerCnt.pprint()

    #IP_RTT_Record.saveAsTextFiles("/Users/lilinzhe/Desktop/netowrk_monitor/record/Ping");



            
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
    
    IP_TTL_Record = records.map(parseIPtoTTL)
    IP_TTL_Average = IP_TTL_Record.mapValues(lambda x: (x, 1)) \
                                  .reduceByKey(lambda x, y: (x[0] + y[0], x[1] + y[1])) \
                                  .mapValues(lambda x: x[0] / x[1])
    IP_TTL_Deviation = IP_TTL_Record.join(IP_TTL_Average) \
                                    .mapValues(lambda x: ((x[0] - x[1])**2, 1))\
                                    .reduceByKey(lambda x, y: (x[0] + y[0], x[1] + y[1])) \
                                    .mapValues(lambda x: (x[0] / x[1])**(0.5))

    IP_TTL_Deviation.pprint();





    def parseIPtoSeq(s):
        idxStart = s.find("from ") + 5
        idxEnd = idxStart
        while s[idxEnd] != ':':
            idxEnd += 1
        IP = s[idxStart:idxEnd]
        
        idxStart = s.find("icmp_seq=") + 9
        idxEnd = idxStart
        while s[idxEnd] != ' ':
            idxEnd += 1
        Seq = s[idxStart:idxEnd]
    
        return (IP, float(Seq))
            
    IP_Seq_Record = records.map(parseIPtoSeq)
    IP_Seq_Count = IP_Seq_Record.mapValues(lambda x: 1) \
                                .reduceByKey(lambda x, y: x + y)
    IP_Seq_Max = IP_Seq_Record.reduceByKey(lambda x, y: max(x, y))
    IP_Seq_LossRate = IP_Seq_Max.join(IP_Seq_Count) \
                                .mapValues(lambda x: int((x[0] - x[1] + 1) / x[0] * 100))
    IP_Seq_LossRate.pprint()






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

    #mappedInformation = records.map(parseInformation)
    #mappedInformation.saveAsTextFiles("/Users/lilinzhe/Desktop/netowrk_monitor/record/Ping");
    #mappedInformation.pprint()





    
    recordsTraceRoute = lines.filter(lambda line: "  " in line)
    recordsTraceRoute.pprint()
    
    def parseTraceRouteInformation(s):
        idxStart = s.find(" (") + 2
        idxEnd = idxStart
        while s[idxEnd] != ')':
            idxEnd += 1
        IP = s[idxStart:idxEnd]

        idxStart = s.find(")  ") + 3
        #idxEnd = idxStart
        #while idxEnd != ' ':
        #    idxEnd += 1
        #RTT = s[idxStart:idxEnd]
        
        #return (IP, float(RTT))
        return (IP, float(s[idxStart:idxStart+5]))

    TraceRoute_record = recordsTraceRoute.map(parseTraceRouteInformation)
    TraceRoute_record.pprint()




    ssc.start()
    ssc.awaitTermination()
