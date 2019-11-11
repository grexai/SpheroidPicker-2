#-------------------------------------------------
#
# Project created by QtCreator 2018-09-24T13:22:57
#
#-------------------------------------------------
# gitinor . user
QT       += core gui widgets

TARGET = ASP
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11
QT += serialport
#CONFIG += console
CONFIG(release, debug|release) {
  CONFIG += console
}

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    stagecontroller.cpp \
    serialcom.cpp \
    arduinopressurecontroller.cpp \
    imagetools.cpp \
    calibratedialog.cpp \
    comps.cpp \
    controller.cpp \
    propertyreader.cpp \
    arduinogcode.cpp \
    deeplearning.cpp \
    cameracv.cpp \
    auto_methods.cpp \
    hardwareselector.cpp

HEADERS += \
    mainwindow.h \
    serialcom.h \
    arduinopressurecontroller.h \
    imagetools.h \
    types.h \
    stagecontroller.h \
    calibratedialog.h \
    comps.h \
    controller.h \
    propertyreader.h \
    arduinogcode.h \
    deeplearning.h \
    cameracv.h \
    marcros.h \
    auto_methods.h \
    counter.h \
    hardwareselector.h

FORMS += \
        mainwindow.ui \
    calibratedialog.ui \
    hardwareselector.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/'../../../Libraries/Leica sdk/C++/lib64/' -lahmcore
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/'../../../Libraries/Leica sdk/C++/lib64/' -lahmcore
#else:unix: LIBS += -L$$PWD/'../../../Libraries/Leica sdk/C++/lib64/' -lahmcore

INCLUDEPATH += $$PWD/'../../../Libraries/Leica sdk/C++/include'
DEPENDPATH += $$PWD/'../../../Libraries/Leica sdk/C++/lib64'

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/'../../../Libraries/Leica sdk/C++/lib64/libahmcore'
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/'../../../Libraries/Leica sdk/C++/lib64/libahmcore'
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/'../../../Libraries/Leica sdk/C++/lib64/ahmcore.lib'
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/'../../../Libraries/Leica sdk/C++/lib64/ahmcore.lib'
else:unix: PRE_TARGETDEPS += $$PWD/'../../../Libraries/Leica sdk/C++/lib64/libahmcore.a'

unix|win32: LIBS += -L$$PWD/'../../../Libraries/Leica sdk/C++/lib64/' -lahmcorelocator

# OPENCV

INCLUDEPATH += $$PWD/'../../../Libraries\opencv-3.4.5\build\install\include'

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/ -lopencv_core345
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/ -lopencv_core345

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/ -lopencv_imgproc345
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/ -lopencv_imgproc345

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/ -lopencv_highgui345
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/ -lopencv_highgui345

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/ -lopencv_imgcodecs345
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/ -lopencv_imgcodecs345

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/ -lopencv_videoio345
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/ -lopencv_videoio345

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/ -lopencv_ml345
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/ -lopencv_ml345

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/ -lopencv_dnn345
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/ -lopencv_dnn345

DISTFILES +=

win32: LIBS += -L$$PWD/../../../Libraries/tf-gpu131/lib/ -llib64tf131

INCLUDEPATH += $$PWD/../../../Libraries/tf-gpu131/include
DEPENDPATH += $$PWD/../../../Libraries/tf-gpu131/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../../Libraries/tf-gpu131/lib/lib64tf131.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../../../Libraries/tf-gpu131/lib/liblib64tf131.a

win32:CONFIG(release, debug|release): LIBS += $$PWD/../../../Libraries/tf-gpu131/lib/lib64tf131.lib
else:win32:CONFIG(debug, debug|release): $$PWD/../../../Libraries/tf-gpu131/lib/liblib64tf131.a

