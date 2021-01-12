#include "spheroid_selector.h"
#include "ui_spheroid_selector.h"
#include <iostream>
#include <imagetools.h>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>

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
    //ui->Object_list
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
//   cv::cvtColor(cfrm,cfrm,CV_BGR2RGB);
 //  imagetools imtools;
   //imtools.convert_bgr_to_rgb(cfrm)
void spheroid_selector::on_Object_list_activated(const QModelIndex &index)
{
   cv::Mat3b  cfrm;
   m_bbs->at(index.row()).copyTo(cfrm);
   cv::cvtColor(cfrm,cfrm,CV_BGR2RGB,0);
 //  cfrm.c
   delete qframe;
   qframe= nullptr;
   qframe = new QImage(cfrm.cols,cfrm.rows, QImage::Format_RGB32);
         for (int y = 0; y < cfrm.rows; ++y) {
           //  cfrm.at[y]
                 const cv::Vec3b *srcrow = cfrm[y];
                 QRgb *destrow = (QRgb*)qframe->scanLine(y);
                 for (int x = 0; x < cfrm.cols; ++x) {
                         destrow[x] = qRgba(srcrow[x][2], srcrow[x][1], srcrow[x][0], 255);
                 }
         }


   im_view_pxmi.setPixmap( QPixmap::fromImage(*qframe) );
    //ui->selected_item_scene->fitInView(&im_view_pxmi,Qt::KeepAspectRatio);
    ui->selected_item_scene->fitInView(&im_view_pxmi, Qt::KeepAspectRatio);
    //ui->selected_item_scene->setScene(&scene);
 //  scene->addItem(item);
  // ui->selected_item_scene->setScene(scene);
}


void spheroid_selector::update_scene()
{

}

