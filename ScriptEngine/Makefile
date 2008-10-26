# Created by Waipot Ngamsaad (waipot.ngamsaad@ymail.com)

# Compiler configuration
CXX = g++
CXXFLAGS = -Wall `wx-config --cxxflags`  -Iart 
LIBS = `wx-config --libs` -lm 
LUALIBS = -llua -llualib 

# Application variables
APP = wxLuaEditor
CPPDEPS = -MT$@ -MF`echo $@ | sed -e 's,\.o$$,.d,'` -MD
OBJS = main.o LuaScriptEditor.o LuaScriptEngine.o 

$(APP): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS) $(LUALIBS) 

.cpp.o:
	$(CXX) $(CXXFLAGS) $(CPPDEPS) -c $<

wrap: LuaScriptEngine.i
	swig -c++ -lua $<

clean:
	rm -rf *.o
	rm -rf *~
	rm -rf *.d
	rm -f $(APP)

.PHONY: clean wrap

# Dependencies tracking:
-include ./*.d

