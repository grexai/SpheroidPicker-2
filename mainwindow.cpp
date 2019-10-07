#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyleFactory>
#include <QMouseEvent>
//#include <iostream>
#include <QStyle>
#include <QMessageBox>
#include <QProgressDialog>
#include <QLabel>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QProgressDialog progress("Getting User Interface ready", NULL, 0, 0);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.setValue(20);

    ui->setupUi(this);
    ui->graphicsView->setScene(new QGraphicsScene(this));
    ui->graphicsView->scene()->addItem(&qpxmi);
    ui->graphicsView_2->setScene(new QGraphicsScene(this));
    ui->graphicsView_2->scene()->addItem(&im_view_pxmi);
    ui->p_home_y->hide();
    ui->p_ymax->hide();
    ui->p_ym_button->hide();
    ui->p_yp_button->hide();
   // ui->P_C_box->hide();

    progress.setValue(20);
    imtools = new imagetools;
    timer = new QTimer(this);
    disp_pressure= new QTimer(this);
    ctrl = new controller;
    ui->scanning_progress->setValue(static_cast<int>(m_status));
    progress.setLabelText("configuring hardware");
    std::map<std::string, std::string> settings;
    propreader = new propertyreader;
    progress.setValue(50);
    propreader->read_settings("config.txt",settings);
    propreader->apply_settings(settings);
    ctrl->connect_microscope_unit(propreader->cfg.port_pipette,propreader->cfg.port_pressurecontrooler);
    progress.setLabelText("Loading neural network graph");
    progress.setValue(75);
    dl = new deeplearning;
    dl->setup_dnn_network(propreader->cfg.classesFile,propreader->cfg.model_weights,propreader->cfg.textGraph);
    progress.setLabelText("Loading neural network graph");
    progress.setValue(100);
    progress.setAutoClose(true);
/*
    setdarkstyle();*/


   // qApp->installEventFilter(this);
   // std::cout << "AKOS EDITION v1.1" << std::endl;
}

