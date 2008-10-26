CC = g++
CFLAGS = `fltk-config --cflags` 

genparts: genparts.o ../Math3D.o ../bvh.o ../glm.o
	$(CC) -O3 -o genparts *.o ../Math3D.o ../bvh.o ../glm.o -lopengl32

.c.o:
	$(CC) $(CFLAGS) -c $<

.cpp.o:
	$(CC) $(CFLAGS) -c $<

../Math3D.o:
	cd ..; $(CC) $(CFLAGS) -c Math3D.cpp

../bvh.o:
	cd ..; $(CC) $(CFLAGS) -c bvh.c

../glm.o:
	cd ..; $(CC) $(CFLAGS) -c glm.c

clean:
	rm -rf *.o
	rm -rf *~
	rm -ff genparts genparts.exe
