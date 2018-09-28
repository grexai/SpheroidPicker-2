#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QThread>
#include <QTimer>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void displayImage();
    void SpawnThreadAndLaunch();

public slots:

private slots:
    void on_Campushbtn_clicked();
     void update_window();
     void on_actionDark_Mode_triggered();

private:
    QTimer *timer;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
