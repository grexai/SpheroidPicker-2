#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QThread>
#include <QTimer>
#include <QMainWindow>
//#include <QtSerialPort/QSerialPort>
//#include <serialcom.h>
#include <arduinopressurecontroller.h>


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
    serialcom *sp = nullptr;
    serialcom *sppc = nullptr;
    arduinopressurecontroller* acp= nullptr ;
    QSerialPort qsp_pip;
    QSerialPort qsp_pc;
private slots:
     void on_Campushbtn_clicked();

     void update_window();

     void on_actionDark_Mode_triggered();

     void on_actionExit_triggered();

     void on_Home_pip_clicked();

     void on_Con_pc_clicked();

     void on_Con_pip_clicked();

     void on_SetPressure_clicked();

     void on_atm_button_clicked();

     void on_pc_Breakin_button_clicked();

private:
    QTimer *timer;
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
