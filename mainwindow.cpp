#include <QStyleFactory>
#include <QMouseEvent>
#include <QStyle>
#include <QMessageBox>
#include <QLabel>
#include <QApplication>
#include <QtXml>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <marcros.h>
#include <serialcom.h>
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
    ui->Camera_settings->hide();
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

    sph_s = new spheroid_selector;
    this->start_camera();
    sph_s->set_bbs(m_bboxes);
    p_s = new Plateselector;
    bool ispipetteconnected = ctrl->connect_pipette_controller(propreader->cfg.port_pipette);

    try {
       bool ispipetteconnected = ctrl->connect_pipette_controller(propreader->cfg.port_pipette);
    }
    catch (...) {
        std::cerr << "pipette controller failed to connect";
    }

    ctrl->connect_tango_stage();
    progress.setValue(33);
    progress.setLabelText("homing manipulator");
    ctrl->pipette_home_z();
    ctrl->pipette_home_x();
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    progress.setValue(50);
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Automatic Spheroid Picker",
                                                                tr("Do you want to run stage inicialization?\n"),
                                                                QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes) {
            progress.setLabelText("Stage inicialization, may take about 1 minute");
            ctrl->stage_set_speed(30000);
            ctrl->stage_autocalibrate();
    }
    ctrl->stage_set_speed(ui->s_speed_spinbox->value());
    progress.setLabelText("Loading neural network");
    progress.setValue(70);
    /*
    QMessageBox::StandardButton res1Btn = QMessageBox::question( this, "Automatic Spheroid Picker",
                                                              tr("Load Matterport Rcnn?"),
                                                                QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (res1Btn == QMessageBox::Yes) {
        dl = new matterport_mrcnn();
        dl->setup_dnn_network(propreader->cfg.matterport_graph.c_str(),
                              propreader->cfg.matterport_folder.c_str(),
                              nullptr);
       cv::Mat test = cv::imread("BIOMAGwhite-01.png", cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
        // start a fake inference for finish preload
       cv::Mat mask;
       std::vector<cv::Mat> bbs;
       //std::thread t_inf(&i_deeplearning::dnn_inference,dl,test,test,test,m_bboxes);
       //t_inf.detach();
       QTextStream(stdout) << QDir::currentPath();
       dl->dnn_inference(test,test,test,m_bboxes);
       QTextStream(stdout) << QDir::currentPath();
    }else{
        dl = new invecption_v2();
        dl->setup_dnn_network(propreader->cfg.classesFile.c_str(),
                          propreader->cfg.model_weights.c_str(),
                          propreader->cfg.textGraph.c_str());
    }*/
    dl = new matterport_mrcnn();
    dl->setup_dnn_network(propreader->cfg.matterport_graph.c_str(),
                          propreader->cfg.matterport_folder.c_str(),
                          nullptr);
   cv::Mat test = cv::imread("BIOMAGwhite-01.png", cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
    // start a fake inference for finish preload
   cv::Mat mask;
   std::vector<cv::Mat> bbs;
   //std::thread t_inf(&i_deeplearning::dnn_inference,dl,test,test,test,m_bboxes);
   //t_inf.detach();
   dl->dnn_inference(test,test,test,m_bboxes);


   //}
    QTextStream(stdout) << QDir::currentPath();


    p_s = new Plateselector;
    connect(p_s,SIGNAL(signal_s_p_changed()),this, SLOT(s_p_changed()));
    connect(p_s,SIGNAL(signal_t_p_changed()),this, SLOT(t_p_changed()));
    automethods = new auto_methods(ctrl,cameracv,dl);
    // USER interface connections
    connect(this, SIGNAL(scan_finished()),this,SLOT(scan_stopped()));
    connect(this, SIGNAL(stiched_img_rdy()),this,SLOT(show_on_view_2()));
    connect(this, SIGNAL(prog_changed(int)),
        this, SLOT(set_progressbar(int)),Qt::QueuedConnection);
    connect(this, SIGNAL(stich_prog_changed(int)),
        this, SLOT(set_stich_progressbar(int)),Qt::QueuedConnection);
    connect(ctrl,SIGNAL(signal_process_qui()),this,SLOT(slot_process_qui()),Qt::QueuedConnection);
    emit p_s->signal_s_p_changed();
    emit p_s->signal_t_p_changed();
    emit ui->magnification->activated(6);
    ui->statusBar->addWidget(ui->stage_pos_text);
    ui->statusBar->addWidget(ui->s_xpos_label);
    ui->statusBar->addWidget(ui->s_ypos_label);
    ui->statusBar->addWidget(ui->p_pos_text);
    ui->statusBar->addWidget(ui->xc_label);
    ui->statusBar->addWidget(ui->yc_label);
    ui->statusBar->addWidget(ui->zc_label);

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
  // qApp->setStyle(QPalette::Base)

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
                QTextStream(stdout) <<"coors x: " << point_mouse->x() << "    y: " << point_mouse->y()<< "\n";
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
    case Qt::Key_Tab: ui->tabWidget->setCurrentWidget(ui->tabWidget->currentIndex() ? ui->live_image:ui->tab2); break;
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
          //      QTextStream(stdout ) << "point 1 saved:" << cpos1->at(0) <<" "<<cpos1->at(1)<< " "<< cpos1->at(2) <<"\n";
            }

        }
        if (calib->a.value()==2){
            imtools->addPointToImage(disp,Point(disp.cols-100,disp.rows-100));
            if(cpos2== nullptr)
            {
            //    cpos2 = new std::vector<float>;
            //    *cpos2 = ctrl->pipette_get_coordinates();
            //     QTextStream(stdout ) << "point 2 saved: " << cpos2->at(0) <<"y: "<<cpos2->at(1)<< "z: "<< cpos2->at(2) <<"\n";
            }
        }
        if (calib->a.value()==3)
        {
            if(cpos3 == nullptr)
            {
            //    cpos3 = new std::vector<float>;
           //     *cpos3 = ctrl->pipette_get_coordinates();
           //     QTextStream(stdout ) << "point 3 saved: x: " << cpos3->at(0) <<"y: "<<cpos3->at(1)<< "z: "<< cpos3->at(2) <<"\n";
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

    auto cfrm =  cameracv->get_current_frm();
    if(cfrm == nullptr)
    {
        return;
    }
    if(cfrm->empty())
    {
        timer->stop();
        disconnect(timer, SIGNAL(timeout()), this, SLOT(update_window()));
        cameracv->stopCameraLoop();
        cameracv->resetvideodevice();
        cameracv->rmvideodevice();
        //cameracv->~CameraCV();
    }else{
        cv::Mat displayfrm = imtools->convert_bgr_to_rgb(cfrm);
        calib_frame_view(displayfrm);
        delete qframe;
        qframe = new QImage(const_cast< unsigned char*>(displayfrm.data),displayfrm.cols, displayfrm.rows, QImage::Format_RGB888);
        //QPainter p(qframe );
        qpxmi.setPixmap( QPixmap::fromImage(*qframe) );
        ui->graphicsView->fitInView(&qpxmi, Qt::KeepAspectRatio);
    }
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
    //ctrl->stage_set_speed(5000.0f);
    mouse.push_back(point_mouse->x()*1.5f);
    mouse.push_back(point_mouse->y()*1.5f);
  //  this->center_spheroid(mouse);
    auto res = this->get_centered_coordinates(mouse);
    QTextStream(stdout) << res.at(0) << res.at(1) ;
    std::vector<std::vector<float>> objpos;

    for (int idx = 0; idx <= 6; ++idx)
    {
       res.push_back(0);
    }
    objpos.push_back(res);
    if (global_obj_im_coordinates == nullptr){
        global_obj_im_coordinates = new std::vector<std::vector<float>>;
    }
    int n  = global_obj_im_coordinates->size();
    QTextStream(stdout)<< n<< ": number of objects";


        //    global_obj_im_coordinates->push_back(this->get_centered_coordinates(im_obj.at(idx)));
    global_obj_im_coordinates->push_back(objpos.at(0));
    ui->found_objects->addItem(QString::number(n)+"manual item");
    ctrl->stage_set_speed(15000.0f);
}

void MainWindow::on_graphicsView_customContextMenuRequested(const QPoint &pos)
{
    QMenu contextMenu(("Context menu"), ui->graphicsView->viewport());
    // QAction action1("Move Pipette Here", ui->graphicsView->viewport());
    //connect(&action1, SIGNAL(triggered()), this, SLOT(move_action()));
    QAction action2("Center this postion", ui->graphicsView->viewport());
    connect(&action2, SIGNAL(triggered()), this, SLOT(center_this_point_action()));
    QAction action3("Pick up", ui->graphicsView->viewport());
    QAction action4("Deploy", ui->graphicsView->viewport());
    //contextMenu.addAction(&action1);
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


void MainWindow::export_csv(){
    if (global_obj_im_coordinates != nullptr)
    {
        std::ofstream csvfile;
        std::string fname = get_date_time_str();
        QTextStream(stdout)<< QDir::currentPath() << "\n";
        csvfile.open("./"+fname+".csv");
        csvfile << "This is the first cell in the first column.\n";
        csvfile << "bbx0," << "bby0,"<< "bbx1,"<<"bby1,"
               <<"length,"<< "area,"<<"circularity,"
               <<"masscenterx,"<<"masscentery\n";
        for (int idx = 0; idx < global_obj_im_coordinates->size(); ++idx)
        {
            for(int j = 0; j < global_obj_im_coordinates->at(idx).size();++j)
            {
                csvfile << global_obj_im_coordinates->at(idx).at(j) << ",";

            }
             csvfile <<  "\n";
        }
        csvfile.close();
    }
    else
    {
    QTextStream(stdout)<< "no object data to save";

    }
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
    sph_s->clear_list();
    this->predict_sph();
}


void MainWindow::start_camera()
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
        QTextStream(stdout) << "camera1";
        connect(timer, SIGNAL(timeout()), this, SLOT(update_window()));
        timer->start(16);
    }
}

