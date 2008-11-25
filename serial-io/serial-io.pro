######################################################################
# Automatically generated by qmake (2.01a) Fri Jan 26 16:40:52 2007
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

LIBS += -lserial

QMAKE_CXXFLAGS +=  -O0
#  -g

QT	+= xml

# Input
HEADERS += DeviceDialog.h mainwindow.h SerialReceiveThread.h EditBufferDialog.h EditEventDialog.h ValueBuffer.h Event.h InitialValueDialog.h OptionsDlg.h RxTxValue.h SaveBufferDlg.h Dialog.h EditDialogDialog.h Variable.h  Representation.h VariableDialog.h LongLongIntValidator.h
SOURCES += DeviceDialog.cpp main.cpp mainwindow.cpp SerialReceiveThread.cpp EditBufferDialog.cpp EditEventDialog.cpp ValueBuffer.cpp Event.cpp InitialValueDialog.cpp OptionsDlg.cpp RxTxValue.cpp SaveBufferDlg.cpp Dialog.cpp EditDialogDialog.cpp Variable.cpp Representation.cpp VariableDialog.cpp LongLongIntValidator.cpp
RESOURCES += siom.qrc
TRANSLATIONS += siom_ger.ts

target.path = /usr/bin/
INSTALLS += target