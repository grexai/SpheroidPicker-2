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



hardwareselector::~hardwareselector()
{
    delete ui;
}

void hardwareselector::on_pushButton_clicked()
{
    this->hide();
    main = new MainWindow(this);

    main->show();

    main->set_h4(ui->comboBox_3->currentIndex());
    main->set_pip_man(ui->pip_manip_combobox->currentIndex());
   // main->show();


    //main->ctrl->connect_microscope_unit(propreader->cfg.port_pipette,propreader->cfg.port_pressurecontrooler);
}
