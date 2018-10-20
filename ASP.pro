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
SOURCES += \
        main.cpp \
        mainwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    stagecontroller.cpp \
    serialcom.cpp \
    arduinopressurecontroller.cpp \
    imagetools.cpp \
    PipetteController.cpp

HEADERS += \
        mainwindow.h \
    mainwindow.h \
    serialcom.h \
    arduinopressurecontroller.h \
    PipetterController.h \
    imagetools.h \
    types.h

FORMS += \
        mainwindow.ui

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

INCLUDEPATH += $$PWD/'../../../Libraries/Leica sdk/C++/include'
DEPENDPATH += $$PWD/'../../../Libraries/Leica sdk/C++/lib64'

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Libraries/OpenCV/opencv/build/x64/vc15/lib/ -lopencv_world341
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Libraries/OpenCV/opencv/build/x64/vc15/lib/ -lopencv_world341d
else:unix: LIBS += -L$$PWD/../../../Libraries/OpenCV/opencv/build/x64/vc15/lib/ -lopencv_world341

INCLUDEPATH += $$PWD/'../../../Libraries/OpenCV/opencv/build/include'
DEPENDPATH += $$PWD/'../../../Libraries/OpenCV/opencv/build/x64/vc15/lib'


win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/OpenCV/opencv/build/x64/vc15/lib/libopencv_world341.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/OpenCV/opencv/build/x64/vc15/lib/libopencv_world341d.a

else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/OpenCV/opencv/build/x64/vc15/lib/opencv_world341.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../Libraries/OpenCV/opencv/build/x64/vc15/lib/opencv_world341d.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../Libraries/OpenCV/opencv/build/x64/vc15/lib/libopencv_world341.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Libraries/OpenCV/opencv/build/x64/vc15/lib/ -lopencv_world341
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Libraries/OpenCV/opencv/build/x64/vc15/lib/ -lopencv_world341d
else:unix: LIBS += -L$$PWD/../../../Libraries/OpenCV/opencv/build/x64/vc15/lib/ -lopencv_world341


