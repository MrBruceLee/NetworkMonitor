IP_RTT_LargerCnt = IP_RTT_Record_Windowed.join(IP_RTT_Average) \
.mapValues(lambda x: (x[0] - x[1])) \
.filter(lambda x: x[1] > 5) \
.mapValues(lambda x: 1) \
.reduceByKey(lambda x, y: x + y)

IP_RTT_LargerCnt.pprint()