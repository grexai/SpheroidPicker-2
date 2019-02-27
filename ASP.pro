#-------------------------------------------------
#
# Project created by QtCreator 2018-09-24T13:22:57
#
#-------------------------------------------------

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
CONFIG += console
SOURCES += \
        main.cpp \
        mainwindow.cpp \
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
    cameracv.cpp

HEADERS += \
        mainwindow.h \
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

FORMS += \
        mainwindow.ui \
    calibratedialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/'../../../Libraries/Leica sdk/C++/lib64/' -lahmcore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/'../../../Libraries/Leica sdk/C++/lib64/' -lahmcore
else:unix: LIBS += -L$$PWD/'../../../Libraries/Leica sdk/C++/lib64/' -lahmcore

INCLUDEPATH += $$PWD/'../../../Libraries/Leica sdk/C++/include'
DEPENDPATH += $$PWD/'../../../Libraries/Leica sdk/C++/lib64'

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/'../../../Libraries/Leica sdk/C++/lib64/libahmcore'
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/'../../../Libraries/Leica sdk/C++/lib64/libahmcored'
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/'../../../Libraries/Leica sdk/C++/lib64/ahmcore.lib'
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/'../../../Libraries/Leica sdk/C++/lib64/ahmcored.lib'
else:unix: PRE_TARGETDEPS += $$PWD/'../../../Libraries/Leica sdk/C++/lib64/libahmcore.a'

unix|win32: LIBS += -L$$PWD/'../../../Libraries/Leica sdk/C++/lib64/' -lahmcorelocator

# OPENCV

INCLUDEPATH += $$PWD/'../../../Libraries\opencv-3.4.5\build\install\include'

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/ -lopencv_core345
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/debug/ -lopencv_core345

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/ -lopencv_imgproc345
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/debug/ -lopencv_imgproc345

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/ -lopencv_highgui345
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/debug/ -lopencv_highgui345

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/ -lopencv_imgcodecs345
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/debug/ -lopencv_imgcodecs345

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/ -lopencv_videoio345
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/debug/ -lopencv_videoio345

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/ -lopencv_ml345
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/debug/ -lopencv_ml345

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/ -lopencv_dnn345
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Libraries/opencv-3.4.5/build/lib/debug/ -lopencv_dnn345

#INCLUDEPATH += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/Release
#DEPENDPATH += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/Release

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/opencv_core345.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/debug/opencv_core345.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/opencv_core345.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/debug/opencv_core345.lib

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/libopencv_imgproc345.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/debug/libopencv_imgproc345.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/opencv_imgproc345.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/debug/opencv_imgproc345.lib


#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/opencv_highgui345.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/debug/opencv_highgui345.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/opencv_highgui345.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/debug/opencv_highgui345.lib

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/opencv_imgcodecs345.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/debug/opencv_imgcodecs345.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/opencv_imgcodecs345.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/debug/opencv_imgcodecs345.lib

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/opencv_ml345.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/debug/opencv_ml345.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/opencv_ml345.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/debug/opencv_ml345.lib

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/opencv_ml345.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/debug/opencv_ml345.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/release/opencv_video345.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/opencv-3.4.5/build/lib/debug/opencv_video345.lib
