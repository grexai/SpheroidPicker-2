#include "calibratedialog.h"
#include "ui_calibratedialog.h"
#include "arduinogcode.h"
#include "imagetools.h"
#include "mainwindow.h"

calibratedialog::calibratedialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::calibratedialog)
{

    clicks=0;
    ui->setupUi(this);

}

calibratedialog::~calibratedialog()
{
    Iscalibrating= false;
    delete ui;
}

void calibratedialog::on_pushButton_clicked()
{

    QString txt = "Point ";
    clicks=clicks+1;
  /*  if (clicks==4)
    {
        //txt.append(QString::number(clicks+1)).append(" ready");

        clicks = 0;
    }*/
    txt.append(QString::number(clicks+1)).append(" ready");
    ui->pushButton->setText(txt);

}

void calibratedialog::on_buttonBox_accepted()
{
   Iscalibrating = false;
   this->~calibratedialog();


}
