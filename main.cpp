//#include "mainwindow.h"
#include "hardwareselector.h"
#include <QApplication>
#include <QStyleOptionFrameV2>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    setstyle();

    hardwareselector w;
    QPixmap pixmap("../BIOMAGwhite-01.png");
    QSplashScreen splash(pixmap);

    splash.show();
    QTimer::singleShot(1100,&splash,SLOT(close()));
    QTimer::singleShot(1130,&w,SLOT(show()));
  //  w.show();


    return a.exec();

}
