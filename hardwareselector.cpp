#include "hardwareselector.h"
#include "ui_hardwareselector.h"

hardwareselector::hardwareselector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::hardwareselector)
{
    ui->setupUi(this);
    //main->setdefault();
    main->setdarkstyle();

}

#include <QSplashScreen>

hardwareselector::~hardwareselector()
{
    delete ui;
}

void hardwareselector::on_pushButton_clicked()
{
    this->hide();
    main = new MainWindow(this);
 //   QPixmap pixmap("D:/dev/cpp/VS2015/gpuphasefield/2d-segmentation/test-images/black.png");
 //   QSplashScreen splash(pixmap);
  //  splash.show();
  //  QTimer::singleShot(5000,&splash,SLOT(show()));

    main->show();
    main->set_h4(ui->comboBox_3->currentIndex());
   // main->show();


    //main->ctrl->connect_microscope_unit(propreader->cfg.port_pipette,propreader->cfg.port_pressurecontrooler);
}
