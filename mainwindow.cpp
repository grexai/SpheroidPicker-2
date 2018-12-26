#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyleFactory>
#include <QMouseEvent>




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
    calib = new calibratedialog;
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
    //float cp = (QRandomGenerator::global()->generate());
    ui->lcdNumber->display( QString::number(cp));
}

void MainWindow::show_currentpressure(){
    update_currentpressure();
    connect(disp_pressure, SIGNAL(timeout()), this, SLOT(update_currentpressure()));
    disp_pressure->start(1000);
}


bool MainWindow::eventFilter( QObject *obj, QEvent *event ){
    if (obj == ui->graphicsView && true)
       {
         if (event->type() == QEvent::MouseButtonPress)
         {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*> (event);
            if(point_mouse!= nullptr){
                delete point_mouse;
                point_mouse  = new QPoint;
            }else{

                point_mouse  = new QPoint;
            }
            *point_mouse = ui->graphicsView->mapFrom(ui->graphicsView, mouseEvent->pos());
            if(mouseEvent->button() == Qt::RightButton)
            {
                this->ui->graphicsView->setContextMenuPolicy(Qt::CustomContextMenu);
                QTextStream(stdout) << true;
                connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
                        this, SLOT(on_graphicsView_customContextMenuRequested(const QPoint &)));
                QTextStream(stdout) <<"coors x: " << point_mouse->x() << "    y: " << point_mouse->y() << endl;
            }
            else
            return true;
         }
    }

}




void MainWindow::calib_frame_view(){
    using namespace cv;
    if (calib->Iscalibrating == 1){
        Point pmid = Point(imtools->getframe()->cols/2,100 );

        if (calib->clicks==0){
            imtools->addPointToImage(pmid);
        }
        if (calib->clicks==1){
            imtools->addPointToImage(Point(100,imtools->getframe()->rows-100));
          //  apipc->getcurrentpos();
            if(cpos1== nullptr){
               cpos1 = new std::vector<float>;
               apipc->getcurrentpos();
                *cpos1 = apipc->getcurrentpos();

                QTextStream(stdout ) << "point 1 saved:" << cpos1->at(0) <<" "<<cpos1->at(1)<< " "<< cpos1->at(2) <<endl;
            }

        }
        if (calib->clicks==2){
            imtools->addPointToImage(Point(imtools->getframe()->cols-100,imtools->getframe()->rows-100));
            if(cpos2== nullptr){
               cpos2 = new std::vector<float>;
               *cpos2 = apipc->getcurrentpos();
            QTextStream(stdout ) << "point 2 saved: " << cpos2->at(0) <<" "<<cpos2->at(1)<< " "<< cpos2->at(2) <<endl;
            }
        }
        if (calib->clicks==3){

            if(cpos3 == nullptr){
               cpos3 = new std::vector<float>;
               *cpos3 = apipc->getcurrentpos();
            QTextStream(stdout ) << "point 3 saved: x: " << cpos3->at(0) <<" "<<cpos3->at(1)<< " "<< cpos3->at(2) <<endl;
            }
        }



            //  calib->calibpos.push_back(apipc->getcurrentpos());
        //  QTextStream(stdout ) << "point 1 saved: x: " <<calib->calibpos.at(2).x<< "   y: "<<calib->calibpos.at(2).y<<"   z:" <<calib->calibpos.at(2).z<<endl;


        }

    }





void MainWindow::update_window()
{
    imtools->getCameraframe();
    calib_frame_view();
    //imshow("wtf",*imtools->getframe());
   //cv:: waitKey(0);
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
        float w =1920;
        float h = 1080;
        QTextStream(stdout)<< "setting sizes";
        imtools->setimagewidth(w);
        imtools->setimageheight(h);
        setMouseTracking(true);
        ui->graphicsView->installEventFilter(this);


        connect(timer, SIGNAL(timeout()), this, SLOT(update_window()));
        timer->start(20);



    }
}

void MainWindow::on_exptime_button_clicked()
{
    imtools->setexposuretime(ui->exptime_spin->value());
}

void MainWindow::on_width_button_clicked()
{
    disconnect(timer, SIGNAL(timeout()), this,nullptr);
    float w =  ui->width_spin->value();
    imtools->setimagewidth(w);
    connect(timer, SIGNAL(timeout()), this, SLOT(update_window()));
    timer->start(20);
}

