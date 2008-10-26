TEMPLATE	= app
LANGUAGE	= C++
CONFIG	+= warn_off release
INCLUDEPATH	+= . include /usr/include/festival /usr/include/estools
LIBS += -lFestival -lestools -lestbase -leststring -lesd -lncurses -lm
HEADERS	+= include/mymaindialog.h include/aimlparser.h include/tts.h
SOURCES	+= src/main.cpp src/mymaindialog.cpp src/aimlparser.cpp src/tts.cpp
FORMS	      = ui/aimlsetdialog.ui ui/maindialog.ui
IMAGES	+= img/programq.png
DEPENDPATH  += include src ui
UI_DIR      = tmp
MOC_DIR     = tmp
OBJECTS_DIR = tmp
RC_FILE     = resource.rc
win32{
  LIBS      += advapi32.lib
}

#Qt4 specific
QT          += qt3support xml
CONFIG	+= uic3

