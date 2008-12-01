import random

file = open('sampleData.dat', 'w')

for i in range(1000):
    file.write(str(random.gauss(0, 5)))
    file.write(' ')
    file.write(str(random.gauss(0, 0.1)))
    file.write('\n')

file.close()
