//#include "mainwindow.h"
#include "hardwareselector.h"
#include <QApplication>
#include <QStyleOptionFrameV2>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    setstyle();

    hardwareselector w;
    w.show();

    return a.exec();

}
