#include "mainwindow.h"
#include <QApplication>
#include <QStyleOptionFrameV2>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    setstyle();


    MainWindow w;
    w.show();

    return a.exec();
}
