#include "hardwareselector.h"
#include "ui_hardwareselector.h"

hardwareselector::hardwareselector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::hardwareselector)
{
    ui->setupUi(this);

    main->setdarkstyle();

}

hardwareselector::~hardwareselector()
{
    delete ui;
}

void hardwareselector::show_hw_selector(){
    this->show();
}


void hardwareselector::on_pushButton_clicked()
{
    this->hide();
    main = new MainWindow(nullptr);
    main->show();
    main->set_h4(ui->comboBox_3->currentIndex());
    main->set_pip_man(ui->pip_manip_combobox->currentIndex());
    connect(main,SIGNAL(close_and_return_hw()),this,SLOT(show_hw_selector()));
}
