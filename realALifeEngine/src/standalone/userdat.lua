a=al.Point3r(1,0,0)
print(a)
m=getmetatable(a)
table.foreach(m,print)

b=al.Point3r(0,0,1)

c=a/2
