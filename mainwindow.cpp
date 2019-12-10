#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyleFactory>
#include <QMouseEvent>
#include <QStyle>
#include <QMessageBox>
#include <QLabel>
#include <marcros.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QProgressDialog progress("Getting User Interface ready", nullptr, 0, 0);
    progress.setRange(0,100);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.setValue(0);

    ui->setupUi(this);
    ui->graphicsView->setScene(new QGraphicsScene(this));
    ui->graphicsView->scene()->addItem(&qpxmi);
    ui->graphicsView_2->setScene(new QGraphicsScene(this));
    ui->graphicsView_2->scene()->addItem(&im_view_pxmi);
   // ui->P_C_box->hide();
    progress.setValue(10);
    imtools = new imagetools;
    timer = new QTimer(this);
   // timer->setSingleShot(false); // always restarts
    disp_pressure= new QTimer(this);
    ctrl = new controller;
    m_s_t_acitive.store(false);
    ui->scanning_progress->setValue(static_cast<int>(m_status));
    progress.setValue(25);
    progress.setLabelText("configuring hardware");
    std::map<std::string, std::string> settings;
    propreader = new propertyreader;
    propreader->read_settings("config.txt",settings);
    propreader->apply_settings(settings);

    ctrl->connect_microscope_unit(propreader->cfg.port_pipette,propreader->cfg.port_pressurecontrooler);

    progress.setValue(50);
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Automatic Spheroid Picker",
                                                                tr("Do yo want to run stage inicialization?\n"),
                                                                QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes) {
            progress.setLabelText("Stage inicialization, may take about 1 minute");
            ctrl->stage_set_speed(30000);
            ctrl->stage_autocalibrate();
    }
    ctrl->stage_set_speed(ui->s_speed_spinbox->value());
    progress.setValue(75);
    progress.setLabelText("Loading neural network graph");
    QMessageBox::StandardButton res1Btn = QMessageBox::question( this, "Automatic Spheroid Picker",
                                                                tr("Load Matterport Rcnn?"),
                                                                QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (res1Btn == QMessageBox::Yes) {
        dl = new matterport_mrcnn();
        dl->setup_dnn_network(propreader->cfg.matterport_graph.c_str(),
                              propreader->cfg.matterport_folder.c_str(),
                              nullptr);
        cv::Mat test= cv::imread("BIOMAGwhite-01.png", cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
        // start a fake inference for finish preload
        std::thread t_inf(&i_deeplearning::dnn_inference,dl,
                          test,test);
        t_inf.detach();
    }else{
        dl = new invecption_v2();
        dl->setup_dnn_network(propreader->cfg.classesFile.c_str(),
                          propreader->cfg.model_weights.c_str(),
                          propreader->cfg.textGraph.c_str());
    }

    automethods = new auto_methods(ctrl,cameracv,dl);
    // USER interface connections
    connect(this, SIGNAL(scan_finished()),this,SLOT(scan_stopped()));
    connect(this, SIGNAL(stiched_img_rdy()),this,SLOT(show_on_view_2()));
    connect(this, SIGNAL(prog_changed(int)),
        this, SLOT(set_progressbar(int)),Qt::QueuedConnection);
    connect(this, SIGNAL(stich_prog_changed(int)),
        this, SLOT(set_stich_progressbar(int)),Qt::QueuedConnection);
    ui->magnification->activated(6);
    progress.setLabelText("Done");
    progress.setValue(100);
    progress.setAutoClose(true);
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
   qApp->style()->standardPalette();
 //  qApp->setStyle(this->style()->standardIcon());
 //  qApp->setStyle(this->style()->standardPixmap());
   qApp->setStyleSheet("");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_currentpressure()
{
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

void MainWindow::show_currentpressure()
{
   // ctrl->spawn_pressure_thread();
    connect(disp_pressure, SIGNAL(timeout()), this, SLOT(update_currentpressure()));
    disp_pressure->start(500);
}

//Events

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
    case Qt::Key_4: on_p_xm_button_clicked();break;
    case Qt::Key_6: on_p_xp_button_clicked();break;
    case Qt::Key_2: on_p_ym_button_clicked();break;
    case Qt::Key_8: on_p_yp_button_clicked();break;
    case Qt::Key_W:on_s_yp_button_clicked(); break;
    case Qt::Key_S: on_s_ym_button_clicked();break;
    case Qt::Key_A:on_s_xm_button_clicked();break;
    case Qt::Key_D:on_s_xp_button_clicked();break;
    case Qt::Key_1:on_p_em_button_clicked();break;
    case Qt::Key_3:on_p_ep_button_clicked();break;
    case Qt::Key_L: ui->tabWidget->setCurrentWidget(ui->tabWidget->currentIndex() ? ui->live_image:ui->tab2); break;
    case Qt::Key_Space:on_pc_pulse_button_clicked();break;
    case Qt::Key_Escape: on_actionExit_triggered();break;
    default: ;
    }
    QMainWindow::keyPressEvent(e);
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
    }
}

