OUTNAME=./librs232.a
SONAME =librs232.a
INCPATH=-I./ 
DEFINE=-D_DEBUG -D_LINUX -D_REENTRANT -D__USE_LARGEFILE64 
COMPILE=gcc -g -Wall $(INCPATH) $(DEFINE) -fPIC -c -o
LINK=ar cr $(OUTNAME)

LIBOBJS=librs232.o

all : $(OUTNAME)
$(OUTNAME) : $(LIBOBJS)
	$(LINK)	$(LIBOBJS)
	ranlib $(OUTNAME)

$(OBJDIR)/librs232.o : librs232.c 
	$(COMPILE) librs232.o librs232.c

clean :
	rm $(LIBOBJS) $(OUTNAME)