void MainWindow::on_height_button_clicked()
{
    float h =  ui->height_spin->value();
    imtools->setimagewidth(h);
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

void MainWindow::on_p_home_x_clicked()
{
    apipc->goHome(true,false,false);
}

void MainWindow::on_p_home_y_clicked()
{
    apipc->goHome(false,true,false);
}

void MainWindow::on_p_home_z_clicked()
{
    apipc->goHome(false,false,true);
}

void MainWindow::on_Con_pc_clicked()
{
    QString port = "COM5"; //AKOS //5
    acp = new arduinopressurecontroller(qsp_pc,port);
    if (acp->isconnected)
    {
        ui->pc_stat->setText(con_str);
       // show_currentpressure();
    }
    else{
        ui->pc_stat->setText(fail_str);
    }
}

void MainWindow::on_Con_pip_clicked()
{
    QString port2 = "COM21"; //7AKOS
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

void MainWindow::on_pc_pulse_button_clicked()
{
    acp->breakIn(ui->pulse_value->value(),ui->pulse_time->value());
}

void MainWindow::on_get_coors_pushButton_clicked()
{

   // Float3coor rc;
   // Float3coor randx = apipc->getcurrentpos();
   // std::vector<float> randx = apipc->getcurrentpos();
   // ui->xc_label->setText("X: " + QString::number(randx.x,'f',2));
   // ui->yc_label->setText("Y: " + QString::number(randx.y,'f',2));
   // ui->zc_label->setText("Z: " + QString::number(randx.z,'f',2));
    float ipdata[6]=  {960.0f,100.0f,1820.0f,540.0f,980.0f,540.0f};


    float ipdata2[6] = {960.0f,100.0f,1820.0f,100.0f,980.0f,980.0f};
      cv::Mat cip = cv::Mat(2,3,CV_32F,ipdata2);

    float cpdata[9]= {933.0f,922.0f,970.0f,195.0f,255.0f,151.0f,100.0f,100.0f,85.0f};
    float randtest[9] = {1,2,3,4,5,6,7,8,9};
    std::vector<float> rV = {1.0f,2.0f,3.0f};
    std::vector<float> rV3 = {4.0f,5.0f,6.0f};
  //  cv::Mat randmat;
    cv::Mat randmat;
    randmat.push_back(*cpos1);
    randmat.push_back(*cpos2);
    randmat.push_back(*cpos3);
    std::cout << randmat << "before reshape"<<endl;
   randmat =  randmat.reshape(0,3);
   cv::transpose(randmat,randmat);
  //  randmat=randmat.reshape(3,3);
    std::cout << randmat<< "randmat"<< std::endl;
    cv::Mat cpp = cv::Mat(3,3,CV_32F,cpdata);

 // cv::Mat cip = (cv::Mat_<int>(2,3) << 960,100,1820,540,980,540);
 // cv::Mat cpp = (cv::Mat_<int>(3,3)<<933,922,970,195,255,151,100,100,85);

    TM = getTMatrix(randmat,cip,centers);
    //ic = new cv::Mat;
    imgc= new cv::Mat(1,2,CV_32F);
    *imgc = geticenter(cip);
    pc= getpcenter(randmat);
    QTextStream(stdout)<<"done" << endl;
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

            iop::int32 curSpeedX = stage->XAxis().getCurrentSpeed();
            ui->s_speed_spinbox->setValue(float(curSpeedX));
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


void MainWindow::on_s_set_speed_button_clicked()
{
    stage->XAxis().setCurrentSpeed((iop::int32)(ui->s_speed_spinbox->value()));
    stage->YAxis().setCurrentSpeed((iop::int32)(ui->s_speed_spinbox->value()));
}

void MainWindow::on_save_image_button_clicked()
{
    imtools->saveImg(*(imtools->getframe()),
                     (ui->imagename_lineedit->text().toStdString()));
}


void MainWindow::on_s_get_coors_pushButton_clicked()
{
    iop::int32 y0 = stage->YAxis().getCurrentPosition();
    iop::int32 x0 = stage->XAxis().getCurrentPosition();

    ui->s_xpos_label->setText("X: " + QString::number(x0,'f',2));
    ui->s_ypos_label->setText("Y: " + QString::number(y0,'f',2));
}

void MainWindow::MoveAction(){
    QTextStream(stdout) << "move action" << endl;
    std::vector<float> mouse ;
    mouse.push_back(point_mouse->x()*1.5f);
    mouse.push_back(point_mouse->y()*1.5f);
    float ipdata2[6] = {960.0f,100.0f,1820.0f,100.0f,980.0f,980.0f};
      cv::Mat cip = cv::Mat(2,3,CV_32F,ipdata2);

      *imgc= geticenter(cip);
    std::cout << *imgc << std::endl;
    cv::Mat pipc = getonimgpipettecoors(TM,mouse,*imgc,pc);
    std::cout << pipc << "pipette coors" << std::endl;
   // pipc.at<float>(0,0);
      QTextStream(stdout) << pipc.at<float>(0,1) << endl;
    apipc->setabsoluepositioning();
    apipc->moveToZSync(static_cast<float>(pipc.at<float>(0,2)));
    apipc->moveToXSync(static_cast<float>(pipc.at<float>(0,0)));
    apipc->moveToYSync(static_cast<float>(pipc.at<float>(0,1)));
}

void MainWindow::on_graphicsView_customContextMenuRequested(const QPoint &pos)
{

    QTextStream(stdout) << "isthis"<< endl;
    QMenu contextMenu(("Context menu"), ui->graphicsView->viewport());
    QAction action1("Move Here", ui->graphicsView->viewport());
    connect(&action1, SIGNAL(triggered()), this, SLOT(MoveAction()));
    QAction action2("Pick up", ui->graphicsView->viewport());

    QAction action3("Deploy", ui->graphicsView->viewport());
    contextMenu.addAction(&action1);
    contextMenu.addAction(&action2);
    contextMenu.addAction(&action3);
    contextMenu.exec(ui->graphicsView->viewport()->mapToGlobal(pos));

}


void MainWindow::on_actionCalibrate_Pipette_triggered()
{
    calib->Iscalibrating= true;
  //  calib->setModal(true);

    calib->exec();
}
