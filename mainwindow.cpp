#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyleFactory>
#include <QMouseEvent>
#include <iostream>

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
    ctrl = new controller;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_currentpressure(){
    float cp = ctrl->get_pressure();
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
                point_mouse = new QPoint;
            }else{
                point_mouse = new QPoint;
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
    if (calib->Iscalibrating == 1)
    {
        Point pmid = Point(imtools->getdisplayframe()->cols/2,100 );

        if (calib->clicks==0)
        {
            imtools->addPointToImage(pmid);
        }
        if (calib->clicks==1){
            imtools->addPointToImage(Point(100,imtools->getdisplayframe()->rows-100));
            if(cpos1== nullptr)
               {
               cpos1 = new std::vector<float>;
               *cpos1 = ctrl->pipette_get_coordinates();
               QTextStream(stdout ) << "point 1 saved:" ;//<< cpos1->at(0) <<" "<<cpos1->at(1)<< " "<< cpos1->at(2) <<endl;
               }

        }
        if (calib->clicks==2){
            imtools->addPointToImage(Point(imtools->getdisplayframe()->cols-100,imtools->getdisplayframe()->rows-100));
            if(cpos2== nullptr)
            {
               cpos2 = new std::vector<float>;
               *cpos2 = ctrl->pipette_get_coordinates();
               QTextStream(stdout ) << "point 2 saved: ";// << cpos2->at(0) <<" "<<cpos2->at(1)<< " "<< cpos2->at(2) <<endl;
            }
        }
        if (calib->clicks==3)
        {
            if(cpos3 == nullptr)
            {
                cpos3 = new std::vector<float>;
                *cpos3 = ctrl->pipette_get_coordinates();
                QTextStream(stdout ) << "point 3 saved: x: " ;//<< cpos3->at(0) <<" "<<cpos3->at(1)<< " "<< cpos3->at(2) <<endl;
              //  calib->clicks=0;
            }
        }
    }
}

void MainWindow::update_window()
{
    imtools->getCameraframe();
    calib_frame_view();
    if (qframe != nullptr)
    {
        delete[] qframe;
    }
    auto qframe = new QImage((const unsigned char*) (imtools->getdisplayframe()->data),imtools->getdisplayframe()->cols, imtools->getdisplayframe()->rows, QImage::Format_RGB888);
    qpxmi.setPixmap( QPixmap::fromImage(*qframe) );
    ui->graphicsView->fitInView(&qpxmi, Qt::KeepAspectRatio);

}

