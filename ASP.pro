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
#QT += core5compat
QT += serialport
QT += xml
QT += core
QMAKE_MOC_OPTIONS += -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED
CONFIG(staticlib): DEFINES += QUAZIP_STATIC
CONFIG += console
CONFIG(release, debug|release) {
}


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    mosaicworker.cpp \
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
    hardwareselector.cpp \
    plateselector.cpp \
    spheroid_selector.cpp

HEADERS += \
    mainwindow.h \
    mosaicworker.h \
    serialcom.h \
    controller.h \
    arduinogcode.h \
    arduinopressurecontroller.h \
    imagetools.h \
    types.h \
    stagecontroller.h \
    calibratedialog.h \
    comps.h \
    propertyreader.h \
    deeplearning.h \
    cameracv.h \
    marcros.h \
    auto_methods.h \
    counter.h \
    hardwareselector.h \
    plateselector.h \
    spheroid_selector.h

FORMS += \
        mainwindow.ui \
    calibratedialog.ui \
    hardwareselector.ui \
    plateselector.ui \
    spheroid_selector.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/'../../libraries/Leica sdk/C++/lib64/' -lahmcore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/'../../libraries/Leica sdk/C++/lib64/' -lahmcore
else:unix: LIBS += -L$$PWD/'../../libraries/Leica sdk/C++/lib64/' -lahmcore

INCLUDEPATH += $$PWD/'../../libraries/Leica sdk/C++/include'
DEPENDPATH += $$PWD/'../../libraries/Leica sdk/C++/lib64'

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/'../../libraries/Leica sdk/C++/lib64/libahmcore'
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/'../../libraries/Leica sdk/C++/lib64/libahmcore'
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/'../../libraries/Leica sdk/C++/lib64/ahmcore.lib'
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/'../../libraries/Leica sdk/C++/lib64/ahmcore.lib'
else:unix: PRE_TARGETDEPS += $$PWD/'../../../libraries/Leica sdk/C++/lib64/libahmcore.a'

unix|win32: LIBS += -L$$PWD/'../../libraries/Leica sdk/C++/lib64/' -lahmcorelocator

# OPENCV

INCLUDEPATH += $$PWD/'../../libraries\opencv-4.5.5\opencv-4.5.5\build\install\include'

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../libraries/opencv-4.5.5/opencv-4.5.5/build/lib/Release/ -lopencv_core455
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libraries/opencv-4.5.5/opencv-4.5.5/build/lib/Release/ -lopencv_core455

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../libraries/opencv-4.5.5/opencv-4.5.5/build/lib/Release/ -lopencv_imgproc455
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../libraries/opencv-4.5.5/opencv-4.5.5/build/lib/Release/ -lopencv_imgproc455

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../libraries/opencv-4.5.5/opencv-4.5.5/build/lib/Release/ -lopencv_highgui455
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libraries/opencv-4.5.5/opencv-4.5.5/build/lib/Release/ -lopencv_highgui455

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../libraries/opencv-4.5.5/opencv-4.5.5/build/lib/Release/ -lopencv_imgcodecs455
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libraries/opencv-4.5.5/opencv-4.5.5/build/lib/Release/ -lopencv_imgcodecs455

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../libraries/opencv-4.5.5/opencv-4.5.5/build/lib/Release/ -lopencv_videoio455
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libraries/opencv-4.5.5/opencv-4.5.5/build/lib/Release/ -lopencv_videoio455

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../libraries/opencv-4.5.5/opencv-4.5.5/build/lib/Release/ -lopencv_ml455
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libraries/opencv-4.5.5/opencv-4.5.5/build/lib/Release/ -lopencv_ml455

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../libraries/opencv-4.5.5/opencv-4.5.5/build/lib/Release/ -lopencv_dnn455
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libraries/opencv-4.5.5/opencv-4.5.5/build/lib/Release/ -lopencv_dnn455



DISTFILES +=

CONFIG(release,tf-cpu|release){
#win32: LIBS += -L$$PWD/../../../libraries/tf-cpu113/lib/ -tensorflow

INCLUDEPATH += $$PWD/../../libraries/libtensorflow-gpu-windows-x86_64-2.7.0\include
DEPENDPATH += $$PWD/../../libraries/libtensorflow-gpu-windows-x86_64-2.7.0\include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../libraries/libtensorflow-gpu-windows-x86_64-2.7.0\lib\tensorflow.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../../../libraries/libtensorflow-gpu-windows-x86_64-2.7.0\lib/tensorflow.a

win32:CONFIG(release, debug|release): LIBS += $$PWD/../../libraries/libtensorflow-gpu-windows-x86_64-2.7.0\lib\tensorflow.lib
else:win32:CONFIG(debug, debug|release): $$PWD/../../libraries/libtensorflow-gpu-windows-x86_64-2.7.0\lib/tensorflow.a
}
#else
#{
#win32: LIBS += -L$$PWD/../../libraries/tf-gpu131/lib/ -llib64tf131

#INCLUDEPATH += $$PWD/../../libraries/tf-gpu131/include
#DEPENDPATH += $$PWD/../../libraries/tf-gpu131/include

#win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../libraries/tf-gpu131/lib/lib64tf131.lib
#else:win32-g++: PRE_TARGETDEPS += $$PWD/../../libraries/tf-gpu131/lib/liblib64tf131.a

#win32:CONFIG(release, debug|release): LIBS += $$PWD/../../libraries/tf-gpu131/lib/lib64tf131.lib
#else:win32:CONFIG(debug, debug|release): $$PWD/../../libraries/tf-gpu131/lib/liblib64tf131.a
#}
