#include "spheroid_selector.h"
#include "ui_spheroid_selector.h"
#include <iostream>
spheroid_selector::spheroid_selector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::spheroid_selector)
{
    ui->setupUi(this);
}

spheroid_selector::~spheroid_selector()
{
    delete ui;
}

void spheroid_selector::on_pushButton_clicked()
{   this->list_checked();
}

void spheroid_selector::set_list(QString data)
{
    ui->Object_list->addItem(data);
   // ui->Object_list->selectAll();
 //   ui->Object_list->currentItem()->setFlags(Qt::ItemIsUserCheckable);
}

void spheroid_selector::list_props()
{
    std::cout<< ui->Object_list->count() <<  std::endl;
    for (int idx=0; idx<ui->Object_list->count();++idx )
    {
       auto item = ui->Object_list->item(idx);

       item->setFlags(item->flags() | Qt::ItemIsUserCheckable);

       item->setCheckState(Qt::Unchecked);

    }
}

std::vector<int> spheroid_selector::list_checked()
{
    std::vector<int> checked = {0};
    for (int idx=0; idx<ui->Object_list->count();++idx )
    {
       auto item = ui->Object_list->item(idx);

        std::cout<<item->checkState()<< std::endl;
        if(item->checkState()==2)
        {
            checked.push_back(idx);
        }
    }
    return checked;
}



