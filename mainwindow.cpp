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
    ui->graphicsView->setScene(new QGraphicsScene(this));
    ui->graphicsView->scene()->addItem(&qpxmi);
    imtools= new imagetools;
    timer = new QTimer(this);
    disp_pressure= new QTimer(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_currentpressure(){
    float cp = acp->getPipettePressure();
     QTextStream(stdout) << cp << endl;
    //float cp = (QRandomGenerator::global()->generate());
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
    imtools->getCameraframe();
    if (qframe != nullptr)
    {
        delete[] qframe;
    }
    auto qframe = new QImage((const unsigned char*) (imtools->getframe()->data),imtools->getframe()->cols, imtools->getframe()->rows, QImage::Format_RGB888);
    qpxmi.setPixmap( QPixmap::fromImage(*qframe) );
    ui->graphicsView->fitInView(&qpxmi, Qt::KeepAspectRatio);
}

void MainWindow::on_Campushbtn_clicked()
{
    if ((imtools->iscameraopen)){
        ui->Campushbtn->setText("Camera on");
        imtools->rmvideodevice();
        disconnect(timer, SIGNAL(timeout()), this,nullptr);
    }else{

        imtools->setvideodevice(cameraIndex);
        if(!imtools->getCamera()->open(cameraIndex))
              {
                  QMessageBox::critical(this,
                                        "Camera Error",
                                        "Make sure you entered a correct camera index,"
                                        "<br>or that the camera is not being accessed by another program!");
                  return;
              }

        ui->Campushbtn->setText("Camera off");
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
    QString port = "COM5";
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
    QString port2 = "COM20"; //13
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
    acp->requestPressure(0.0f);
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
    iop::int32 x0= stage->XAxis().getCurrentPosition();
    iop::int32 x = ui->s_step_spinbox->value();
    stage->XAxis().moveToAsync(iop::int32(x0+x));
}

void MainWindow::on_s_xm_button_clicked()
{

    iop::int32 x0= stage->XAxis().getCurrentPosition();
    iop::int32 x = ui->s_step_spinbox->value();
    stage->XAxis().moveToAsync(x0-x);
}

void MainWindow::on_s_yp_button_clicked()
{
    iop::int32 y0= stage->YAxis().getCurrentPosition();
    iop::int32 y = ui->s_step_spinbox->value();
    stage->YAxis().moveToAsync(y0+y);
}

void MainWindow::on_s_ym_button_clicked()
{
    iop::int32 y0= stage->YAxis().getCurrentPosition();
    iop::int32 y = ui->s_step_spinbox->value();
    stage->YAxis().moveToAsync(y0-y);
}

void MainWindow::on_p_xp_button_clicked()
{
    apipc->setrelativepositioning();
    apipc->moveToXSync(ui->pip_step_spinbox->value());
}

void MainWindow::on_p_xm_button_clicked()
{
    apipc->setrelativepositioning();
    apipc->moveToXSync(-(ui->pip_step_spinbox->value()));
}

void MainWindow::on_p_yp_button_clicked()
{
    apipc->setrelativepositioning();
    apipc->moveToYSync(ui->pip_step_spinbox->value());
}

void MainWindow::on_p_ym_button_clicked()
{
    apipc->setrelativepositioning();
    apipc->moveToYSync(-(ui->pip_step_spinbox->value()));
}

void MainWindow::on_p_zp_button_clicked()
{
    apipc->setrelativepositioning();
    apipc->moveToZSync(ui->pip_step_spinbox->value());
}


void MainWindow::on_p_zm_btton_clicked()
{
    apipc->setrelativepositioning();
    apipc->moveToZSync(-(ui->pip_step_spinbox->value()));
}

void MainWindow::on_lcdNumber_overflow()
{

}

void MainWindow::on_Con_xystage_button_clicked()
{
//    try{

        if(theHardwareModel()){

            QTextStream(stdout) << "asdsad" << endl;

            iop::string name = "";

            pRootUnit = theHardwareModel()->getUnit("");

            QTextStream(stdout) << pRootUnit << endl;

            if(pRootUnit){
                pStageUnit = findUnit(pRootUnit, ahm::MICROSCOPE_STAGE);
                stage = new Stage(pStageUnit) ;

                if(!pStageUnit){
                    QTextStream(stdout) << "yo program  has succesfully crashed. is not works" << endl;
                    return;
                }
            ui->s_stat->setText(con_str);
            }
        }
}

void MainWindow::on_actionOpen_console_triggered()
{
    QTextStream(stdout) << "yo program  has succesfully crashed. is not works" << endl;
}

void MainWindow::on_s_center_button_clicked()
{

    iop::int32 x0 = stage->XAxis().getMinPosition();
    iop::int32 x1 = stage->XAxis().getMaxPosition();

    iop::int32 y0 = stage->YAxis().getMinPosition();
    iop::int32 y1 = stage->YAxis().getMaxPosition();

    iop::int32 x = x0 + (iop::int32) ( (x1-x0)/2.0);
    iop::int32 y = y0 + (iop::int32) ( (y1-y0)/2.0);

    stage->moveToAsync(x,y, false);
}

void MainWindow::on_exptime_button_clicked()
{
    imtools->setexposuretime(ui->exptime_spin->value());
}

void MainWindow::on_width_button_clicked()
{
    imtools->setimagewidth(ui->width_spin->value());
}

void MainWindow::on_height_button_clicked()
{
    imtools->setimagewidth(ui->height_spin->value());
}
