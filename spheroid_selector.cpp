#include "spheroid_selector.h"
#include "ui_spheroid_selector.h"
#include <iostream>
#include <imagetools.h>

spheroid_selector::spheroid_selector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::spheroid_selector)
{
    ui->setupUi(this);
    ui->selected_item_scene->setScene(new QGraphicsScene(this));
    ui->selected_item_scene->scene()->addItem(&im_view_pxmi);
    m_bbs = new std::vector<cv::Mat>;
}

spheroid_selector::~spheroid_selector()
{
    delete ui;
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
    std::vector<int> checked ;
    for (int idx=0; idx<ui->Object_list->count();++idx )
    {
       auto item = ui->Object_list->item(idx);
        if(item->checkState()==2)
        {
            checked.push_back(idx);
        }
    }
    return checked;
}

void spheroid_selector::set_bbs(std::vector<cv::Mat>& bbs)
{
  // m_bbs->clear();

    m_bbs = &bbs;
}

void spheroid_selector::clear_list()
{
    ui->Object_list->clear();
}


void spheroid_selector::on_Object_list_itemActivated(QListWidgetItem *item)
{

}

void spheroid_selector::on_Object_list_currentRowChanged(int currentRow)
{

}

void spheroid_selector::on_Object_list_activated(const QModelIndex &index)
{
   cv::imshow("asdsad",m_bbs->at(index.row()));
   cv::waitKey(1);
   auto cfrm=  m_bbs->at(index.row());
   delete qframe;
   cv::cvtColor(m_bbs->at(index.row()),m_bbs->at(index.row()),CV_BGR2RGB);
 //  imagetools imtools;
   //imtools.convert_bgr_to_rgb(cfrm)
   qframe = new QImage(const_cast< unsigned char*>(m_bbs->at(index.row()).data),m_bbs->at(index.row()).cols, m_bbs->at(index.row()).rows, QImage::Format_RGB888);
   im_view_pxmi.setPixmap(QPixmap::fromImage(*qframe));
   ui->selected_item_scene->fitInView(&im_view_pxmi,Qt::KeepAspectRatio);
}

void spheroid_selector::update_scene(){

}
