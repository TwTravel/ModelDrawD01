fh = open('/data/tzwang/temp/objinfo.txt')
#for line in fh.readlines():
line = fh.readlines()
str  = ((line[0].split(':'))[1]).split(',');
xx = float(str[0])
yy = float(str[1])
print(xx,yy)
   #yy = float(str[3])
   #print(xx,yy)