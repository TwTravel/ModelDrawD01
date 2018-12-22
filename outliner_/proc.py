import numpy as np
fh = open('aa.txt')
#for line in fh.readlines():
line = fh.readlines()
str  =  line[0].split(',');
xx = float(str[0])
yy = float(str[1])
ll = len(line)
print( len(line) )
KK = np.zeros((int(ll),7))
print(xx,yy)
   #yy = float(str[3])
   #print(xx,yy)