void MainWindow::on_Campushbtn_clicked()
{
    this->start_camera();
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
                     (ui->imagename_lineedit->text().toStdString().append(this->get_date_time_str())));

}

void MainWindow::on_s_get_coors_pushButton_clicked()
{
    std::vector<int> coords = ctrl->stage_get_coords();
    ui->s_xpos_label->setText("X: " + QString::number(coords.at(0),'f',2));
    ui->s_ypos_label->setText("Y: " + QString::number(coords.at(1),'f',2));
}


void MainWindow::on_changeplate_button_clicked()
{
    m_stage_plate_thread= new std::thread(&MainWindow::change_plate,this);
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

void MainWindow::slot_process_qui(){
    QTextStream(stdout)<< "slot called";
    qApp->processEvents();
}

void MainWindow::set_pip_man(int value)
{
    if(value == 0)
    {
        ui->p_home_y->hide();
        //ui->p_ymax->hide();
        ui->p_ym_button->hide();
        ui->p_yp_button->hide();
        //ui->p_zmax->hide();
    }
    if (value == 1)
    {
        ui->p_home_y->show();
        //ui->p_ymax->show();
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
    ctrl->pipette_extrude_relative(static_cast<float>(ui->p_extruder_step_box->value()),static_cast<int>(ui->extrude_set_speed_spinbox->value()));
}

void MainWindow::on_p_em_button_clicked()
{
    ctrl->pipette_extrude_relative(static_cast<float>(-(ui->p_extruder_step_box->value())),static_cast<int>(ui->extrude_set_speed_spinbox->value()));
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

    ui->x_midpoint_ui->setText(std::to_string(this->mid_s_x_p).c_str());
}

void MainWindow::on_petri_b_clicked()
{
    //m_move_petri_b_thread = new std::thread(&MainWindow::move_to_petri_B,this);

    this->put_to_target_plate(ui->t_well_x_combobox->currentIndex(),(ui->t_well_y_spinbox->value()));//y-1

}

void MainWindow::on_pick_and_put_clicked()
{
    if (check_for_pipette_height(62,100)){
        //if (m_put_and_pick_thread->joinable()){
        //    m_put_and_pick_thread->join();
        //}

       // m_put_and_pick_thread = new std::thread (&MainWindow::pick_and_put,this);
        pick_and_put();

    }

}

void MainWindow::on_found_objects_highlighted(int index)
{

}

void MainWindow::on_found_objects_activated(int index)
{
    m_center_selected_sph_thread = new std::thread (&MainWindow::center_selected_sph,this,index);
}


void MainWindow::on_pickup_sph_clicked()
{
   //
   bool yes = 0;
   if (check_for_pipette_height(62,100)){

        //m_picking_thread = new std::thread(&MainWindow::xz_stage_pickup_sph,this,ui->found_objects->currentIndex());
        xz_stage_pickup_sph(ui->found_objects->currentIndex());

        //if (m_picking_thread->joinable()){
        //    m_picking_thread->join();
        //    QTextStream(stdout) << "joining picking thread";
        //}
   }



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
    QTextStream(stdout) << "pciking up" << "\n";
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

    ctrl->stage_set_speed(50000);// akos changed the speed
//    ctrl->stage_move_to_x_sync(STAGE_CENTER_X-366407);
//    ctrl->stage_move_to_y_sync(STAGE_CENTER_Y);
    move_to_t_plate(ui->t_well_x_combobox->currentIndex(),(ui->t_well_y_spinbox->value()),p_s->m_selected_target);
    //ctrl->stage_move_to_x_sync(877396+27000); //Akos
    //ctrl->stage_move_to_y_sync(1820+18000);  // Akos

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    ctrl->pipette_move_to_x_sync(mid_s_x_p);
    ctrl->pipette_move_to_z_sync(static_cast<float>(ui->set_z_spinbox_2->value()+0.3));

    ctrl->pipette_extrude_relative(static_cast<float>(ui->p_extruder_step_box->value()));
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    ctrl->pipette_move_to_z_sync(static_cast<float>(ui->set_z_spinbox->value()+20.0f)); //Akos Z value
    /* ctrl->stage_move_to_x_sync(STAGE_CENTER_X); // Akos center x
    ctrl->stage_move_to_y_sync(STAGE_CENTER_Y); // Akos center y */
}


/*  OLD function not syncorised

void MainWindow::xz_stage_pickup_sph(int obj_idx){

    auto start = std::chrono::system_clock::now();
    //set Pickup speeds
    //ui->found_objects->currentIndex()
    int original_stage_speed = ctrl->stage_get_x_speed();
    ctrl->stage_set_speed(30000);
    ctrl->pipette_set_speed(ui->z_dive_speed->value());
    //slowly center the selected sph
    ctrl->stage_move_to_x_async(static_cast<int>(global_obj_im_coordinates->at(obj_idx).at(0)));
    ctrl->stage_move_to_y_sync(static_cast<int>(global_obj_im_coordinates->at(obj_idx).at(1)));
    //
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //move to the middle of image with the pipette
    ctrl->pipette_move_to_x_sync(this->mid_s_x_p);
    //move Z axis down to the petri (auto Z height)
    ctrl->pipette_move_to_z_sync(static_cast<float>(ui->set_z_spinbox->value())+0.1f);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // PICK UP WITH SYRINGE
    ctrl->pipette_extrude_relative(-static_cast<float>(ui->p_extruder_step_box->value()));
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    ctrl->pipette_move_to_z_sync(static_cast<float>(ui->set_z_spinbox->value()+20.0f)); //Akos Z value
    // MOVE x axis out of image

    ctrl->pipette_movex_sync(-2.6f);
    ctrl->stage_set_speed(original_stage_speed);

    //TODO
    // this point CHECK if the spheroid picked up!
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
        // CHRONO END
    std::cout << "[PICK UP]elapsed time: " << elapsed_seconds.count() << "s\n";

}

*/


bool MainWindow::check_for_pipette_height(int p_tresh, int p_max_trials)
{

    /*
    checks if height of the pipette is under thresh with max_trial, in 50 ms
    until the pipette is not higher than a coordinate, it returns false
    */
    std::vector<float> coors;
    for (int trial = 0; trial< p_max_trials; trial++)
    {
        QTextStream(stdout)<< "pipette is too low\n" << trial;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        qApp->processEvents();
        if (!(trial%10))
        {
            coors = ctrl->pipette_get_coordinates();
            if (coors.at(2) > p_tresh){
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                return true;
            }
        }
    }
    QTextStream(stdout)<< "the pipette is too low end of trying\n";
    return false;
}


void MainWindow::xz_stage_pickup_sph(int obj_idx){
    if (global_obj_im_coordinates != nullptr){


            auto start = std::chrono::system_clock::now();
            //set Pickup speeds
            //ui->found_objects->currentIndex()

            int original_stage_speed = ctrl->stage_get_x_speed();
            ctrl->stage_set_speed(15000);

            ctrl->pipette_set_speed(ui->z_dive_speed->value());
            //slowly center the selected sph


            QTextStream(stdout)<< "stage move x a";
            ctrl->stage_move_to_x_async(static_cast<int>(global_obj_im_coordinates->at(obj_idx).at(0)));
            QTextStream(stdout)<< "stage move y a";

            ctrl->stage_move_to_y_sync(static_cast<int>(global_obj_im_coordinates->at(obj_idx).at(1)));
            //

            // move to the middle of image with the pipette
            QTextStream(stdout)<< "pip move blocking x";
            ctrl->pipette_blocking_move_x(this->mid_s_x_p);
            //move Z axis down to the petri (auto Z height)

            QTextStream(stdout)<< "pip move blocking z";
            ctrl->pipette_blocking_move_z(static_cast<float>(ui->set_z_spinbox->value())+0.1f);

            // PICK UP WITH SYRINGE
            QTextStream(stdout)<< "pip move blocking e";

            ctrl->pipette_blocking_move_e(-static_cast<float>(ui->p_extruder_step_box->value()));
            qApp->processEvents();
            QTextStream(stdout)<< "pip move blocking z";

            ctrl->pipette_blocking_move_z(static_cast<float>(ui->set_z_spinbox->value()+25.0f)); //Akos Z value

            // MOVE x axis out of image
            QTextStream(stdout)<< "pip move blocking x";

            ctrl->pipette_blocking_move_x(this->mid_s_x_p-2.6f);


            ctrl->stage_set_speed(original_stage_speed);


            //TODO
            // this point CHECK if the spheroid picked up!
            //std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
                // CHRONO END
            std::cout << "[PICK UP]elapsed time: " << elapsed_seconds.count() << "s\n";
       // }else{
        //    QTextStream(stdout)<< "pipette is too low";
        //}

    }else{
        QTextStream(stdout)<< "object highlighted nothing to pick";
    }

}

void MainWindow::put_to_target_plate(int x,int y, int type)
{
    std::vector<float> coors = ctrl->pipette_get_coordinates();

    // do a thread test :D
    ctrl->stage_set_speed(15000);   // akos changed the speed
    QTextStream(stdout)<< "stage move xy";
    move_to_t_plate(x, y , p_s->m_selected_target);
   // std::this_thread::sleep_for(std::chrono::milliseconds(4000));
    QTextStream(stdout)<< "pip move blocking x";
    ctrl->pipette_blocking_move_x(mid_s_x_p);
    QTextStream(stdout)<< "pip move blocking z";
    ctrl->pipette_blocking_move_z(static_cast<float>(ui->set_z_spinbox_2->value()));
    QTextStream(stdout)<< "pip move blocking e";
    ctrl->pipette_blocking_move_e(static_cast<float>(ui->p_extruder_step_box->value())+static_cast<float>(ui->doubleSpinBox_2->value()));
    QTextStream(stdout)<< "pip move blocking z";
    ctrl->pipette_blocking_move_z(static_cast<float>(ui->set_z_spinbox->value()+25.0f)); //Akos Z value

}


void MainWindow::clean_pipette(int x, int y)
{
    // No cleaning in high content
    if (!(p_s->m_selected_target ==  2))
    {
        ctrl->pipette_blocking_move_z(static_cast<float>(ui->set_z_spinbox->value()+25.0f));
        std::vector<float> coors = ctrl->pipette_get_coordinates();

        ctrl->stage_set_speed(15000);   // akos changed the speed
        ctrl->pipette_set_speed(ui->z_dive_speed->value()*2);
        QTextStream(stdout)<< "stage move xy";
        move_to_t_plate(x,y,p_s->m_selected_source);
        // std::this_thread::sleep_for(std::chrono::milliseconds(4000));
        QTextStream(stdout)<< "pip move blocking x";
        ctrl->pipette_blocking_move_x(mid_s_x_p);
        QTextStream(stdout)<< "pip move blocking z";
        ctrl->pipette_blocking_move_z(static_cast<float>(ui->set_z_spinbox_2->value()));
        QTextStream(stdout)<< "pip move blocking e";
        float cleaning_amount = 3.0f;

        ctrl->pipette_blocking_move_e(-cleaning_amount);
        ctrl->pipette_blocking_move_e(cleaning_amount);
        QTextStream(stdout)<< "pip move blocking z";
        ctrl->pipette_blocking_move_z(static_cast<float>(ui->set_z_spinbox->value()+20.0f)); //Akos Z value

    }
    else
    {
        QTextStream(stdout)<< "No pipette cleaning is enabled while high content plate is selected";
    }


}

void MainWindow::two_well_cleaning()
{

    this->clean_pipette(0,1); // H1
    this->clean_pipette(1,1); // G1
    this->clean_pipette(2,1); // G1
}


void MainWindow::pick_and_put()
{
    auto start = std::chrono::system_clock::now();
    //MOVE to the petri "B" 35mm petri
    //if the Petri "A" is centered MID (stage 751431,501665)
    // CENTER 751431 501665 after auto calibration
    if (ui->cleancheckBox->isChecked() == true) {
        two_well_cleaning();
    }
    this->xz_stage_pickup_sph(ui->found_objects->currentIndex());
    this->put_to_target_plate(ui->t_well_x_combobox->currentIndex(),(ui->t_well_y_spinbox->value()));//y-1

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
        // CHRONO END
    std::cout << "[PICK AND PUT]elapsed time: " << elapsed_seconds.count() << "s\n";
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
   // std::vector<cv::Mat> bbs;
    this->m_bboxes.clear();
    cv::Mat mask;
    std::vector<std::vector<float>> im_obj = dl->dnn_inference(displayfrm,displayfrm,mask,m_bboxes);
   // cv::floodFill(mask, cv::Point(0,0),cv::Scalar(255,255,255));
   // cv::bitwise_not(mask,mask);
    imtools->saveImg(&mask,(ui->imagename_lineedit->text()+"_mask").toStdString());

    ui->found_objects->clear();
    delete qfrm_t2;
    qfrm_t2 = new QImage(const_cast< unsigned char*>(displayfrm.data),displayfrm.cols,displayfrm.rows, QImage::Format_RGB888);
    QPainter p(qfrm_t2);
    p.setPen(Qt::black);
    p.setFont(QFont("Arial", 25));
   // m_current_detections = {};
    // struct for storing spheroid detection data;
    for (int idx = 0; idx < im_obj.size(); ++idx)
    {
       QString text(QString::number(idx) );
       global_obj_im_coordinates->push_back(this->get_centered_coordinates(im_obj.at(idx)));
       ui->found_objects->addItem(QString::number(idx)+" L:" + QString::number(im_obj.at(idx).at(4),'f',1)
                                                      +" A:" + QString::number(im_obj.at(idx).at(5),'f',1)
                                                      +" C:" + QString::number(im_obj.at(idx).at(6),'f',3));
        sph_s->set_list(text+". Spheroid"
                    +" Perimeter: " + QString::number(im_obj.at(idx).at(4),'f',1)
                    +" Aera: " + QString::number(im_obj.at(idx).at(5),'f',1)
                    +" Circularity: " + QString::number(im_obj.at(idx).at(6),'f',3));
        const QRect rectangle = QRect(im_obj.at(idx).at(0),im_obj.at(idx).at(1), 100, 100);
        sph_props c = {idx,im_obj.at(idx).at(4),im_obj.at(idx).at(4),im_obj.at(idx).at(4)};
       // std::cout<< "c-d" << c << std::endl;
        // make it pointer!
          m_current_detections.push_back(c);
          std::cout<< "c-d" << m_current_detections.at(idx).idx << std::endl;
          std::cout<< "c-d" << m_current_detections.at(idx).perimeter << std::endl;

        p.drawText(rectangle,Qt::TextSingleLine,text);
    }

    sph_s->list_props();

 //    p.drawText();-

    im_view_pxmi.setPixmap( QPixmap::fromImage(*qfrm_t2));
    ui->graphicsView_2->fitInView(&im_view_pxmi, Qt::KeepAspectRatioByExpanding);
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
    datetime.append((QString::number(current_time.hour())).toStdString()+"h");
    datetime.append((QString::number(current_time.minute())).toStdString()+"m");
    datetime.append((QString::number(current_time.second())).toStdString()+"s");
    return datetime;
}

/*!
 *screening a given rectangle area at the selected magnification
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
    this->m_bboxes.clear();
    //TODO
    //MOVE the pipette OUT
    //const float platesize_x= ui->set_plate_size_spinbox->value()*10000;
    //const float pla2tesize_y = ui->set_plate_size_spinbox_2->value()*10000;
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
        QTextStream(stdout) << "this folder already folder exists"<< "\n";
    }
    else
    {
        QTextStream(stdout)<< folder.c_str()<< "created" << "\n";
        QDir().mkdir(folder.c_str());
    }

    QTextStream(stdout)<< "starting..";
    m_folder = folder;

    const int xpos=ctrl->stage_get_x_coords();
    const int ypos=ctrl->stage_get_y_coords();
    const int wmax = static_cast<int>(platesize_x/m_img_width); // um
    const int hmax = static_cast<int>(platesize_y/m_img_height); // um
    QTextStream(stdout)<< "wmax: "<<wmax << " hmax" << hmax;
    int counter = 0;
    if(scanvector.size()>0){scanvector.clear();}
    float p_v=0.0f;
    //folder.append("s_"+datetime);
    if (global_obj_im_coordinates != nullptr){delete global_obj_im_coordinates;}
    global_obj_im_coordinates = new std::vector<std::vector<float>>;
    int image_posy = 0;
    int image_posx  = 0 ;
    for (int j = 0; j < hmax; ++j)
    {
        ctrl->stage_set_speed(7000.0f);
        if (j == 0){
            image_posy =static_cast<int>(ypos+m_img_height*j);
        }
        else{
           image_posy =static_cast<int>(ypos+m_img_height*j-m_img_height*0.1);
        }

        ctrl->stage_move_to_y_sync(image_posy);
        ctrl->stage_set_speed(5000.0f);
        for (int  i = 0; i< wmax; ++i)
        {
            if(m_s_t_acitive)
            {
                if (i == 0){
                    image_posx = static_cast<int>(xpos+m_img_width*i);
                }else
                {
                    image_posx = static_cast<int>(xpos+m_img_width*i-m_img_width*0.1);
                }
                ctrl->stage_move_to_x_sync(image_posx);

                p_v= (static_cast<float>((wmax)*j+i)/static_cast<float>((hmax)*(wmax)))*100;
                QTextStream(stdout)<<"progvalue"<< p_v<< "\n";
                prog_changed(static_cast<int>(p_v));
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                int leading = 2;
                std::string num2str= folder + "_H" + std::to_string(j*0.000001).substr(8-leading)+ "_W" + std::to_string(i*0.000001).substr(8-leading);
                std::string posy = std::to_string(j+1)+ "/" + std::to_string(hmax);
                std::string posx = std::to_string(i+1)+ "/" + std::to_string(wmax);
                ui->current_scaningpos->setText(("Scaning pos: W: "+posx +" H: " + posy).c_str() );
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                auto tmp = cameracv->get_current_frm();
                cv::Mat displayfrm = imtools->convert_bgr_to_rgb(tmp);
                scanvector.push_back(displayfrm);
                cv::Mat maskfrm;
                std::string image_name = "Image_" + std::to_string(counter*0.000001).substr(8-leading);
                std::string full_path = folder + image_name;
                //imtools->saveImg(tmp.get(),num2str.c_str());
                imtools->saveImg(tmp.get(),full_path.c_str());
                // INFERENCE AND FEATURE EXTRACTION
                std::vector<std::vector<float>>im_objects = dl->dnn_inference(scanvector.at(counter),scanvector.at(counter),maskfrm,m_bboxes);
                for (int k =0; k<im_objects.size();++k)
                {
                   QTextStream(stdout) << im_objects.at(k).at(0) << "cors" << im_objects.at(k).at(1);
                   if (im_objects.at(k).at(0)>1920-192 ||im_objects.at(k).at(1)>1080-108)
                   {
                       QTextStream(stdout) << "dropping this MF " ;
                       continue;
                   }
                   global_obj_im_coordinates->push_back( this->get_centered_coordinates(im_objects.at(k)));
                   for (int idx = 2;idx<=8;++idx)
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
    sph_s->clear_list();
 //   m_current_detections = {};
    for (int i=0 ; i<global_obj_im_coordinates->size(); ++i )
    {
       ui->found_objects->addItem(QString::number(i)+" L:" + QString::number(global_obj_im_coordinates->at(i).at(2),'f',1)+" A:" + QString::number(global_obj_im_coordinates->at(i).at(3),'f',1)+ " C:" + QString::number(global_obj_im_coordinates->at(i).at(4),'f',3));
       sph_s->set_list(QString::number(i) + ". Spheroid"
                   +" Perimeter: " + QString::number(global_obj_im_coordinates->at(i).at(4),'f',1)
                   +" Aera: " + QString::number(global_obj_im_coordinates->at(i).at(5),'f',1)
                   +" Circularity: " + QString::number(global_obj_im_coordinates->at(i).at(6),'f',3));
       //m_current_detections->peri.push_back(global_obj_im_coordinates->at(i).at(4));
       //m_current_detections->area.push_back(global_obj_im_coordinates->at(i).at(5));
       //m_current_detections->circ.push_back(global_obj_im_coordinates->at(i).at(6));

    }

    sph_s->list_props();
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
        QTextStream(stdout) << m_img_width <<":"<<wmax<<" : "<<platesize_x<< "\n";
        if(Mimage != nullptr){delete Mimage;Mimage= nullptr;}
        Mimage = new cv::Mat(cv::Mat::zeros((hmax * FULL_HD_IMAGE_HEIGHT-m_overlap), (wmax * FULL_HD_IMAGE_WIDTH-m_overlap), CV_8UC3));
        float p_v=0.0f;
        for (int i = 0; i < hmax; ++i)
        {
            for (int j = 0; j < wmax; ++j)
            {
                QTextStream(stdout) << i <<" : "<<j<< "\n";
                p_v= (static_cast<float>((wmax)*i+j)/static_cast<float>((hmax)*(wmax)))*100;
              //  QTextStream(stdout)<<"progvalue"<< p_v<< "\n";
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

    ui->graphicsView_2->fitInView(&im_view_pxmi, Qt::KeepAspectRatio);
    ui->tabWidget->setCurrentWidget(ui->tab2);
}



void MainWindow::s_p_changed()
{
    this->s_p_selected = p_s->m_selected_source;
    std::cout << "p_s->m_selected_source" << p_s->m_selected_source<< " checking varriable";
    if(s_p_selected == 0) //96 wp
    {
        QTextStream(stdout) << "96 wp selected";
        ui->s_well_x_combobox->clear();

        for (char c = 'H'; c >= 'A'; --c)
        {
            ui->s_well_x_combobox->addItem(QString(c));
        }
        ui->s_well_y_spinbox->clear();
        ui->s_well_y_spinbox->setRange(1,12);

        ui->s_well_x_combobox->setCurrentIndex(ui->s_well_x_combobox->count()-1);
        ui->s_well_y_spinbox->setValue(1);

    }
    else if(s_p_selected == 1) //384 wp
    {
        QTextStream(stdout) << "384 wp selected";
        QTextStream(stdout) << ui->s_well_x_combobox->count()<< "nitems";
        ui->s_well_x_combobox->clear();
        for (char c = 'P'; c >= 'A'; --c)
        {
            ui->s_well_x_combobox->addItem(QString(c));
        }
        ui->s_well_x_combobox->setCurrentIndex(ui->s_well_x_combobox->count()-1);
        ui->s_well_y_spinbox->clear();
        ui->s_well_y_spinbox->setRange(1,24);
        ui->s_well_y_spinbox->setValue(1);
    }
    else if(s_p_selected == 2) //petri 30mm
    {
        QTextStream(stdout) << "petri 30mm";
        QTextStream(stdout) << ui->s_well_x_combobox->count()<< "nitems";
        ui->s_well_x_combobox->clear();

        ui->s_well_x_combobox->addItem(QString("A"));

        ui->s_well_x_combobox->setCurrentIndex(ui->s_well_x_combobox->count()-1);
        ui->s_well_y_spinbox->clear();
        ui->s_well_y_spinbox->setRange(1,2);
        ui->s_well_y_spinbox->setValue(1);
    }
    else{

    }
}

void MainWindow::t_p_changed()
{
    this->t_p_selected = p_s->m_selected_target;
    std::cout << "target plate value" << p_s->m_selected_target << std::endl;
    if(t_p_selected == 0) //96 wp
    {
        QTextStream(stdout) << "96 wp selected";
        ui->t_well_x_combobox->clear();

        for (char c = 'H'; c >= 'A'; --c)
        {
            ui->t_well_x_combobox->addItem(QString(c));
        }
        ui->t_well_y_spinbox->clear();
        ui->t_well_y_spinbox->setRange(1,12);

        ui->t_well_x_combobox->setCurrentIndex(ui->t_well_x_combobox->count()-1);
        ui->t_well_y_spinbox->setValue(1);

    }
    else if(t_p_selected == 1) //384 wp
    {
        QTextStream(stdout) << "384 wp selected";
        QTextStream(stdout) << ui->t_well_x_combobox->count()<< "nitems";
        ui->t_well_x_combobox->clear();
        for (char c = 'P'; c >= 'A'; --c)
        {
            ui->t_well_x_combobox->addItem(QString(c));
        }
        ui->t_well_x_combobox->setCurrentIndex(ui->t_well_x_combobox->count()-1);
        ui->t_well_y_spinbox->clear();
        ui->t_well_y_spinbox->setRange(1,24);
        ui->t_well_y_spinbox->setValue(1);
    }

    else if(t_p_selected == 2) //HCS
    {
        QTextStream(stdout) << "HCS";
        QTextStream(stdout) << ui->t_well_x_combobox->count()<< "nitems";
        ui->t_well_x_combobox->clear();
        for (char c = 'G'; c >= 'A'; --c)
        {
            ui->t_well_x_combobox->addItem(QString(c));
        }
        ui->t_well_x_combobox->setCurrentIndex(ui->t_well_x_combobox->count()-1);
        ui->t_well_y_spinbox->clear();
        ui->t_well_y_spinbox->setRange(1,8);
        ui->t_well_y_spinbox->setValue(1);
    }
    else if(t_p_selected == 3) //petri 30mm
    {

    }
    else{

    }
}


void MainWindow::move_to_s_plate(int x_idx,int y_idx,int type){
    std::cout<< "moving to target plate given indexes: "<<x_idx <<" : " << y_idx<<std::endl;
    int stage_frist_t_well_left_x = 0;
    int stage_frist_t_well_top_y = 0;
    int dia_well_plate_x = 0;
    int dia_well_plate_y = 0;
    int s_x;
    int s_y;
    QTextStream(stdout)<< "type: in move_to_plate" << type <<  "\n" ;
    switch(type){
    case 0:
        //96 well  plate
        dia_well_plate_x = DIA_96_WELLPLATE;
        dia_well_plate_y = DIA_96_WELLPLATE;
        stage_frist_t_well_left_x = STAGE_FIRST_WELL_LEFT_X+x_idx*DIA_96_WELLPLATE;
        stage_frist_t_well_top_y = STAGE_FIRST_WELL_TOP_Y+(y_idx-1)*DIA_96_WELLPLATE;
        s_x = stage_frist_t_well_left_x+stoi(propreader->cfg.wp_96_offset_X)+x_idx*dia_well_plate_x;
        s_y = stage_frist_t_well_top_y+stoi(propreader->cfg.wp_96_offset_Y)+(y_idx-1)*dia_well_plate_y;
        break;
    case 1:
        // 384 well plate
        dia_well_plate_x = DIA_384_WELLPLATE;
        dia_well_plate_y = DIA_384_WELLPLATE;
        stage_frist_t_well_left_x = STAGE_FIRST_T_WELL_LEFT_X + 13500; // to make it center constans into96
        stage_frist_t_well_top_y = STAGE_FIRST_T_WELL_TOP_Y +  9000; // to make it center constans into96
        s_x = stage_frist_t_well_left_x+stoi(propreader->cfg.wp_96_offset_X)+x_idx*dia_well_plate_x;
        s_y = stage_frist_t_well_top_y+stoi(propreader->cfg.wp_96_offset_Y)+(y_idx-1)*dia_well_plate_y;
        break;

    case 2:
        s_x = 460000;
        s_y = 250000;
        // should be petri
        if (y_idx == 2){
            s_y = 843000;
        }

        break;
    default:
        dia_well_plate_x = DIA_96_WELLPLATE;
        dia_well_plate_y = DIA_96_WELLPLATE;
        stage_frist_t_well_left_x = STAGE_FIRST_T_WELL_LEFT_X;
        stage_frist_t_well_top_y = STAGE_FIRST_T_WELL_TOP_Y;
        s_x = stage_frist_t_well_left_x+stoi(propreader->cfg.wp_96_offset_X)+x_idx*dia_well_plate_x;
        s_y = stage_frist_t_well_top_y+stoi(propreader->cfg.wp_96_offset_Y)+(y_idx-1)*dia_well_plate_y;
    }

    ctrl->stage_move_to_x_async(s_x);
    ctrl->stage_move_to_y_sync(s_y);
}

void MainWindow::on_move_to_s_plate_clicked()
{
    m_move_s_plate_thread = new std::thread(&MainWindow::move_to_s_plate,this,
                                            ui->s_well_x_combobox->currentIndex(),
                                            ui->s_well_y_spinbox->value(),
                                            p_s->m_selected_source);
}

void MainWindow::move_to_t_plate(int x_idx,int y_idx, int type)
{
    std::cout<< "moving to target plate given indexes: "<<x_idx <<" : " << y_idx<<std::endl;
    int stage_frist_t_well_left_x = 0;
    int stage_frist_t_well_top_y = 0;
    int dia_well_plate_x = 0;
    int dia_well_plate_y = 0;
    int s_x;
    int s_y;
    QTextStream(stdout)<< "type: in move_to_plate" << type <<  "\n" ;
    switch(type){
    case 0:
        //96 well  plate
        dia_well_plate_x = DIA_96_WELLPLATE;
        dia_well_plate_y = DIA_96_WELLPLATE;
        stage_frist_t_well_left_x = STAGE_FIRST_T_WELL_LEFT_X + 27000;
        stage_frist_t_well_top_y = STAGE_FIRST_T_WELL_TOP_Y + 18000;
        s_x = stage_frist_t_well_left_x+stoi(propreader->cfg.wp_96_offset_X)+x_idx*dia_well_plate_x;
        s_y = stage_frist_t_well_top_y+stoi(propreader->cfg.wp_96_offset_Y)+(y_idx-1)*dia_well_plate_y;
        break;
    case 1:
        // 384 well plate
        dia_well_plate_x = DIA_384_WELLPLATE;
        dia_well_plate_y = DIA_384_WELLPLATE;
        stage_frist_t_well_left_x = STAGE_FIRST_T_WELL_LEFT_X + 13500; // to make it center constans into96
        stage_frist_t_well_top_y = STAGE_FIRST_T_WELL_TOP_Y +  9000; // to make it center constans into96
        s_x = stage_frist_t_well_left_x+stoi(propreader->cfg.wp_96_offset_X)+x_idx*dia_well_plate_x;
        s_y = stage_frist_t_well_top_y+stoi(propreader->cfg.wp_96_offset_Y)+(y_idx-1)*dia_well_plate_y;
        break;
    case 2:
        // HCS screening plate
        dia_well_plate_x = DIA_96_WELLPLATE;
        //dia_well_plate_y = STAGE_HCS_D_Y;
        stage_frist_t_well_left_x = STAGE_FIRST_3DHCS_T_WELL_C_X;
        stage_frist_t_well_top_y = STAGE_FIRST_3DHCS_T_WELL_C_Y;
        s_y = stage_frist_t_well_top_y;
        if (y_idx == 2){
            s_y = 128300;
        }
        if (y_idx == 3){
            s_y = 282200;
        }
        if (y_idx == 4){
            s_y = 390100;
        }
        if (y_idx == 5){
            s_y = 473800;
        }
        if (y_idx == 6){
            s_y = 582000;
        }
        if (y_idx == 7){
            s_y = 733600;
        }
        if (y_idx == 8){
            s_y = 845600;
        }
        s_x = stage_frist_t_well_left_x+x_idx*dia_well_plate_x;
        QTextStream(stdout)<< "sx" << s_x << "\n sy" << s_y << "\n" ;
        // s_y = stage_frist_t_well_top_y+(y_idx-1)*dia_well_plate_y;

        break;
    case 3:
        // should be petri
        dia_well_plate_x = DIA_96_WELLPLATE;
        dia_well_plate_y = DIA_96_WELLPLATE;
        stage_frist_t_well_left_x = STAGE_FIRST_3DHCS_T_WELL_C_X;
        stage_frist_t_well_top_y = STAGE_FIRST_3DHCS_T_WELL_C_Y;
        if (!((y_idx+1) % 3)){
            stage_frist_t_well_top_y += 82000;
        }
        s_x = stage_frist_t_well_left_x+stoi(propreader->cfg.wp_96_offset_X)+x_idx*dia_well_plate_x;
        s_y = stage_frist_t_well_top_y+stoi(propreader->cfg.wp_96_offset_Y)+(y_idx-1)*dia_well_plate_y;
        break;
    default:
        dia_well_plate_x = DIA_96_WELLPLATE;
        dia_well_plate_y = DIA_96_WELLPLATE;
        stage_frist_t_well_left_x = STAGE_FIRST_T_WELL_LEFT_X;
        stage_frist_t_well_top_y = STAGE_FIRST_T_WELL_TOP_Y;
        s_x = stage_frist_t_well_left_x+stoi(propreader->cfg.wp_96_offset_X)+x_idx*dia_well_plate_x;
        s_y = stage_frist_t_well_top_y+stoi(propreader->cfg.wp_96_offset_Y)+(y_idx-1)*dia_well_plate_y;
    }

    ctrl->stage_move_to_x_async(s_x);
    ctrl->stage_move_to_y_sync(s_y);
}

void MainWindow::on_move_to_t_plate_clicked()
{
    m_move_t_plate_thread = new std::thread(&MainWindow::move_to_t_plate,this,
                                            ui->t_well_x_combobox->currentIndex(),
                                            ui->t_well_y_spinbox->value(),
                                            p_s->m_selected_target
                                            );

}

void MainWindow::change_plate()
{
    int ymin = ctrl->stage_get_y_min_pos();
    int xmin = ctrl->stage_get_x_min_pos();
    ctrl->pipette_home_z();
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    ctrl->pipette_home_x();
    ctrl->stage_move_to_x_async(xmin);
    ctrl->stage_move_to_y_sync(ymin);
}

void MainWindow::collect_selected_obj(std::vector<int> selected_obj)
{
    auto start = std::chrono::system_clock::now();
    int x_row_max = 6;

    int y_idx = 1, x_idx = 0; // y should start 1
    for (int idx = 0; idx < selected_obj.size(); ++idx)
    {
        std:: cout <<"selected obj idx"<<selected_obj.at(idx) << std::endl;
        this->xz_stage_pickup_sph(selected_obj.at(idx));
        this->put_to_target_plate(x_idx,y_idx);
        std:: cout <<"selected obj x:y idx "<<x_idx <<" ; "<< y_idx<< std::endl;
        x_idx++;


        if(x_idx>x_row_max)// 6 is true for 96 well plate
        {

            x_idx=0;
            y_idx++; }

    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "[SPHEROID COLLECTING] elapsed time: " <<
                 elapsed_seconds.count() << "s\n";

}



void MainWindow::on_actionSpheroid_selector_triggered()
{
    sph_s->show();
    sph_s->activateWindow();
}

void MainWindow::on_actionPlate_selector_triggered()
{
    p_s->show();
    p_s->activateWindow();
}

void MainWindow::on_pushButton_6_clicked()
{
    std::vector<int> selected_obj = sph_s->list_checked();
    m_collect_thread = new std::thread(&MainWindow::collect_selected_obj,this,selected_obj);
}


void MainWindow::get_selected_source_plate()
{
    this->s_p_selected = p_s->m_selected_target;
}


void MainWindow::on_actionExport_object_properties_triggered()
{
    this->export_csv();
   // this->export_bias_xml();
    QTextStream(stdout)<< "exporting object props\n";
}


void MainWindow::export_bias_xml(){
    std::string current_fname = m_folder + "imagelist.xml";
    QFile xmlFile(current_fname.c_str());
    if (!xmlFile.open(QFile::WriteOnly | QFile::Text ))
    {
        qDebug() << "Already opened or there is another issue";
        xmlFile.close();
    }
    QTextStream xmlContent(&xmlFile);

    QDomDocument document;

    //make the root element
    //magic stuff for not randomly reordering attributes
    qSetGlobalQHashSeed(0);

    QDomElement root = document.createElement("BIAS");
    root.setAttribute("version", "\"1.0\"");
    //add it to document
    document.appendChild(root);
    QDomElement images = document.createElement("images");
    root.appendChild(images);
    using namespace  cv;
    if (scanvector.size()>0 && !m_s_t_acitive)
    {
        const float platesize_x = ui->set_plate_size_spinbox->value()*C_MM_TO_UMx0p1; // convert mm to STAGE 0.1um unit
        const float platesize_y = ui->set_plate_size_spinbox_2->value()*C_MM_TO_UMx0p1; // convert mm to STAGE 0.1um unit
        int wmax = static_cast<int>(platesize_x / m_img_width);
        int hmax = static_cast<int>(platesize_y / m_img_height);
        QTextStream(stdout) << m_img_width <<":"<<wmax<<" : "<<platesize_x<< "\n";
        int counter = 0;
        int leading = 2;
        int xval = 0; int yval=0;

        for (int i = 0; i < hmax; ++i)
        {
            if (i==0){
                yval =(m_img_height*0.05);
            }else{
                yval = yval + m_img_height*0.1 - m_img_height*0.01;
               // yval = ((m_img_height*0.5)+i*m_img_height-m_img_height*0.1)*0.1;
            }
            for (int j = 0; j < wmax; ++j)
            {

                QDomElement imagenode = document.createElement("image");
                std::string image_name = "Image_" + std::to_string(counter*0.000001).substr(8-leading)+".png";
                imagenode.setAttribute("url", image_name.c_str());
                images.appendChild(imagenode);
                QDomElement res = document.createElement("resolution");
                res.setAttribute("x", 1.428);
                res.setAttribute("y", 1.427);
                res.setAttribute("z", 1.0);
                imagenode.appendChild(res);
                QDomElement center = document.createElement("center");

                if (j == 0){
                    xval = m_img_width*0.05;}
                else{
                    xval = xval + m_img_width*0.1 - m_img_width*0.01 ;
                   // xval = ((m_img_width*0.5)+j*m_img_width-m_img_width*0.1)*0.1;
                }


                center.setAttribute("x", xval);
                center.setAttribute("y", yval);
                center.setAttribute("z", 1.0);

                imagenode.appendChild(center);

                counter = counter+1;
            }
        }

    }
    xmlContent << document.toString();

    qSetGlobalQHashSeed(-1);

}

void MainWindow::on_actionExport_Screening_to_Bias_triggered()
{
    this->export_bias_xml();
}
