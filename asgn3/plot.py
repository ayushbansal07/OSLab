import matplotlib.pyplot as plt

file = open('results.txt', "r") 
lines = [float(x) for x in file.readlines() ]
print lines

N = [lines[i] for i in range(0,18,6)]
FCFS = [lines[i] for i in range(1,19,6)] 
# print FCFS
SJF = [lines[i] for i in range(2,19,6)]
RR1 = [lines[i] for i in range(3,19,6)]
RR2 = [lines[i] for i in range(4,19,6)]
RR5 = [lines[i] for i in range(5,19,6)]

FCFS_plt, = plt.plot(N,FCFS,'bo-')
SJF_plt, = plt.plot(N,SJF,'ro-')
RR1_plt, = plt.plot(N,RR1,'go-')
RR2_plt, = plt.plot(N,RR2,'yo-')
RR5_plt, = plt.plot(N,RR5, 'bs-')
plt.legend([FCFS_plt,SJF_plt,RR1_plt,RR2_plt,RR5_plt],["FCFS","SJF","RR1","RR2","RR5"])
plt.xlabel('Number of Processes')
plt.ylabel('Average Turnaround Time')
# plt.axis([0, 6, 0, 20])
plt.show()
