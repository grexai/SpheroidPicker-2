#include "spheroid_selector.h"
#include "ui_spheroid_selector.h"
#include <iostream>
#include <imagetools.h>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <map>


spheroid_selector::spheroid_selector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::spheroid_selector)
{
    ui->setupUi(this);
    ui->selected_item_scene->setScene(new QGraphicsScene(this));
    ui->selected_item_scene->scene()->addItem(&im_view_pxmi);

    // Set availableFeaturesList parent to this widget
    availableFeaturesList = new QListWidget(this);
    availableFeaturesList->addItems(availableFeatures);
    // Position the availableFeaturesList manually
    availableFeaturesList->move(350, 300); // Example position
    availableFeaturesList->resize(120,120);
    // Set addButton parent to this widget
    QPushButton *addButton = new QPushButton("Add Feature", this);
    // Position the addButton manually
    addButton->move(350, 430); // Example position

    // Connect addButton click signal to addFeature slot
    connect(addButton, &QPushButton::clicked, this, &spheroid_selector::addFeature);
    // Map feature names to member names

    featureMap["Area"] = {&sph_props::area, MemberPointer::FLOAT};
    featureMap["Perimeter"] = {&sph_props::perimeter, MemberPointer::FLOAT};
    featureMap["Maximum Diameter"] = {&sph_props::maxdiameter, MemberPointer::FLOAT};
    featureMap["Circularity"] = {&sph_props::circularity, MemberPointer::FLOAT};
    //featureMap["idx"] = {&sph_props::idx, MemberPointer::INT}; // Adding "idx" feature
    // Initialize the featureMap entries
    MemberPointer idxPointer;
    idxPointer.intMemberPtr = &sph_props::idx;
    idxPointer.type = MemberPointer::INT;
    featureMap["idx"] = idxPointer;



}

spheroid_selector::~spheroid_selector()
{
    delete ui;
}

void spheroid_selector::addFeature() {
    QListWidgetItem *selectedItem = availableFeaturesList->currentItem();
    if (!selectedItem) {
        qDebug() << "Please select a feature to add.";
        return;
    }

    QString feature = selectedItem->text();

    if (featuresAdded.contains(feature)) {
        qDebug() << "Feature already added.";
        return;
    }

    QLabel *featureLabel = new QLabel(feature + ":", this);

    QSpinBox *minSpinBox = new QSpinBox(this);
    QSpinBox *maxSpinBox = new QSpinBox(this);

    minSpinBox->setMinimum(0);
    minSpinBox->setMaximum(9999999);
    maxSpinBox->setMinimum(0);
    maxSpinBox->setMaximum(9999999);

    // Manually set positions for the widgets
    int yPos = this->height()-60-availableFeatures.size()*30 + featuresAdded.size()*30; // Adjust Y position based on number of features already added
    featureLabel->move(10, yPos);
    minSpinBox->move(150, yPos);
    maxSpinBox->move(250, yPos);

    // Add widgets to the spheroid_selector widget
    featureLabel->show();
    minSpinBox->show();
    maxSpinBox->show();

    featuresAdded.append(feature);
    // Store the spin boxes in the map
    featureSpinBoxes.insert(feature, minSpinBox);
    featureSpinBoxes.insert(feature + "_max", maxSpinBox);
}

void spheroid_selector::set_list(QString data)
{
    ui->Object_list->addItem(data);
    QTextStream(stdout)<< data << '\n'  ;
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
    cv::cvtColor(cfrm,cfrm,cv::COLOR_BGR2RGB,0);
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


void spheroid_selector::on_pushButton_clicked()
{
    this->close();
}


void spheroid_selector::on_pushButton_2_clicked()
{
    std::vector<int> satisfyingSpheroids;

    std::cout << "number of spheroids to check" << current_spheroid_data->size() <<  std::endl;
    for (int spheroid=0; spheroid < current_spheroid_data->size();++spheroid  ){
        bool satisfiesCriteria = true;
        // Check which features are available in the sph_props instance
        //std::cout <<"features added" << std::endl;
        //for (const QString& feature : featuresAdded) {
        //    qDebug() << feature;
        //}
        //std::cout <<"feature map" << std::endl;
        //for (const auto& pair : featureMap) {
        //    std::cout << "Feature: " << pair.first.toStdString() << ", Pointer to Member: " << pair.second.floatMemberPtr << std::endl;
        //}

        for (const QString& feature : featuresAdded) {
            // Convert feature name to lower case for case-insensitive comparison
            QString lowercaseFeature = feature.toLower();
            //std::cout << feature.toStdString() << std::endl;

            // Check if the feature is mapped and the corresponding member is not zero
            auto it = featureMap.find(QString(feature));

            //std::cout <<"example currentspheroid data" << current_spheroid_data->at(spheroid).circularity << std::endl;
            //std::cout <<"featuremap size: " << featureMap.size() << std::endl;

            int maxValue = featureSpinBoxes[feature+"_max"]->value();
            //std::cout << "max value of " <<  feature.toStdString() <<maxValue<< std::endl;
            if (it != featureMap.end() && current_spheroid_data->at(spheroid).*(it->second.floatMemberPtr) != 0.0) {
                std::cout << feature.toStdString() <<  "value of the feature" <<current_spheroid_data->at(spheroid).*(it->second.floatMemberPtr) << std::endl;
                int maxValue = featureSpinBoxes[feature+"_max"]->value();
                int minValue = featureSpinBoxes[feature]->value();
                // Check if the value of the feature satisfies the min-max conditions
                float featureValue = current_spheroid_data->at(spheroid).*it->second.floatMemberPtr;
                if (featureValue < minValue || featureValue > maxValue) {
                    satisfiesCriteria = false;
                    break; // No need to check other features if this one fails
                }
            } else {
                std::cout << feature.toStdString() << " is not available." << std::endl;
                // Feature is not available or value is zero
                satisfiesCriteria = false;
                break; // No need to check other features if this one fails
            }
            // If all features satisfy the conditions, save the index of the spheroid
        }
        if (satisfiesCriteria) {
            satisfyingSpheroids.push_back(spheroid);
        }

    }

    std::cout<< "list of filtered spheroids indices "<< std::endl;
    for (int idx=0; idx< satisfyingSpheroids.size(); ++idx)
    {
        std::cout<< satisfyingSpheroids.at(idx) << std::endl;
    }
    tickListItems(ui->Object_list,satisfyingSpheroids);
}




void spheroid_selector::tickListItems(QListWidget* listWidget, const std::vector<int>& indexesToTick) {
    // Iterate over the indexes to tick
    for (int index : indexesToTick) {
        // Check if the index is within the valid range
        if (index >= 0 && index < listWidget->count()) {
            // Get the item at the specified index
            QListWidgetItem* item = listWidget->item(index);
            // Check if the item exists
            if (item) {
                // Set the item as selected or checked, depending on your requirement
                item->setSelected(true); // For selection
                item->setCheckState(Qt::Checked); // For check state
            }
        }
    }
}
