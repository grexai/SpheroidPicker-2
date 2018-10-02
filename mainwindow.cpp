#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imaging.h"
#include <thread>
#include <QStyleFactory>
//#include "serialcom.h"
//#include "ArduinoPressureController.h"

imaging imaging;
bool cam = false;
cv::VideoCapture cap(0);

void setdarkstyle(){
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


void MainWindow::update_window()
{

    cv::Mat cframe;
    cap.read( cframe);
    cvtColor(cframe,cframe,CV_BGR2RGB,0);
    imaging.setframe(cframe);

    auto scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    scene->addPixmap(QPixmap::fromImage( QImage((const unsigned char*) (imaging.getframe().data),imaging.getframe().cols, imaging.getframe().rows, QImage::Format_RGB888)));

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
    setdarkstyle();

}
void porttest(){


}
void MainWindow::on_pushButton_13_clicked()
{

    ui->label_3->setText("connecting0");
     qsp.setPortName("COM10");
    ui->label_3->setText("connecting1");
      qsp.setBaudRate(QSerialPort::Baud115200);
      qsp.setDataBits(QSerialPort::Data8);
      qsp.setParity(QSerialPort::NoParity);
      qsp.setStopBits(QSerialPort::OneStop);
      qsp.setFlowControl(QSerialPort::NoFlowControl);
      ui->label_3->setText("connecting");
     if (qsp.open(QIODevice::ReadWrite))
     {
         ui->label_3->setText("connected");
     }
     else{
         ui->label_3->setText("Fila");
     }
     (qsp.flush());
     QByteArray s;

     QString command = "9,-100,10;";
     QByteArray s1;
     s1 = qsp.readAll();
     qsp.waitForReadyRead(1000);

     QByteArray x =command.toLocal8Bit();
     qsp.write(x);


}

void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::exit(0);
}

void MainWindow::on_pushButton_11_clicked()
{
    ui->label_3->setText("connecting0");
     qsp.setPortName("COM13");
    ui->label_3->setText("connecting1");
      qsp.setBaudRate(QSerialPort::Baud115200);
      qsp.setDataBits(QSerialPort::Data8);
      qsp.setParity(QSerialPort::NoParity);
      qsp.setStopBits(QSerialPort::OneStop);
      qsp.setFlowControl(QSerialPort::NoFlowControl);
       ui->label_3->setText("connecting");
     if (qsp.open(QIODevice::ReadWrite))
     {
         ui->label_3->setText("connected");
     }
     else{
         ui->label_3->setText("Fila");
     }
     (qsp.flush());
     QByteArray s;
     s = qsp.readAll();
     qsp.waitForReadyRead(1000);
           ui->label_3->setText(s);
     QString command = "G28\r\n";

     QByteArray x =command.toLocal8Bit();
     qsp.write(x);
}