//View
//Calibration view for 3axis pipette

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

//update main graphics scene
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

//right click actions

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

//menus

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
    QCoreApplication::quit();
}

void MainWindow::on_actionHW_selector_triggered()
{
    this->close_and_return_hw();
    this->deleteLater();
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

// buttons

void MainWindow::on_predict_sph_clicked()
{
   // m_predict_thread = new std::thread(&MainWindow::predict_sph,this);
    this->predict_sph();
}

void MainWindow::on_Campushbtn_clicked()
{
    cameracv = new CameraCV(cameraIndex);
    if (Iscameraopen==true){
        ui->Campushbtn->setText("Camera on");
        delete cameracv;
        cameracv= nullptr;
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
    std::vector<float> current_coors = ctrl->pipette_get_coordinates();
    ui->xc_label->setText("X: " + QString::number(static_cast<double>(current_coors.at(0)),'f',2));
    ui->yc_label->setText("Y: " + QString::number(static_cast<double>(current_coors.at(1)),'f',2));
    ui->zc_label->setText("Z: " + QString::number(static_cast<double>(current_coors.at(2)),'f',2));
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

void MainWindow::on_start_screening_clicked()
{

    if(!m_s_t_acitive)
    {
        m_s_t_acitive = true;

        float p_x= static_cast<float>(ui->set_plate_size_spinbox->value());
        float p_y =static_cast<float>(ui->set_plate_size_spinbox_2->value());
        m_screening_thread = new std::thread(&MainWindow::screen_area,this,p_x,p_y);
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
        emit prog_changed( m_progvalue );
    }
}

void MainWindow::set_stich_progressbar( int value ){
    if ( value != m_stich_progvalue ) {
        m_stich_progvalue = value;
        m_stich_prog->setValue(m_stich_progvalue );
        emit stich_prog_changed( m_stich_progvalue );
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

void MainWindow::scan_stopped()
{
    QMessageBox::information(this,"Scanning done","Scanning of the plate finished,pipette and stage controller unlocked" );
    m_s_t_acitive=false;
   // this->m_screening_thread->join();
}

void MainWindow::set_pip_man(int value)
{
    if(value == 0)
    {
        ui->p_home_y->hide();
        ui->p_ymax->hide();
        ui->p_ym_button->hide();
        ui->p_yp_button->hide();
    }
    if (value == 1)
    {
        ui->p_home_y->show();
        ui->p_ymax->show();
        ui->p_ym_button->show();
        ui->p_yp_button->show();
    }
}

void MainWindow::on_p_set_speed_spinbox_valueChanged(int arg1)
{
     QTextStream(stdout)<< "speed changed to: " << arg1 ;
     ctrl->pipette_set_speed(arg1);
}

void MainWindow::on_s_speed_spinbox_valueChanged(int arg1)
{
     ctrl->stage_set_speed(static_cast<int>(arg1));
}

void MainWindow::on_pickup_sph_clicked()
{
   m_picking_thread = new std::thread(&MainWindow::xz_stage_pickup_sph,this);
}

void MainWindow::on_view_scan_clicked()
{
   delete m_stich_prog;
   m_stich_prog=new QProgressDialog("Stiching", nullptr, 0, 0);
   m_stich_prog->setRange(0,100);
   m_stich_prog->setWindowModality(Qt::WindowModal);
   m_stich_prog->setMinimumDuration(0);
   m_stich_prog->setValue(0);
   m_stich_prog->setAutoClose(true);
   m_create_mosaic_thread = new std::thread(&MainWindow::create_mosaic,this);
}

void MainWindow::on_p_ep_button_clicked()
{
    ctrl->pipette_extrude_relative(static_cast<float>(ui->p_extruder_step_box->value()));
}

void MainWindow::on_p_em_button_clicked()
{
    ctrl->pipette_extrude_relative(static_cast<float>(-(ui->p_extruder_step_box->value())));
}

void MainWindow::on_s_accel_spinbox_valueChanged(int arg1)
{
    ctrl->stage_set_acceleration(arg1);
}

/*!
 * XZ CALIBRATION STEP 2, user should move X to the middle of
 * IMAGE and click this button
 * this button saves X positon of the pipette.
 * auto pick up will send back to this pos (should be middle of the image)
 */

void MainWindow::on_save_m_p_button_clicked()
{

    std::vector<float> pos = ctrl->pipette_get_coordinates();
    this->mid_s_x_p = pos.at(0);
}

void MainWindow::on_petri_b_clicked()
{
    m_move_petri_b_thread = new std::thread(&MainWindow::move_to_petri_B,this);
}

void MainWindow::on_pick_and_put_clicked()
{
    m_put_and_pick_thread = new std::thread (&MainWindow::pick_and_put,this);
}

void MainWindow::on_found_objects_highlighted(int index)
{
    m_center_selected_sph_thread = new std::thread (&MainWindow::center_selected_sph,this,index);
}

void MainWindow::on_magnification_currentIndexChanged(int index)
{

}

void MainWindow::on_magnification_highlighted(int index)
{

}

void MainWindow::on_magnification_activated(int index)
{
    switch (index)
    {
    case 0 : m_img_width=IMG_W_M0p61X; m_img_height=IMG_H_M0p61X;break;
    case 1 : m_img_width=IMG_W_M1X; m_img_height=IMG_H_M1X;break;
    case 2 : m_img_width=IMG_W_M2X; m_img_height=IMG_H_M2X;break;
    case 3 : m_img_width=IMG_W_M3X; m_img_height=IMG_H_M3X;break;
    case 4 : m_img_width=IMG_W_M4X; m_img_height=IMG_H_M4X;break;
    case 5 : m_img_width=IMG_W_M5X; m_img_height= IMG_H_M5X;break;
    case 6 : m_img_width=IMG_W_M5p5x; m_img_height=IMG_H_M5p5x;break;
    }
}

//Auto pickup,scanning methods and helpers

//Lock the control panels of the ui
void MainWindow::lock_ui()
{
    ui->pip_c_box->setDisabled(true);
    ui->S_c_box->setDisabled(true);
    ui->syringe_box->setDisabled(true);
}

//Unlock the control panels of the ui
void MainWindow::unlock_ui()
{
    ui->pip_c_box->setEnabled(true);
    ui->S_c_box->setEnabled(true);
    ui->syringe_box->setEnabled(true);
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

//calculate a global stage coordinate from image coordinate
//screen center that position
std::vector<float> MainWindow::get_centered_coordinates(std::vector<float>& sph_coors)
{
    std:: vector<float> center_coordinates;
    // const float  img_w_5p5x = 27426; //    100nm
    //  const float  img_h_5p5x = 15421;
    const float x_conv =  m_img_width/FULL_HD_IMAGE_WIDTH;
    const float y_conv = m_img_height/FULL_HD_IMAGE_HEIGHT;
    float mid_obj_pxl_x =  960 - sph_coors.at(0);
    float mid_obj_pxl_y =  540 - sph_coors.at(1);
    float obj_pos_um_x = ( mid_obj_pxl_x *x_conv);
    float obj_pos_um_y = (  mid_obj_pxl_y *y_conv);
    center_coordinates.push_back(ctrl->stage_get_x_coords()-(obj_pos_um_x));
    center_coordinates.push_back(ctrl->stage_get_y_coords()-(obj_pos_um_y));
    return center_coordinates;
}

//Center a spheroid with a given coordinates
void MainWindow::center_spheroid(std::vector<float>& coors)
{
    ctrl->stage_set_speed(3000);
    std:: vector<float> c_coords = this->get_centered_coordinates(coors);
    ctrl->stage_move_to_x_sync(static_cast<int>(c_coords.at(0)));
    ctrl->stage_move_to_y_sync(static_cast<int>(c_coords.at(1)));
    ctrl->stage_set_speed(ui->s_speed_spinbox->value());
}

void MainWindow::center_selected_sph(int index)
{
    //set Picking speeds
    ctrl->stage_set_speed(3000);
    //slowly center the selected sph
    //QTextStream (stdout) <<"global coors"<<global_obj_im_coordinates->at(ui->found_objects->currentIndex()).at(0) <<":" <<global_obj_im_coordinates->at(index).at(1)<< endl;
    ctrl->stage_move_to_x_sync(static_cast<int>(global_obj_im_coordinates->at(index).at(0)));
    ctrl->stage_move_to_y_sync(static_cast<int>(global_obj_im_coordinates->at(index).at(1)));
    ctrl->stage_set_speed(ui->s_speed_spinbox->value());
}
/*!
  *MOVE to the petri "B" 35mm petri
  *if the Petri "A" is centered MID (stage 751431,501665)
  */

void MainWindow::move_to_petri_B()
{
    ctrl->stage_move_to_x_sync(STAGE_CENTER_X-366407);
    ctrl->stage_move_to_y_sync(STAGE_CENTER_Y);
    ctrl->pipette_move_to_z_sync(static_cast<float>(ui->set_z_spinbox->value()+0.3));
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    ctrl->pipette_extrude_relative(static_cast<float>(ui->p_extruder_step_box->value()));
    ctrl->pipette_home_z();
}

void MainWindow::xz_stage_pickup_sph(){

    auto start = std::chrono::system_clock::now();
    //set Pickup speeds
    int original_stage_speed = ctrl->stage_get_x_speed();
    ctrl->stage_set_speed(3000);
    ctrl->pipette_set_speed(ui->z_dive_speed->value());
    //slowly center the selected sph
    ctrl->stage_move_to_x_sync(static_cast<int>(global_obj_im_coordinates->at(ui->found_objects->currentIndex()).at(0)));
    ctrl->stage_move_to_y_sync(static_cast<int>(global_obj_im_coordinates->at(ui->found_objects->currentIndex()).at(1)));
    //
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //move to the middle of image with the pipette
    ctrl->pipette_move_to_x_sync(this->mid_s_x_p);
    //move Z axis down to the petri (auto Z height)
    ctrl->pipette_move_to_z_sync(static_cast<float>(ui->set_z_spinbox->value()));
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // PICK UP WITH SYRINGE
    ctrl->pipette_extrude_relative(-static_cast<float>(ui->p_extruder_step_box->value()));
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ctrl->pipette_home_z();
    // MOVE x axis out of image

    ctrl->pipette_movex_sync(-2.6f);
    ctrl->stage_set_speed(original_stage_speed);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    //TODO
    // this point CHECK if the spheroid picked up!

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

        // CHRONO END
    std::cout << "[PICK UP]elapsed time: " << elapsed_seconds.count() << "s\n";

}

void MainWindow::pick_and_put()
{
    //MOVE to the petri "B" 35mm petri
    //if the Petri "A" is centered MID (stage 751431,501665)
    // CENTER 751431 501665 after auto calibration
    this->xz_stage_pickup_sph();
    this->move_to_petri_B();
}

void MainWindow::predict_sph(){
    auto cfrm = cameracv->get_current_frm();

    if (global_obj_im_coordinates != nullptr)
    {
        delete global_obj_im_coordinates;
        global_obj_im_coordinates = nullptr;
    }

    global_obj_im_coordinates = new std::vector<std::vector<float>>;
    cv::Mat displayfrm = imtools->convert_bgr_to_rgb(cfrm);
    std::vector<std::vector<float>> im_obj = dl->dnn_inference(displayfrm,displayfrm);
    ui->found_objects->clear();

    for (int idx = 0; idx < im_obj.size(); ++idx)
    {
       global_obj_im_coordinates->push_back(this->get_centered_coordinates(im_obj.at(idx)));
       ui->found_objects->addItem(QString::number(idx)+" L:" + QString::number(im_obj.at(idx).at(2),'f',1)
                                                      +" A:" + QString::number(im_obj.at(idx).at(3),'f',1)
                                                      +" C:" + QString::number(im_obj.at(idx).at(4),'f',3));
    }

    delete qfrm_t2;
    qfrm_t2 = new QImage(const_cast< unsigned char*>(displayfrm.data),displayfrm.cols,displayfrm.rows, QImage::Format_RGB888);
    im_view_pxmi.setPixmap( QPixmap::fromImage(*qfrm_t2));
    ui->graphicsView_2->fitInView(&im_view_pxmi, Qt::KeepAspectRatio);
    ui->tabWidget->setCurrentWidget(ui->tab2);

}

std::string MainWindow::get_date_time_str()
{
    std::string datetime= "";
    auto current_time = QTime::currentTime();
    auto current_date = QDate::currentDate();
    datetime.append((QString::number(current_date.year())).toStdString());
    datetime.append((QString::number(current_date.month())).toStdString());
    datetime.append((QString::number(current_date.day())).toStdString()+"-");
    datetime.append((QString::number(current_time.hour())).toStdString());
    datetime.append((QString::number(current_time.minute())).toStdString());
    return datetime;
}

/*!
 *screening a given rectangle area at 5X magnification
 *predicting every scan
 *calculating every spheroids coordinates to make it middle of screen
 *saving every image, to scanning folder with date, time, size stamp
 */

void MainWindow::screen_area(float plate_w_mm,float plate_h_mm)
{
    using namespace  cv;
    auto start = std::chrono::system_clock::now();

    //LOCKING CONTROLLER
    this->lock_ui();
    //TODO
    //MOVE the pipette OUT
    //const float platesize_x= ui->set_plate_size_spinbox->value()*10000;
    //const float platesize_y = ui->set_plate_size_spinbox_2->value()*10000;
    const float platesize_x= plate_w_mm*C_MM_TO_UMx0p1;
    const float platesize_y = plate_h_mm*C_MM_TO_UMx0p1;
    int object_counter = 0;
    std::string folder = "Scandata_";
    std::string datetime = "_";
    std::stringstream ss;
    ss << (ui->set_plate_size_spinbox->value());
    std::string plate_size_mm = ss.str();
    folder.append(plate_size_mm).append("mm_");
    folder.append(get_date_time_str()+"/");
    if (QDir().exists(folder.c_str()))
    {
        QTextStream(stdout) << "this folder already folder exists"<< endl;
    }
    else
    {
        QTextStream(stdout)<< folder.c_str()<< "created" << endl;
        QDir().mkdir(folder.c_str());
    }
    QTextStream(stdout)<< "starting..";

    const int xpos=ctrl->stage_get_x_coords();
    const int ypos=ctrl->stage_get_y_coords();
    const int wmax = static_cast<int>(platesize_x/m_img_width); // um
    const int hmax = static_cast<int>(platesize_y/m_img_height); // um
    QTextStream(stdout)<< "wmax: "<<wmax << " hmax" << hmax;
    int counter = 0;
    if(scanvector.size()>0){scanvector.clear();}
    float p_v=0.0f;
    folder.append("s_"+datetime);
    if (global_obj_im_coordinates != nullptr){delete global_obj_im_coordinates;}
    global_obj_im_coordinates = new std::vector<std::vector<float>>;
    for (int j = 0; j < hmax; ++j)
    {
        ctrl->stage_set_speed(7000.0f);
        ctrl->stage_move_to_y_sync(static_cast<int>(ypos+m_img_height*j));
        ctrl->stage_set_speed(5000.0f);
        for (int  i = 0; i< wmax; ++i)
        {
            if(m_s_t_acitive)
            {
                ctrl->stage_move_to_x_sync(static_cast<int>(xpos+m_img_width*i));
                p_v= (static_cast<float>((wmax)*j+i)/static_cast<float>((hmax)*(wmax)))*100;
                QTextStream(stdout)<<"progvalue"<< p_v<< endl;
                prog_changed(static_cast<int>(p_v));
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                int leading = 2;
                std::string num2str= folder + "_H" + std::to_string(j*0.000001).substr(8-leading)+ "_W" + std::to_string(i*0.000001).substr(8-leading);
                std::string posy = std::to_string(j+1)+ "/" + std::to_string(hmax);
                std::string posx = std::to_string(i+1)+ "/" + std::to_string(wmax);
                ui->current_scaningpos->setText(("Scaning pos: W: "+posx +" H: " + posy).c_str() );
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                auto tmp = cameracv->get_current_frm();
                cv::Mat displayfrm = imtools->convert_bgr_to_rgb(tmp);
                scanvector.push_back(displayfrm);
                imtools->saveImg(tmp.get(),num2str.c_str());

                // INFERENCE AND FEATURE EXTRACTION
                std::vector<std::vector<float>>im_objects = dl->dnn_inference(scanvector.at(counter),scanvector.at(counter));
                for (int k =0; k<im_objects.size();++k)
                {
                   global_obj_im_coordinates->push_back( this->get_centered_coordinates(im_objects.at(k)));
                   for (int idx = 2;idx<7;++idx)
                   {
                       global_obj_im_coordinates->at(object_counter).push_back(im_objects.at(k).at(idx));
                   }
              //     std::cout <<"sizeofimageobjat"<< im_objects.at(object_counter).size() << std::endl;
                   // put text TEXT
             //      int baseLine ;
            //       std::string label_txt = std::to_string(object_counter);//,conf: <" + std::to_string(score) + ">";
           //        cv::Size labelSize = getTextSize(label_txt, cv::FONT_HERSHEY_SIMPLEX, 1,2, &baseLine);
          //         rectangle(scanvector.at(counter), cv::Point(im_objects.at(object_counter).at(5), im_objects.at(object_counter).at(6) - round(labelSize.height)), cv::Point(im_objects.at(object_counter).at(5) + round(labelSize.width), im_objects.at(object_counter).at(6) + baseLine), cv::Scalar(255, 255, 255), cv::FILLED);
         //          putText(scanvector.at(counter), label_txt,cv::Point(im_objects.at(object_counter).at(5), im_objects.at(object_counter).at(6)),cv::FONT_HERSHEY_SIMPLEX, 3, cv::Scalar(0,0,0),1);
                   object_counter++;
                }
                counter++;
            }else{
               // scanvector.clear();
                return;
            }
        }

    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

        // CHRONO END
    std::cout << "[SCANNING] elapsed time: " << elapsed_seconds.count() << "s\n";
    prog_changed(100);
    m_s_t_acitive=false;
   // ctrl->stage_set_speed(20000.0f);
    ui->found_objects->clear();
    for (int i=0 ;i<global_obj_im_coordinates->size();++i )
    {
       ui->found_objects->addItem(QString::number(i)+" L:" + QString::number(global_obj_im_coordinates->at(i).at(2),'f',1)+" A:" + QString::number(global_obj_im_coordinates->at(i).at(3),'f',1)+ " C:" + QString::number(global_obj_im_coordinates->at(i).at(4),'f',3));
    }

    this->unlock_ui();
    ui->start_screening->setText("Start Screening");
    scan_finished();
}

/*!
 * creating a stiched image sequence of analyzed images
 */

void MainWindow::create_mosaic(){
    using namespace  cv;
    if (scanvector.size()>0 && !m_s_t_acitive)
    {
        const float platesize_x = ui->set_plate_size_spinbox->value()*C_MM_TO_UMx0p1; // convert mm to STAGE 0.1um unit
        const float platesize_y = ui->set_plate_size_spinbox_2->value()*C_MM_TO_UMx0p1; // convert mm to STAGE 0.1um unit
        int wmax = static_cast<int>(platesize_x / m_img_width);
        int hmax = static_cast<int>(platesize_y / m_img_height);
        QTextStream(stdout) << m_img_width <<":"<<wmax<<" : "<<platesize_x<< endl;
        if(Mimage != nullptr){delete Mimage;Mimage= nullptr;}
        Mimage = new cv::Mat(cv::Mat::zeros((hmax * FULL_HD_IMAGE_HEIGHT), (wmax * FULL_HD_IMAGE_WIDTH), CV_8UC3));

        float p_v=0.0f;
        for (int i = 0; i < hmax; ++i)
        {
            for (int j = 0; j < wmax; ++j)
            {
                QTextStream(stdout) << i <<" : "<<j<< endl;
                p_v= (static_cast<float>((wmax)*i+j)/static_cast<float>((hmax)*(wmax)))*100;
              //  QTextStream(stdout)<<"progvalue"<< p_v<< endl;
                stich_prog_changed(static_cast<int>(p_v));
                for (int ii = 0; ii < scanvector.at(i*wmax + j).cols; ++ii)
                {
                    for (int jj = 0; jj < scanvector.at(i*wmax + j).rows; ++jj)
                    {
                        Mimage->at<Vec3b>((jj + (i * FULL_HD_IMAGE_HEIGHT)), (ii + (j * FULL_HD_IMAGE_WIDTH)))[0] = scanvector.at(i*wmax + j).at<Vec3b>(jj, ii)[0];
                        Mimage->at<Vec3b>((jj + (i * FULL_HD_IMAGE_HEIGHT)), (ii + (j * FULL_HD_IMAGE_WIDTH)))[1] = scanvector.at(i*wmax + j).at<Vec3b>(jj, ii)[1];
                        Mimage->at<Vec3b>((jj + (i * FULL_HD_IMAGE_HEIGHT)), (ii + (j * FULL_HD_IMAGE_WIDTH)))[2] = scanvector.at(i*wmax + j).at<Vec3b>(jj, ii)[2];
                    }
                }
            }
        }

        m_stich_prog->setLabelText("Done");
        m_stich_prog->setValue(100);
        m_stich_prog = nullptr;
        // CV cell label txt
        imtools->saveImg(Mimage,"mozaic");
        scanvector.clear();
        stiched_img_rdy();
    }

}

void MainWindow::show_on_view_2()
{
    delete qfrm_t2;
    qfrm_t2 = new QImage(const_cast< unsigned char*>(Mimage->data),Mimage->cols,Mimage->rows, QImage::Format_RGB888);
    im_view_pxmi.setPixmap( QPixmap::fromImage(*qfrm_t2) );

    //ui->graphicsView_2->fitInView(&im_view_pxmi, Qt::KeepAspectRatioByExpanding);
    ui->tabWidget->setCurrentWidget(ui->tab2);
}

void MainWindow::on_move_to_s_plate_clicked()
{
    int s_x = STAGE_FIRST_WELL_LEFT_X+ui->s_well_x_combobox->currentIndex()*DIA_96_WELLPLATE;
    int s_y = STAGE_FIRST_WELL_TOP_Y+(ui->s_well_y_spinbox->value()-1)*DIA_96_WELLPLATE;
    ctrl->stage_move_to_x_async(s_x);
    ctrl->stage_move_to_y_sync(s_y);
}

void MainWindow::on_s_getmin_clicked()
{
   int x = ctrl->stage_get_x_min_pos();
   int y = ctrl->stage_get_y_min_pos();
   QTextStream(stdout)<<"stage min x,y (" <<x << " , " <<y <<")"<<endl;
}
