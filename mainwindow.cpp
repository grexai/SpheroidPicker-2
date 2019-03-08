#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyleFactory>
#include <QMouseEvent>
//#include <iostream>
#include <QStyle>

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

void MainWindow::setdefault()
{
   qApp->setStyle(QStyleFactory::create("WindowsDefault"));
   qApp->setPalette(this->style()->standardPalette());
   qApp->setStyleSheet("");
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
    setdarkstyle();
    std::cout << "AKOS EDITION v1.1" << std::endl;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_currentpressure(){
  //  float cp = (QRandomGenerator::global()->generate());
  /*  if (ctrl->get_current_pressure()==nullptr){
        return;
    }else
    {
*/
    ui->lcdNumber->display( QString::number(ctrl->get_pressure()));
   // ui->lcdNumber->display( "13");
    //}
}

void MainWindow::show_currentpressure(){
   // ctrl->spawn_pressure_thread();
    connect(disp_pressure, SIGNAL(timeout()), this, SLOT(update_currentpressure()));
    disp_pressure->start(500);
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

void MainWindow::calib_frame_view(cv::Mat& disp){
    using namespace cv;
    if (calib->Iscalibrating == 1)
    {
        Point pmid = Point(disp.cols/2,100 );

        if (calib->clicks==0)
        {
            imtools->addPointToImage(disp,pmid);
        }
        if (calib->clicks==1){
            imtools->addPointToImage(disp,Point(100,disp.rows-100));
            if(cpos1 == nullptr)
            {
                cpos1 = new std::vector<float>;
                *cpos1 = ctrl->pipette_get_coordinates();
                QTextStream(stdout ) << "point 1 saved:" << cpos1->at(0) <<" "<<cpos1->at(1)<< " "<< cpos1->at(2) <<endl;
            }

        }
        if (calib->clicks==2){
            imtools->addPointToImage(disp,Point(disp.cols-100,disp.rows-100));
            if(cpos2== nullptr)
            {
                cpos2 = new std::vector<float>;
                *cpos2 = ctrl->pipette_get_coordinates();
                QTextStream(stdout ) << "point 2 saved: " << cpos2->at(0) <<"y: "<<cpos2->at(1)<< "z: "<< cpos2->at(2) <<endl;
            }

        }
        if (calib->clicks==3)
        {
            if(cpos3 == nullptr)
            {
                cpos3 = new std::vector<float>;
                *cpos3 = ctrl->pipette_get_coordinates();
                QTextStream(stdout ) << "point 3 saved: x: " << cpos3->at(0) <<"y: "<<cpos3->at(1)<< "z: "<< cpos3->at(2) <<endl;
            }
        }
    }
}

void MainWindow::update_window()
{
    auto cfrm=  cameracv->get_current_frm();
    if (cfrm == nullptr)
    {
        return;
    }
    cv::Mat displayfrm = imtools->convert_bgr_to_rgb(cfrm);
    calib_frame_view(displayfrm);
    delete qframe;
    //qframe = new QImage((const unsigned char*) displayfrm.data,displayfrm.cols, displayfrm.rows, QImage::Format_RGB888);
    qframe = new QImage(const_cast< unsigned char*>(displayfrm.data),displayfrm.cols, displayfrm.rows, QImage::Format_RGB888);
    qpxmi.setPixmap( QPixmap::fromImage(*qframe) );
    ui->graphicsView->fitInView(&qpxmi, Qt::KeepAspectRatio);
}

void MainWindow::on_Campushbtn_clicked()
{

    cameracv = new CameraCV(cameraIndex);
    if (Iscameraopen==true){
        ui->Campushbtn->setText("Camera on");
        cameracv->~CameraCV();
        Iscameraopen= false;
        disconnect(timer, SIGNAL(timeout()), this,nullptr);
    }else{
      //  imtools->setvideodevice(cameraIndex);
        if(!cameracv->getCamera()->open(cameraIndex))
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
        cameracv->setimagewidth(w);
        cameracv->setimageheight(h);
        setMouseTracking(true);

        ui->graphicsView->installEventFilter(this);
        cameracv->spawnCameraLoop();
        Iscameraopen = cameracv->is_camera_open();
        connect(timer, SIGNAL(timeout()), this, SLOT(update_window()));
        timer->start(16);
    }
}

void MainWindow::on_exptime_button_clicked()
{
    cameracv->setexposuretime(static_cast<float>(ui->exptime_spin->value()));
}

void MainWindow::on_width_button_clicked()
{
    float w =  ui->width_spin->value();
    cameracv->setimagewidth(w);
}

void MainWindow::on_height_button_clicked()
{
    float h =  ui->height_spin->value();
    cameracv->setimagewidth(h);
}

void MainWindow::on_actionDark_Mode_triggered()
{
    setdarkstyle();
}

void MainWindow::on_actionLight_triggered()
{
    setdefault();
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
    ctrl->request_pressure(static_cast<float>(ui->pressure_value->value()));
}

void MainWindow::on_atm_button_clicked()
{
    ctrl->request_atm();
}

void MainWindow::on_pc_pulse_button_clicked()
{
    ctrl->vaccum_pulse(static_cast<float>(ui->pulse_value->value()),static_cast<float>(ui->pulse_time->value()));
}

void MainWindow::on_get_coors_pushButton_clicked()
{
    std::vector<float> randx = ctrl->pipette_get_coordinates();
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
    QTextStream(stdout) << "your program  has succesfully crashed." << endl;
}

void MainWindow::on_s_center_button_clicked()
{
    ctrl->stage_go_center();
}

void MainWindow::on_save_image_button_clicked()
{
    imtools->saveImg((cameracv->get_current_frm().get()),
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
    ctrl->pipette_move_to_img_coordinates(mouse);
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
    using namespace  cv;
    if (QDir().exists("Scandata")){
        QTextStream(stdout) << "this folder already folder exists"<< endl;
    }
    else{
        QDir().mkdir("Scandata");
    }
    //X 710798   Y-805545
    int xpos=ctrl->stage_get_x_coords();
    int ypos=ctrl->stage_get_y_coords();
    std::cout<< "ypos" << ypos<< "xpos" << xpos<< std::endl;
    int platesize= 300000;     //    100nm
    float  img_w_5p5x = 27426; //    100nm
    float  img_h_5p5x = 19466; //    100nm
    int wmax = static_cast<int>(platesize/img_w_5p5x); // um
    int hmax = static_cast<int>(platesize/img_h_5p5x); // um
    ctrl->stage_set_speed(7500.0f);
    QTextStream(stdout)<< "wmax: "<<wmax << " hmax" << hmax;
    int counter = 1;
    cv::Mat* Mimage = new Mat();
    *Mimage =  cv::Mat::zeros(hmax*2100,wmax*2000,CV_8UC3);
    std::vector<std::vector<cv::Mat>> scanvector;
    std::string folder ="Scandata/screening";
    for (int j = 1; j <= hmax; ++j )
    {
        for (int  i = 1; i<= wmax; ++i)
        {
           // QTextStream(stdout)<< "i" << i << endl;
            ctrl->stage_move_to_x_sync(static_cast<int>(xpos+img_w_5p5x*i));
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
          //  std::cout<< "c" << counter << std::endl;
            std::string num2str= folder + "_W" + std::to_string(i)+ "_H" + std::to_string(j);
            std::string posy = std::to_string(j)+ "/" + std::to_string(hmax);
            std::string posx = std::to_string(i)+ "/" + std::to_string(wmax);
            ui->current_scaningpos->setText(("Scaning pos: W: "+posx +" H: " + posy).c_str() );
            cv::Mat *tmpimg = cameracv->get_current_frm().get();
            imtools->saveImg(tmpimg,num2str.c_str());

            // mosaik creaton
        //    scanvector.at(i).at(j) = *tmpimg;
            for (int ii = 1; ii < tmpimg->cols;++ii)
                {
                    for (int jj = 1; jj < tmpimg->rows; ++jj)
                    {
                        Mimage->at<Vec3b>(jj+(j*1080),ii+(i*1920))[0] = tmpimg->at<Vec3b>(jj, ii)[0];
                        Mimage->at<Vec3b>(jj+(j*1080),ii+(i*1920))[1] = tmpimg->at<Vec3b>(jj, ii)[1];
                        Mimage->at<Vec3b>(jj+(j*1080),ii+(i*1920))[2] = tmpimg->at<Vec3b>(jj, ii)[2];
                    }
                }
            //till this
            counter += 1;
        }
        ctrl->stage_set_speed(20000.0f);
        ctrl->stage_move_to_y_sync(static_cast<int>(ypos+img_h_5p5x*j));
        ctrl->stage_set_speed(7500.0f);
        imtools->saveImg(Mimage,"mozaic");
    }
}

void MainWindow::on_start_screening_clicked()
{
    std::thread t1(&MainWindow::screensample,this);
    t1.detach();
}

void MainWindow::on_pushButton_5_clicked()
{
    ctrl->pipette_calc_TM(cpos1,cpos2,cpos3);
}

void MainWindow::on_actionSpheroid_picker_triggered()
{
    ctrl->connect_microscope_unit();
}

void MainWindow::on_p_set_speed_spinbox_valueChanged(int arg1)
{
     QTextStream(stdout)<< "speed changed to: " << arg1 ;
     ctrl->pipette_set_speed(arg1);
}


void MainWindow::on_s_speed_spinbox_valueChanged(double arg1)
{
     ctrl->stage_set_speed(static_cast<float>(arg1));
}

void MainWindow::on_predict_sph_clicked()
{
    dl = new deeplearning;
    auto cfrm = cameracv->get_current_frm();
    dl->dnn_prediction(*cfrm);
}

void MainWindow::pickup_sph()
{
    QTextStream(stdout) << "pciking up" << endl;
    ctrl->pipette_move_to_img_coordinates(dl->objpos.at(0));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    ctrl->vaccum_pulse(static_cast<float>(ui->pulse_value->value()),static_cast<float>(ui->pulse_time->value()));
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    ctrl->pipette_home_z();
}

void MainWindow::on_pickup_sph_clicked()
{
    std::thread t2(&MainWindow::pickup_sph,this);
    t2.detach();
}






