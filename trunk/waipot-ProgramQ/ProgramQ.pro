TEMPLATE	= app
LANGUAGE	= C++
CONFIG	+= warn_off release
INCLUDEPATH	+= . include
HEADERS	+= include/mymaindialog.h include/aimlparser.h
SOURCES	+= src/main.cpp src/mymaindialog.cpp src/aimlparser.cpp
FORMS	      = ui/aimlsetdialog.ui ui/maindialog.ui
IMAGES	+= img/programq.png
DEPENDPATH  += include src ui
UI_DIR      = tmp
MOC_DIR     = tmp
OBJECTS_DIR = tmp
RC_FILE     = programq.rc
win32{
  LIBS      += advapi32.lib
}

#Qt4 specific
QT          += qt3support xml
CONFIG	+= uic3