void MainWindow::on_Campushbtn_clicked()
{
    if ((imtools->iscameraopen)){
        ui->Campushbtn->setText("Camera on");
     //   imtools->resetvideodevice();
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
        //ui->s_xm_button->installEventFilter(this)
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
    float w =  ui->width_spin->value();
    imtools->setimagewidth(w);
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
    ctrl->pipette_home();
}

void MainWindow::on_p_home_x_clicked()
{
    ctrl->pipette_home_x();
}

void MainWindow::on_p_home_y_clicked()
{
    ctrl->pipette_home_y();
}

void MainWindow::on_p_home_z_clicked()
{
    ctrl->pipette_home_z();

}

void MainWindow::on_SetPressure_clicked()
{
    ctrl->request_pressure(ui->pressure_value->value());
}

void MainWindow::on_atm_button_clicked()
{
    ctrl->request_atm();
}

void MainWindow::on_pc_pulse_button_clicked()
{
    ctrl->vaccum_pulse(ui->pulse_value->value(),ui->pulse_time->value());

}

void MainWindow::on_get_coors_pushButton_clicked()
{
    std::vector<float> randx= ctrl->pipette_get_coordinates();
    ui->xc_label->setText("X: " + QString::number(randx.at(0),'f',2));
    ui->yc_label->setText("Y: " + QString::number(randx.at(1),'f',2));
    ui->zc_label->setText("Z: " + QString::number(randx.at(2),'f',2));
}

void MainWindow::on_s_xp_button_clicked()
{
    ctrl->stage_move_x_async(ui->s_step_spinbox->value());
}

void MainWindow::on_s_xm_button_clicked()
{
    ctrl->stage_move_x_async(-(ui->s_step_spinbox->value()));
}

void MainWindow::on_s_yp_button_clicked()
{
    ctrl->stage_move_y_async(ui->s_step_spinbox->value());
}

void MainWindow::on_s_ym_button_clicked()
{
    ctrl->stage_move_y_async(-ui->s_step_spinbox->value());
}

void MainWindow::on_p_xp_button_clicked()
{
    ctrl->pipette_movex_sync(ui->pip_step_spinbox->value());
}

void MainWindow::on_p_xm_button_clicked()
{
    ctrl->pipette_movex_sync(-(ui->pip_step_spinbox->value()));

}

void MainWindow::on_p_yp_button_clicked()
{
    ctrl->pipette_movey_sync(ui->pip_step_spinbox->value());
}

void MainWindow::on_p_ym_button_clicked()
{
    ctrl->pipette_movey_sync(-(ui->pip_step_spinbox->value()));
}

void MainWindow::on_p_zp_button_clicked()
{
    ctrl->pipette_movez_sync(ui->pip_step_spinbox->value());
}


void MainWindow::on_p_zm_btton_clicked()
{
    ctrl->pipette_movez_sync(-(ui->pip_step_spinbox->value()));
}

void MainWindow::on_actionOpen_console_triggered()
{
    QTextStream(stdout) << "yo program  has succesfully crashed. is not works" << endl;
}

void MainWindow::on_s_center_button_clicked()
{
    ctrl->stage_go_center();
}


void MainWindow::on_s_set_speed_button_clicked()
{
    ctrl->stage_set_speed(ui->s_speed_spinbox->value());
}

void MainWindow::on_save_image_button_clicked()
{
    imtools->saveImg((imtools->getframe()),
                     (ui->imagename_lineedit->text().toStdString()));
}

void MainWindow::on_s_get_coors_pushButton_clicked()
{
    std::vector<int> coords = ctrl->stage_get_coords();
    ui->s_xpos_label->setText("X: " + QString::number(coords.at(0),'f',2));
    ui->s_ypos_label->setText("Y: " + QString::number(coords.at(1),'f',2));
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

    std:: cout<< "now we will strike : "<< std::endl;
    std::cout << centers.img << std::endl;
    cv::Mat pipc = calconimgpipettecoors(TM,mouse,*imgc,pc);
    std::cout << pipc << "pipette coors" << std::endl;
   // pipc.at<float>(0,0);
      QTextStream(stdout) << pipc.at<float>(0,1) << endl;
      ctrl->pipette_movez_sync(static_cast<float>(pipc.at<float>(0,2)));
      ctrl->pipette_movex_sync(static_cast<float>(pipc.at<float>(0,0)));
      ctrl->pipette_movey_sync(static_cast<float>(pipc.at<float>(0,1)));
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
    calib->show();
}

void MainWindow::screensample(){
    if (QDir().exists("Scandata")){
        QTextStream(stdout) << "this folder already folder exists"<< endl;
    }
    else{
        QDir().mkdir("Scandata");
    }
    //X 710798   Y-805545
    iop::int32 xp = 710798;// =stage->XAxis().getCurrentPosition();
    iop::int32 yp = 805545;//= stage->YAxis().getCurrentPosition();
    int xpos=ctrl->stage_get_x_coords();
    int ypos=ctrl->stage_get_y_coords();
    std::cout<< "ypos" << ypos<< "xpos" << xpos<< std::endl;
    int platesize= 300000; //    /100nm
    float  img_w_5p5x = 27426; //  //100nm
    float  img_h_5p5x = 19466; //  um
    int wmax = platesize/img_w_5p5x; // um
    int hmax = platesize/img_h_5p5x; // um
    ctrl->stage_set_speed(7500.0f);
    QTextStream(stdout)<< "wmax: "<<wmax << " hmax" << hmax;
    int counter = 1;
    std::string folder ="Scandata/screening";
    for (int j = 0; j< hmax; j++ )
    {
        for (int  i = 0; i< wmax; i++)
        {
            QTextStream(stdout)<< "i" << i << endl;
            ctrl->stage_move_to_x_sync(xpos+img_w_5p5x*i);
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            std::cout<< "c" << counter << std::endl;
            std::string num2str= folder + "_W" + std::to_string(i)+ "_H" + std::to_string(j);
            imtools->saveImg((imtools->currentFrame.get()),num2str.c_str());
            counter += 1;
        }
        ctrl->stage_set_speed(20000.0f);
        ctrl->stage_move_to_y_sync(ypos+img_h_5p5x*j);
        ctrl->stage_set_speed(7500.0f);
    }
}

void spawnthread(){

}

void MainWindow::on_start_screening_clicked()
{
    std::thread t1(&MainWindow::screensample,this);
    t1.detach();
}

void MainWindow::on_pushButton_5_clicked()
{

    float ipdata2[6] = {960.0f,100.0f,1820.0f,100.0f,980.0f,980.0f};
    cv::Mat cip = cv::Mat(2,3,CV_32F,ipdata2);
    /*
    // image coordinates 2 x 3 matrix image
    //expected format

      Center point
         ||
         ||
        \  /
         \/

       | x1  x2  x3 |
       | y1  y2  y3 |
    */


    // pushing saved pipette positions to a Matrix
    cv::Mat randmat;
    randmat.push_back(*cpos1);
    randmat.push_back(*cpos2);
    randmat.push_back(*cpos3);
    std::cout << randmat << "before reshape"<< endl;
    /*
    // converting to pipette coordinates a 3 x 3 matrix
      Center point
         ||
         ||
        \  /
         \/
       | x1  x2  x3 |
       | y1  y2  y3 |
       | z1  z2  z3 |
    */
    randmat =  randmat.reshape(0,3);
    cv::transpose(randmat,randmat);
    std::cout << randmat<< "randmat"<< std::endl;
    TM = calcTMatrix(randmat,cip,centers);
    //ic = new cv::Mat;
    imgc= new cv::Mat(1,2,CV_32F);
    *imgc = geticenter(cip);
    pc= getpcenter(randmat);
    QTextStream(stdout)<<"done" << endl;
}


void MainWindow::on_actionSpheroid_picker_triggered()
{
       ctrl->connect_microscope_unit();
}
