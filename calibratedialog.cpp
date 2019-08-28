#include "calibratedialog.h"
#include "ui_calibratedialog.h"
#include "arduinogcode.h"
#include "imagetools.h"
#include "mainwindow.h"

calibratedialog::calibratedialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::calibratedialog)
{
     QObject::connect(&a, &Counter::valueChanged,&b, &Counter::setValue);

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


    //a.setValue(0);
    a.setValue((a.value()+1));
    ui->pushButton->setText(QString::number((a.value())+1))  ;          //QString txt = "Point ";;
    QTextStream(stdout)<< "v  " << a.value();
    if(a.value()==3){
        ui->helper_label->setText("calibration finished! \n Press OK to close this window ");
       //                       );
        ui->pushButton->deleteLater();
    }
    //clicks=clicks+1;
    //txt.append(QString::number(clicks+1)).append("ready");
    //ui->pushButton->setText(txt);
}

void calibratedialog::on_buttonBox_accepted()
{
   Iscalibrating = false;
   this->deleteLater();
}

void calibratedialog::on_calibratedialog_destroyed()
{
   Iscalibrating = false;

}
