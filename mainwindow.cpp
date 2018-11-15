#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyleFactory>

//#include "serialcom.h"
//#include "ArduinoPressureController.h"


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
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
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
    imtools= new imagetools;
    timer = new QTimer(this);
    disp_pressure= new QTimer(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_currentpressure(){
//  float cp = acp->getPipettePressure();
    float cp = (QRandomGenerator::global()->generate());
    QString cps = QString::number(cp);
    ui->lcdNumber->display(cps);
}

void MainWindow::show_currentpressure(){
    update_currentpressure();
    connect(disp_pressure, SIGNAL(timeout()), this, SLOT(update_currentpressure()));
    disp_pressure->start(250);
}

void MainWindow::update_window()
{
    cv::Mat cframe;
    imtools->setvideodevice(0);
    imtools->getCameraframe();
    auto scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    scene->addPixmap(QPixmap::fromImage( QImage((const unsigned char*) (imtools->getframe().data),imtools->getframe().cols, imtools->getframe().rows, QImage::Format_RGB888)));
}


void MainWindow::on_Campushbtn_clicked()
{
    if ((imtools->iscameraopen)){
        imtools->rmvideodevice();
        disconnect(timer, SIGNAL(timeout()), this,nullptr);
    }else{
        imtools->setvideodevice(0);
        connect(timer, SIGNAL(timeout()), this, SLOT(update_window()));
        timer->start(20);
    }
}

void MainWindow::on_actionDark_Mode_triggered()
{
    setdarkstyle();
}


void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::exit(0);
}

void MainWindow::on_Home_pip_clicked()
{
    apipc->goHome(false,false,false);
}

void MainWindow::on_Con_pc_clicked()
{
    QString port = "COM10";
    acp = new arduinopressurecontroller(qsp_pc,port);
    if (acp->isconnected)
    {
        ui->pc_stat->setText(con_str);
        show_currentpressure();
    }
    else{
        ui->pc_stat->setText(fail_str);
    }
}

void MainWindow::on_Con_pip_clicked()
{
   // serialcom sp(qsp_pip);
    QString port2 = "COM13"; //13
    apipc = new pipetteController(qsp_pip,port2);
    if (apipc->isconnected)
    {
        ui->pip_stat->setText(con_str);
      //  qsp_pip.flush();
    }
    else{
        ui->pip_stat->setText(fail_str);
    }

}

void MainWindow::on_SetPressure_clicked()
{
    acp->requestPressure(ui->pressure_value->value());
}

void MainWindow::on_atm_button_clicked()
{
    float pr=acp->getPipettePressure();
    ui->label_3->setText(QString::number(pr));
}

void MainWindow::on_pc_Breakin_button_clicked()
{
    acp->breakIn(ui->breakin_value->value(),ui->breakin_time->value());
}

void MainWindow::on_get_coors_pushButton_clicked()
{
    QByteArray  ans;

    Float3coor randx = apipc->getcurrentpos(ans);

    ui->xc_label->setText("X: " + QString::number(randx.x,'f',2));
    ui->yc_label->setText("Y: " + QString::number(randx.y,'f',2));
    ui->zc_label->setText("Z: " + QString::number(randx.z,'f',2));
}

void MainWindow::on_s_xp_button_clicked()
{

}

void MainWindow::on_s_xm_button_clicked()
{

}

void MainWindow::on_s_yp_button_clicked()
{

}

void MainWindow::on_s_ym_button_clicked()
{

}

void MainWindow::on_p_xp_button_clicked()
{
    apipc->setrelativepositioning();
    apipc->moveToXAsync(ui->pip_step_spinbox->value());
}

void MainWindow::on_p_xm_button_clicked()
{
    apipc->setrelativepositioning();
    apipc->moveToXAsync(-(ui->pip_step_spinbox->value()));
}

void MainWindow::on_p_yp_button_clicked()
{
    apipc->setrelativepositioning();
    apipc->moveToYAsync(ui->pip_step_spinbox->value());
}

void MainWindow::on_p_ym_button_clicked()
{
    apipc->setrelativepositioning();
    apipc->moveToYAsync(-(ui->pip_step_spinbox->value()));
}

void MainWindow::on_p_zp_button_clicked()
{
    apipc->setrelativepositioning();
    apipc->moveToZAsync(ui->pip_step_spinbox->value());
}

void MainWindow::on_p_zm_btton_clicked()
{
    apipc->setrelativepositioning();
    apipc->moveToZAsync(-(ui->pip_step_spinbox->value()));
}

void MainWindow::on_lcdNumber_overflow()
{

}

void MainWindow::on_Con_xystage_button_clicked()
{
    //pRootUnit = theHardwareModel()->getUnit("");

    //pStageUnit = findUnit(pRootUnit, ahm::MICROSCOPE_STAGE);
    //stage= new Stage(pRootUnit);
}



void MainWindow::on_actionOpen_console_triggered()
{
    QTextStream(stdout) << "yo program  has succesfully crashed." << endl;
   // std::cout<< "asdsaedsadsadsa"<< std::endl;
}
