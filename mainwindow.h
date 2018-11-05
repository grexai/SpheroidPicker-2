#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QThread>
#include <QTimer>
#include <QMainWindow>

#include "imagetools.h"
#include <arduinopressurecontroller.h>
#include <PipetteController.h>
#include <stagecontroller.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QString con_str= "Connected";

    QString fail_str = "Failed";

    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    void displayImage();

    void SpawnThreadAndLaunch();

    void porttest();

public slots:

protected:
    arduinopressurecontroller* acp= nullptr ;
    pipetteController* apipc= nullptr;
    QSerialPort qsp_pip;
    QSerialPort qsp_pc;
private slots:
     void on_Campushbtn_clicked();

     void update_window();

     void update_currentpressure();

     void show_currentpressure();

     void on_actionDark_Mode_triggered();

     void on_actionExit_triggered();

     void on_Home_pip_clicked();

     void on_Con_pc_clicked();

     void on_Con_pip_clicked();

     void on_Con_xystage_button_clicked();

     void on_SetPressure_clicked();

     void on_atm_button_clicked();

     void on_pc_Breakin_button_clicked();

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

     void on_lcdNumber_overflow();



private:
    QTimer *timer;
    QTimer *disp_pressure;

    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
