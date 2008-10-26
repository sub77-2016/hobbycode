# Uncomment the appropriate line to match your platform:

ROOT = /usr
CC = g++
CFLAGS = -O2 -Wall `wx-config --cxxflags` -DLINUX -I../XFace/include 
LIBS = -Llibquat `wx-config --libs --gl-libs` -L../XFace -lXFace -lxerces-c -lm $(GL_LIBS) 
GL_LIBS = -lGL -lGLU -lglut 

APP_NAME = wxFaceClientSimple
OBJS = wxFaceClientSimple.o FaceClientSimpleDlg.o 
CPPDEPS = -MT$@ -MF`echo $@ | sed -e 's,\.o$$,.d,'` -MD

$(APP_NAME): $(OBJS) 
	$(CC) $(CFLAGS) -o $(APP_NAME) $(OBJS) $(LIBS) 

.cpp.o:
	$(CC) $(CFLAGS) $(CPPDEPS) -c $< 


clean:
	rm -rf *.o
	rm -rf *~
	rm -rf *.d
	rm -f $(APP_NAME)

.PHONY: clean 

# Dependencies tracking:
-include ./*.d

