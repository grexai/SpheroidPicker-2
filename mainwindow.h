#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QThread>
#include <QTimer>
#include <QMainWindow>
#include <QMessageBox>
#include <QGraphicsPixmapItem>
#include "imagetools.h"
#include <arduinopressurecontroller.h>
#include <PipetteController.h>
#include <stagecontroller.h>
#include <QGraphicsSceneMouseEvent>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QString con_str= "Connected";

    QString fail_str = "Failed";

    int cameraIndex=0;

    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    void displayImage();

    void SpawnThreadAndLaunch();

 //   void MainWindow::mouseMoveEvent(QMouseEvent *event);

    void porttest();

signals:
    // The signal of transmit the coordinates of the mouse position
    void signalTargetCoordinate(QPointF point);

public slots:


protected:
    arduinopressurecontroller* acp= nullptr ;
    pipetteController* apipc= nullptr;
    QSerialPort qsp_pip;
    QSerialPort qsp_pc;
    ahm::Unit* pStageUnit= nullptr;
    ahm::Unit* pRootUnit= nullptr;
    Stage *stage= nullptr;
    imagetools* imtools= nullptr;

private slots:
      bool eventFilter(QObject *obj,  QEvent *event);


     void on_Campushbtn_clicked();

     void update_window();

     void update_currentpressure();

     void show_currentpressure();

     void on_actionDark_Mode_triggered();

     void on_actionExit_triggered();

     void on_Home_pip_clicked();

     void on_p_home_x_clicked();

     void on_p_home_y_clicked();

     void on_p_home_z_clicked();

     void on_Con_pc_clicked();

     void on_Con_pip_clicked();

     void on_Con_xystage_button_clicked();

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

private:
    QTimer *timer= nullptr;
    QTimer *disp_pressure= nullptr;
    QGraphicsScene* scene= nullptr;
    QGraphicsPixmapItem  qpxmi;
    QImage* qframe = nullptr;
    Ui::MainWindow *ui= nullptr;

};

#endif // MAINWINDOW_H
