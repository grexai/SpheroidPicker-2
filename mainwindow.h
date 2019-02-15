#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QThread>
#include <QTimer>
#include <QMainWindow>
#include <QMessageBox>
#include <QGraphicsPixmapItem>
#include "imagetools.h"
#include <arduinopressurecontroller.h>
#include <arduinogcode.h>
#include <stagecontroller.h>
#include <QGraphicsSceneMouseEvent>
#include <calibratedialog.h>
#include <comps.h>
#include <controller.h>
#include <cameracv.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    QString con_str= "Connected";

    QString fail_str = "Failed";
    QThread thread;
    bool Iscameraopen= false;
    int cameraIndex=0;

    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    void displayImage();

    void SpawnThreadAndLaunch();

 //   void MainWindow::mouseMoveEvent(QMouseEvent *event);


    void porttest();
    cv::Mat TM;
    cv::Mat* imgc;
    cv::Mat pc;
    centers centers;


signals:
    // The signal of transmit the coordinates of the mouse position
    void signalTargetCoordinate(QPointF point);

public slots:


protected:
    controller* ctrl = nullptr;
    CameraCV* cameracv = nullptr;
    imagetools* imtools= nullptr;
    std::vector<float>* cpos1 = nullptr;
    std::vector<float>* cpos2 = nullptr;
    std::vector<float>* cpos3= nullptr;
    QPoint* point_mouse= nullptr;

private slots:
      bool eventFilter(QObject *obj,  QEvent *event);

 //    void ShowContextMenu(const QPoint &pos);

     void on_Campushbtn_clicked();

     void calib_frame_view(cv::Mat& disp);

     void update_window();

     void update_currentpressure();

     void MoveAction();

     void show_currentpressure();

     void on_actionDark_Mode_triggered();

     void on_actionExit_triggered();

     void on_Home_pip_clicked();

     void on_p_home_x_clicked();

     void on_p_home_y_clicked();

     void on_p_home_z_clicked();

     void on_SetPressure_clicked();

     void on_atm_button_clicked();

     void on_pc_pulse_button_clicked();

     void on_get_coors_pushButton_clicked();

     void on_s_xp_button_clicked();

     void on_s_xm_button_clicked();

     void on_s_yp_button_clicked();

     void on_s_ym_button_clicked();

     void on_p_xp_button_clicked();

     void on_p_xm_button_clicked();

     void on_p_yp_button_clicked();

     void on_p_ym_button_clicked();

     void on_p_zp_button_clicked();

     void on_p_zm_btton_clicked();

     void on_actionOpen_console_triggered();

     void on_s_center_button_clicked();

     void on_exptime_button_clicked();

     void on_width_button_clicked();

     void on_height_button_clicked();

     void on_s_set_speed_button_clicked();

     void on_save_image_button_clicked();

     void on_s_get_coors_pushButton_clicked();

     void on_graphicsView_customContextMenuRequested(const QPoint &pos);

     void on_start_screening_clicked();

     void on_pushButton_5_clicked();

     void screensample();

     void on_actionCalibrate_Pipette_triggered();
     void on_actionSpheroid_picker_triggered();



     void on_p_set_speed_clicked();

     void on_p_set_speed_spinbox_valueChanged(int arg1);

     void on_pip_step_spinbox_valueChanged(int arg1);

private:
    QTimer *timer= nullptr;
    QTimer *disp_pressure= nullptr;
    QGraphicsScene* scene= nullptr;
    QGraphicsPixmapItem  qpxmi;
    QImage* qframe = nullptr;
    Ui::MainWindow *ui= nullptr;
    calibratedialog *calib= nullptr;


    //TEST

    QTimer *timer1= nullptr ;
    QThread *thread1= nullptr;
};

#endif // MAINWINDOW_H
