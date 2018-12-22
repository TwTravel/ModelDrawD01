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
for i in range(0, ll):
   str  =  line[i].split(',')
   print (str,"II", str[0])
   for j in range(0,7):
     #print ("##########",j)
     #print (str[j])
     #print (j)
     KK[i,j] = float(str[j])
     #print (KK[i,j], " ")
   #print("\n")

np.save("KK.npy", KK)
   #yy = float(str[3])
   #print(xx,yy)