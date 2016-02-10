TEMPLATE = app
TARGET = ENF_AudioRecorder

QT += multimedia

win32:INCLUDEPATH += $$PWD
RC_FILE = myapp.rc

HEADERS = \
    audiorecorder.h \
    qaudiolevel.h \
    EAFF.h

SOURCES = \
    main.cpp \
    audiorecorder.cpp \
    qaudiolevel.cpp \
    EAFF.cpp

FORMS += audiorecorder.ui

target.path = $$[QT_INSTALL_EXAMPLES]/multimedia/audiorecorder
INSTALLS += target

QT+=widgets

