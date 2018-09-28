#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imaging.h"
#include <thread>
#include <QStyleFactory>

imaging imaging;
bool cam = false;
VideoCapture cap(0);

void setstyle(){
    qApp->setStyle(QStyleFactory::create("Fusion"));
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25,25,25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);

    darkPalette.setColor(QPalette::Button, QColor(0,0,0));

    darkPalette.setBrush(QPalette::Button,QColor(40,40,40));
    darkPalette.setColor(QPalette::ButtonText, Qt::black);

    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    qApp->setPalette(darkPalette);

    qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");


}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
     timer = new QTimer(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::displayImage(){

}

QImage qt_image;

void MainWindow::update_window()
{

    Mat cframe;
    cap.read( cframe);
    cvtColor(cframe,cframe,CV_BGR2RGB,0);
    imaging.setframe(cframe);

    qt_image = QImage((const unsigned char*) (imaging.getframe().data),imaging.getframe().cols, imaging.getframe().rows, QImage::Format_RGB888);

    auto scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    scene->addPixmap(QPixmap::fromImage( qt_image));

}




void MainWindow::on_Campushbtn_clicked()
{

    if (!cap.open(0)){
        cap.release();
        disconnect(timer, SIGNAL(timeout()), this, SLOT(update_window()));
       //  qt_image = QImage((const unsigned char*) (imaging.getframe().data),imaging.getframe().cols, imaging.getframe().rows, QImage::Format_RGB888);
    }else{
        connect(timer, SIGNAL(timeout()), this, SLOT(update_window()));
        timer->start(20);
}

}

void MainWindow::on_actionDark_Mode_triggered()
{
    setstyle();

}
