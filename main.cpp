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
    QPixmap pixmap("E:/BIOMAGtransparentsml.png");
    pixmap.scaled(400,150,Qt::KeepAspectRatio,Qt::FastTransformation);
    QSplashScreen splash(pixmap);

    splash.show();
    QTimer::singleShot(1200,&splash,SLOT(close()));
    QTimer::singleShot(1100,&w,SLOT(show()));
  //  w.show();


    return a.exec();

}
