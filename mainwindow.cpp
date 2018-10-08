#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imaging.h"
#include <thread>
#include <QStyleFactory>
#include <pipettercontroller.h>
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
    //darkPalette.setColor(QPalette::)
    darkPalette.setColor(QPalette::Button, QColor(0,0,0));

    darkPalette.setBrush(QPalette::Button,QColor(40,40,40));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);

    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    qApp->setPalette(darkPalette);

    qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");


}

QString port = "COM10";

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
    //sp->openport("asd");
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


void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::exit(0);
}

void MainWindow::on_Home_pip_clicked()
{

     QByteArray s;
     s =  qsp_pip.readAll();
      qsp_pip.waitForReadyRead(1000);
           ui->label_3->setText(s);
     QString command = "G28\r\n";

     QByteArray x =command.toLocal8Bit();
      qsp_pip.write(x);
}

// QSerialPort qsp_pc;
// serialcom sp(qsp_pc);

void MainWindow::on_Con_pc_clicked()
{
    acp = new arduinopressurecontroller(qsp_pc,port);
    if (acp->isconnected)
    {
        ui->pc_stat->setText(con_str);
    }
    else{
        ui->pc_stat->setText(fail_str);

    }


}

void MainWindow::on_Con_pip_clicked()
{
   // serialcom sp(qsp_pip);
    bool ispcopened = false;
            //sp->openport("COM13");
    if (ispcopened)
    {
        ui->pip_stat->setText(con_str);
        qsp_pip.flush();
    }
    else{
        ui->pip_stat->setText(fail_str);
    }

 //   acp->breakIn(10,30);
//  acp->acp_sc->openport("COM10");
//  float p = acp->getPipettePressure();
}

void MainWindow::on_SetPressure_clicked()
{
    acp->breakIn(10,30);
   // acp= new arduinopressurecontroller(sppc);
    //arduinopressurecontroller aacp(sppc);

    /*
    QString cmd = "9,-100,10;";
    QString ans;
    sp->send(cmd);
    */

 //   arduinopressurecontroller asp(qsp_pipette);

   // ans = sp->recive();


 //   ui->label_3->setText(ans);
/*
    QByteArray s;

    QString command = "9,-100,10;";

    QByteArray s1;

    s1 = qsp_pc.readAll();

    qsp_pc.waitForReadyRead(1000);

    QByteArray x =command.toLocal8Bit();

    qsp_pc.write(x);
*/
}
