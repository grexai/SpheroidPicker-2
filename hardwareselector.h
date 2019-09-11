#ifndef HARDWARESELECTOR_H
#define HARDWARESELECTOR_H

#include <QDialog>
#include <mainwindow.h>
namespace Ui {
class hardwareselector;
}

class hardwareselector : public QDialog
{
    Q_OBJECT

public:
    explicit hardwareselector(QWidget *parent = nullptr);
    ~hardwareselector();


private slots:
    void on_pushButton_clicked();

private:
    Ui::hardwareselector *ui;
    MainWindow *main;
};

#endif // HARDWARESELECTOR_H