void MainWindow::setdarkstyle(){
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
    ui->lcdNumber->display( QString::number(static_cast<int>(ctrl->get_pressure())));
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
void MainWindow::keyPressEvent( QKeyEvent* e )
{
    switch( e->key() )
    {
    case Qt::Key_Plus: on_p_zp_button_clicked(); break;
    case Qt::Key_Minus: on_p_zm_btton_clicked();break;
    case Qt::Key_2: on_p_xm_button_clicked();break;
    case Qt::Key_8: on_p_xp_button_clicked();break;
    case Qt::Key_4: on_p_ym_button_clicked();break;
    case Qt::Key_6: on_p_yp_button_clicked();break;
    case Qt::Key_W:on_s_yp_button_clicked(); break;
    case Qt::Key_S: on_s_ym_button_clicked();break;
    case Qt::Key_A:on_s_xm_button_clicked();break;
    case Qt::Key_D:on_s_xp_button_clicked();break;
    case Qt::Key_Space:on_pc_pulse_button_clicked();break;
    case Qt::Key_Escape: on_actionExit_triggered();break;
    default: ;
    }
    QMainWindow::keyPressEvent( e );
}

void MainWindow::calib_frame_view(cv::Mat& disp){
    using namespace cv;
    if (Iscameraopen== true && calib!=nullptr )
    {
        if (calib->a.value()==0)
        {
            imtools->addPointToImage(disp,Point(disp.cols/2,100 ));
        }
        if (calib->a.value()==1){
            imtools->addPointToImage(disp,Point(100,disp.rows-100));
            if(cpos1 == nullptr)
            {
          //      cpos1 = new std::vector<float>;
          //      *cpos1 = ctrl->pipette_get_coordinates();
          //      QTextStream(stdout ) << "point 1 saved:" << cpos1->at(0) <<" "<<cpos1->at(1)<< " "<< cpos1->at(2) <<endl;
            }

        }
        if (calib->a.value()==2){
            imtools->addPointToImage(disp,Point(disp.cols-100,disp.rows-100));
            if(cpos2== nullptr)
            {
            //    cpos2 = new std::vector<float>;
            //    *cpos2 = ctrl->pipette_get_coordinates();
            //     QTextStream(stdout ) << "point 2 saved: " << cpos2->at(0) <<"y: "<<cpos2->at(1)<< "z: "<< cpos2->at(2) <<endl;
            }
        }
        if (calib->a.value()==3)
        {
            if(cpos3 == nullptr)
            {
            //    cpos3 = new std::vector<float>;
           //     *cpos3 = ctrl->pipette_get_coordinates();
           //     QTextStream(stdout ) << "point 3 saved: x: " << cpos3->at(0) <<"y: "<<cpos3->at(1)<< "z: "<< cpos3->at(2) <<endl;
          //      ctrl->pipette_calc_TM(cpos1,cpos2,cpos3);

           // calib->Iscalibrating = false;
              //   calib->deleteLater();

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
    qframe = new QImage(const_cast< unsigned char*>(displayfrm.data),displayfrm.cols, displayfrm.rows, QImage::Format_RGB888);
    qpxmi.setPixmap( QPixmap::fromImage(*qframe) );
    ui->graphicsView->fitInView(&qpxmi, Qt::KeepAspectRatio);
}

void MainWindow::on_predict_sph_clicked()
{
    auto cfrm = cameracv->get_current_frm();
    cv::Mat image;
    if (global_obj_im_coordinates != nullptr) {delete global_obj_im_coordinates;}
    global_obj_im_coordinates = new std::vector<std::vector<float>>;
    std::vector<std::vector<float>> im_obj = dl->dnn_prediction(*cfrm,image);
    global_obj_im_coordinates->push_back(im_obj.at(0));
 //   cv::Mat rgb = imtools->convert_bgr_to_rgb(image);
    qframe = new QImage(const_cast< unsigned char*>(image.data),image.cols,image.rows, QImage::Format_RGB888);
    im_view_pxmi.setPixmap( QPixmap::fromImage(*qframe) );
    ui->graphicsView_2->fitInView(&im_view_pxmi, Qt::KeepAspectRatio);
    ui->tabWidget->setCurrentWidget(ui->tab2);
    ui->found_objects->clear();

    for (int i=1 ;i<=im_obj.size();++i )
    {
        ui->found_objects->addItem(QString::number(i));
    }
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

void MainWindow::on_Home_pip_clicked()
{
    ctrl->pipette_xz_home();
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
    ui->xc_label->setText("X: " + QString::number(static_cast<double>(randx.at(0)),'f',2));
    ui->yc_label->setText("Y: " + QString::number(static_cast<double>(randx.at(1)),'f',2));
    ui->zc_label->setText("Z: " + QString::number(static_cast<double>(randx.at(2)),'f',2));
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
    QTextStream(stdout)<< "+ presseeed" << endl;
    ctrl->pipette_movex_sync(ui->pip_step_spinbox->value());
}

void MainWindow::on_p_xm_button_clicked()
{
    QTextStream(stdout)<< "- presseeed" << endl;
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
    QTextStream(stdout)<< "2 presseeed" << endl;
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

void MainWindow::move_action(){
    std::vector<float> mouse ;
    mouse.push_back(point_mouse->x()*1.5f);
    mouse.push_back(point_mouse->y()*1.5f);
    ctrl->pipette_move_to_img_coordinates(mouse);
}

void MainWindow::center_this_point_action()
{
    std::vector<float> mouse;
    ctrl->stage_set_speed(5000.0f);
    mouse.push_back(point_mouse->x()*1.5f);
    mouse.push_back(point_mouse->y()*1.5f);
    this->center_spheroid(mouse);
    ctrl->stage_set_speed(15000.0f);
}


void MainWindow::on_graphicsView_customContextMenuRequested(const QPoint &pos)
{
    QMenu contextMenu(("Context menu"), ui->graphicsView->viewport());
    QAction action1("Move Pipette Here", ui->graphicsView->viewport());
    connect(&action1, SIGNAL(triggered()), this, SLOT(move_action()));
    QAction action2("Center this postion", ui->graphicsView->viewport());
    connect(&action2, SIGNAL(triggered()), this, SLOT(center_this_point_action()));
    QAction action3("Pick up", ui->graphicsView->viewport());
    QAction action4("Deploy", ui->graphicsView->viewport());
    contextMenu.addAction(&action1);
    contextMenu.addAction(&action2);
    contextMenu.addAction(&action3);
    contextMenu.addAction(&action4);
    contextMenu.exec(ui->graphicsView->viewport()->mapToGlobal(pos));
}

void MainWindow::on_actionCalibrate_Pipette_triggered()
{
    if (Iscameraopen)
    {
        calib = new calibratedialog;
        calib->Iscalibrating= true;
        calib->show();
    }
}

std::string MainWindow::get_date_time_str()
{
    std::string datetime= "";
    datetime.append((QString::number(QDate::currentDate().year())).toStdString());
    datetime.append((QString::number(QDate::currentDate().month())).toStdString());
    datetime.append((QString::number(QDate::currentDate().day())).toStdString()+"-");
    datetime.append((QString::number(QTime::currentTime().hour())).toStdString());
    datetime.append((QString::number(QTime::currentTime().minute())).toStdString());
    return datetime;
}

void MainWindow::screensample()
{
    using namespace  cv;
    ui->pip_c_box->setDisabled(true);
    ui->S_c_box->setDisabled(true); //  int platesize= 350000; //    100nm (means one step) --> for the stage
    ui->syringe_box->setDisabled(true);
    const float platesize= ui->set_plate_size_spinbox->value()*10000;
    float  img_w_5p5x = 27426;  //    100nm
    float  img_h_5p5x = 15421;  //    100nm
    int object_counter = 0;
    std::string folder = "Scandata_";
    std::string datetime = "_";
    std::stringstream ss;
    ss << (ui->set_plate_size_spinbox->value());
    std::string plate_size_mm = ss.str();
    folder.append(plate_size_mm).append("mm_");
    datetime.append((QString::number(QDate::currentDate().year())).toStdString());
    datetime.append((QString::number(QDate::currentDate().month())).toStdString());
    datetime.append((QString::number(QDate::currentDate().day())).toStdString()+"-");
    datetime.append((QString::number(QTime::currentTime().hour())).toStdString());
    datetime.append((QString::number(QTime::currentTime().minute())).toStdString());
    folder.append(datetime+"/");
    if (QDir().exists(folder.c_str())){
        QTextStream(stdout) << "this folder already folder exists"<< endl;
    }
    else{
        QTextStream(stdout)<< folder.c_str()<< "created" << endl;
        QDir().mkdir(folder.c_str());
    }
    QTextStream(stdout)<< "starting..";
    const int xpos=ctrl->stage_get_x_coords();
    const int ypos=ctrl->stage_get_y_coords();
    const int wmax = static_cast<int>(platesize/img_w_5p5x); // um
    const int hmax = static_cast<int>(platesize/img_h_5p5x); // um
    QTextStream(stdout)<< "wmax: "<<wmax << " hmax" << hmax;
    int counter = 0;
    float p_v=0.0f;
    folder.append("s_"+datetime);
    if (global_obj_im_coordinates != nullptr) {delete global_obj_im_coordinates;}
    global_obj_im_coordinates = new std::vector<std::vector<float>>;
    for (int j = 0; j < hmax; ++j )
    {
        ctrl->stage_set_speed(7000.0f);
        ctrl->stage_move_to_y_sync(static_cast<int>(ypos+img_h_5p5x*j));
        ctrl->stage_set_speed(5000.0f);
        for (int  i = 0; i< wmax; ++i)
        {
            if(m_s_t_acitive)
            {
                ctrl->stage_move_to_x_sync(static_cast<int>(xpos+img_w_5p5x*i));
                p_v= (static_cast<float>((wmax)*j+i)/static_cast<float>((hmax)*(wmax)))*100;
                QTextStream(stdout)<<"progvalue"<< p_v<< endl;
                prog_changed(static_cast<int>(p_v));
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                int leading = 2;
                std::string num2str= folder + "_H" + std::to_string(j*0.000001).substr(8-leading)+ "_W" + std::to_string(i*0.000001).substr(8-leading);
                std::string posy = std::to_string(j+1)+ "/" + std::to_string(hmax);
                std::string posx = std::to_string(i+1)+ "/" + std::to_string(wmax);
                ui->current_scaningpos->setText(("Scaning pos: W: "+posx +" H: " + posy).c_str() );
                auto tmp = cameracv->get_current_frm();
                scanvector.push_back(*tmp.get());
                imtools->saveImg(tmp.get(),num2str.c_str());
                std::vector<std::vector<float>>im_objects = dl->dnn_prediction(scanvector.at(counter),scanvector.at(counter));
                for (int k =0; k<im_objects.size();++k)
                {
                   global_obj_im_coordinates->push_back( this->get_centered_coordinates(im_objects.at(k)));
                   QTextStream (stdout) <<object_counter << endl;
                   object_counter++;
                }
                counter++;
            }else{
                break;
            }
        }

    }
    prog_changed(100);
    m_s_t_acitive=false;
    ctrl->stage_set_speed(20000.0f);
    ui->found_objects->clear();
    for (int i=1 ;i<=global_obj_im_coordinates->size();++i )
    {
        ui->found_objects->addItem(QString::number(i));
    }
    ui->pip_c_box->setEnabled(true);
    ui->S_c_box->setEnabled(true);
    ui->syringe_box->setEnabled(true);
    ui->start_screening->setText("Start Screening");
   /* if(m_screening_thread->joinable())
    {
        m_screening_thread->join();
        QMessageBox::information(this,"Scanning done","Scanning of the plate finished,pipette and stage controller unlocked" );
    }*/
}

void MainWindow::on_start_screening_clicked()
{
    if(!m_s_t_acitive)
    {
        m_s_t_acitive = true;
        connect(this, SIGNAL(prog_changed(int)),
            this, SLOT(set_progressbar(int)),Qt::QueuedConnection);
        m_screening_thread = new std::thread(&MainWindow::screensample,this);
        ui->start_screening->setText("Stop Screening");
    }
    else
    {
        m_s_t_acitive= false;
        ui->pip_c_box->setEnabled(true);
        ui->S_c_box->setEnabled(true);
        ui->start_screening->setText("Start Screening");
        if(m_screening_thread->joinable())
        {
            m_screening_thread->join();
            m_screening_thread->~thread();
            scanvector.clear();
            global_obj_im_coordinates->clear();
        }
        QMessageBox::warning(this,"Scanning canceled",
                             "Scanning of the plate is canceled,pipette and stage controller unlocked");
    }
}

void MainWindow::set_progressbar( int value ){
    if ( value != m_progvalue ) {
        m_progvalue = value;
        ui->scanning_progress->setValue(m_progvalue );
    //    scan_progress
        QTextStream(stdout)<< m_progvalue;
        emit prog_changed( m_progvalue );
    }
}

void MainWindow::set_h4(int value)
{
    if(value == 0)
    {
        ui->P_C_box->hide();
        ui->syringe_box->show();
    }
    if (value == 1)
    {
       ui->P_C_box->show();
       ui->syringe_box->hide();
    }


}

void MainWindow::on_actionSpheroid_picker_triggered()
{
    std::map<std::string, std::string> settings;
    propreader = new propertyreader;
    propreader->read_settings("config.txt",settings);
    propreader->apply_settings(settings);
    ctrl->connect_microscope_unit(propreader->cfg.port_pipette,propreader->cfg.port_pressurecontrooler);
    dl = new deeplearning;
    dl->setup_dnn_network(propreader->cfg.classesFile,propreader->cfg.model_weights,propreader->cfg.textGraph);
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



void MainWindow::pickup_sph()
{
    QTextStream(stdout) << "pciking up" << endl;
    //ctrl->pipette_move_to_img_coordinates(dl->objpos.at(0));
    std::this_thread::sleep_for(std::chrono::milliseconds(7000));
    ctrl->vaccum_pulse(static_cast<float>(ui->pulse_value->value()),static_cast<float>(ui->pulse_time->value()));
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    ctrl->pipette_home_z();
}

std::vector<float> MainWindow::get_centered_coordinates(std::vector<float> sph_coors)
{
    std:: vector<float> center_coordinates;
    const float  img_w_5p5x = 27426; //    100nm
    const float  img_h_5p5x = 15421;
    const float x_conv =  img_w_5p5x/1920;
    const float y_conv = img_h_5p5x/1080;
    QTextStream(stdout) << x_conv <<"xc : yc " << y_conv << endl;
    QTextStream(stdout) << "dla0a0:  "<< sph_coors.at(0) << endl;
    QTextStream(stdout) << "dla0a1:  " <<  sph_coors.at(1) << endl;
    float mid_obj_pxl_x =  960 - sph_coors.at(0);
    float mid_obj_pxl_y =  540 - sph_coors.at(1);
    float obj_pos_um_x = ( mid_obj_pxl_x *x_conv);
    float obj_pos_um_y = (  mid_obj_pxl_y *y_conv);
    center_coordinates.push_back( ctrl->stage_get_x_coords()-(obj_pos_um_x));
    center_coordinates.push_back(  ctrl->stage_get_y_coords()-(obj_pos_um_y));
    return center_coordinates;
}



void MainWindow::center_spheroid(std::vector<float> coors)
{
    ctrl->stage_set_speed(5000.0f);
    std:: vector<float> c_coords = this->get_centered_coordinates(coors);
    ctrl->stage_move_to_x_sync(static_cast<int>(c_coords.at(0)));
    ctrl->stage_move_to_y_sync(static_cast<int>(c_coords.at(1)));
    ctrl->stage_set_speed(15000.0f);
}

void MainWindow::xz_stage_pickup_sph(){
  //  ctrl->pipette_movex_sync(-0.3); // center
 //   ctrl->pipette_movez_sync(0.2);

   // ctrl->pipette_extrude_relative(-0.1);
   // ctrl->pipette_movex_sync(-0.3); // center
   // ctrl->pipette_movez_sync(0.2);
}

void MainWindow::on_pickup_sph_clicked()
{
    //this->center_spheroid(dl->objpos.at(ui->n_obj->value()));
  //  this->center_spheroid();
    QTextStream (stdout) <<"global coors"<<global_obj_im_coordinates->at(ui->found_objects->currentIndex()).at(0) <<":" <<global_obj_im_coordinates->at(ui->found_objects->currentIndex()).at(1)<< endl;
    ctrl->stage_move_to_x_sync(static_cast<int>(global_obj_im_coordinates->at(ui->found_objects->currentIndex()).at(0)));
    ctrl->stage_move_to_y_sync(static_cast<int>(global_obj_im_coordinates->at(ui->found_objects->currentIndex()).at(1)));
    //std::thread t2(&MainWindow::pickup_sph,this);
    //t2.detach();
}


void MainWindow::on_view_scan_clicked()
{
    using namespace  cv;
    //cv::Mat* Mimage = new cv::Mat(cv::Mat::zeros((hmax * 1080), (wmax * 1920), CV_8UC3));
   // int platesize = 350000; //    100nm // *0.1um

    const float platesize = ui->set_plate_size_spinbox->value()*10000;
    float  img_w_5p5x = 27426; //    100nm
    float  img_h_5p5x = 15421;//19466; //    100nm
    int wmax = static_cast<int>(platesize / img_w_5p5x); // um
    int hmax = static_cast<int>(platesize / img_h_5p5x); // um

    cv::Mat* Mimage = new cv::Mat(cv::Mat::zeros((hmax * 1080), (wmax * 1920), CV_8UC3));
    for (int i = 0; i < hmax; ++i)
    {
        for (int j = 0; j < wmax; ++j)
        {

            for (int ii = 0; ii < scanvector.at(i*wmax + j).cols; ++ii)
            {
                for (int jj = 0; jj < scanvector.at(i*wmax + j).rows; ++jj)
                {
                    Mimage->at<Vec3b>((jj + (i * 1080)), (ii + (j * 1920)))[0] = scanvector.at(i*wmax + j).at<Vec3b>(jj, ii)[0];
                    Mimage->at<Vec3b>((jj + (i * 1080)), (ii + (j * 1920)))[1] = scanvector.at(i*wmax + j).at<Vec3b>(jj, ii)[1];
                    Mimage->at<Vec3b>((jj + (i * 1080)), (ii + (j * 1920)))[2] = scanvector.at(i*wmax + j).at<Vec3b>(jj, ii)[2];
                }
            }


        }
    }
    imtools->saveImg(Mimage,"mozaic");
    qframe = new QImage(const_cast< unsigned char*>(Mimage->data),Mimage->cols,Mimage->rows, QImage::Format_RGB888);
    im_view_pxmi.setPixmap( QPixmap::fromImage(*qframe) );
    ui->graphicsView_2->fitInView(&im_view_pxmi, Qt::KeepAspectRatio);
    ui->tabWidget->setCurrentWidget(ui->tab2);
    scanvector.clear();
    prog_changed(0);
}

void MainWindow::on_p_ep_button_clicked()
{
    ctrl->pipette_extrude_relative(static_cast<float>(ui->p_extruder_step_box->value()));
}

void MainWindow::on_p_em_button_clicked()
{
    ctrl->pipette_extrude_relative(static_cast<float>(-(ui->p_extruder_step_box->value())));
}


void MainWindow::closeEvent (QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Automatic Spheroid Picker",
                                                                tr("Are you sure?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();

    } else {
        event->accept();
        QCoreApplication::quit();
        //this->close();
        this->parentWidget()->show();
    }
}

void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::quit();
}

void MainWindow::on_actionHW_selector_triggered()
{
    this->parentWidget()->show();
    this->deleteLater();
}
