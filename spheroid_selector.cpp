#include "spheroid_selector.h"
#include "ui_spheroid_selector.h"
#include <iostream>
#include <imagetools.h>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <map>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <QStringList>
#include <QTableWidgetItem>


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
    availableFeaturesList->move(460, 340); // Example position
    availableFeaturesList->resize(120,120);
    // Set addButton parent to this widget
    QPushButton *addButton = new QPushButton("Add Feature", this);
    // Position the addButton manually
    addButton->move(540, 470); // Example position

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
    // Set parent for QLabel objects
    Arealabel.setParent(this);
    Perimeterlabel.setParent(this);
    Circulartylabel.setParent(this);
    maxdialabel.setParent(this);


    tableWidget = new QTableWidget(this);
    //tableWidget->setParent(this);
    /*
     *
     *     //int width_of_labels = 500;
    //int area_pos_y = 330;
    //int area_pos_x = 10;
    Arealabel.move(area_pos_x,area_pos_y);
    Perimeterlabel.move(area_pos_x,area_pos_y+20);
    Circulartylabel.move(area_pos_x,area_pos_y+40);
    maxdialabel.move(area_pos_x,area_pos_y+60);
    Arealabel.resize(width_of_labels,20);
    Perimeterlabel.resize(width_of_labels,20);
    Circulartylabel.resize(width_of_labels,20);
    maxdialabel.resize(width_of_labels,20);
    */

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

    QDoubleSpinBox *minSpinBox = new QDoubleSpinBox(this);
    QDoubleSpinBox *maxSpinBox = new QDoubleSpinBox(this);

    minSpinBox->setMinimum(0);
    minSpinBox->setMaximum(9999999);
    minSpinBox->setValue(0);
    maxSpinBox->setMinimum(0);
    maxSpinBox->setValue(999999.0);
    maxSpinBox->setMaximum(9999999);
    int label_x_pos = 600;
    // Manually set positions for the widgets
    int yPos = this->height()/2+60 + featuresAdded.size()*30; // Adjust Y position based on number of features already added
    featureLabel->move(label_x_pos, yPos);
    minSpinBox->move(label_x_pos+100, yPos);
    maxSpinBox->move(label_x_pos+200, yPos);

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

    // std::cout<< ui->Object_list->count() <<  std::endl;
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
    for (int idx=0;idx<ui->Object_list->count();++idx)
    {
        QListWidgetItem* item = ui->Object_list->item(idx);
        // Check if the item exists
        if (item) {
            // Set the item as selected or checked, depending on your requirement
            item->setCheckState(Qt::Unchecked); // For check state
        }
    }
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


            //std::cout << "max value of " <<  feature.toStdString() <<maxValue<< std::endl;

            if (it != featureMap.end() && current_spheroid_data->at(spheroid).*(it->second.floatMemberPtr) != 0.0) {
                std::cout << feature.toStdString() <<  "value of the feature" <<current_spheroid_data->at(spheroid).*(it->second.floatMemberPtr) << std::endl;
                float maxValue = featureSpinBoxes[feature+"_max"]->value();
                float minValue = featureSpinBoxes[feature]->value();
                std::cout << "min can be:" << minValue << ", " << "max can be:" << maxValue << std::endl;
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




void spheroid_selector::get_statistics_of_spheroids() {

    int numRows = availableFeatures.size();
    int numCols = 5; // For Feature, Average, Median, Minimum, Maximum

    this->tableWidget->setRowCount(numRows);
    this->tableWidget->setColumnCount(numCols);
    this->tableWidget->resize(400,150);
    this->tableWidget->move(20,340);
    // Set the header labels for columns
    QStringList headerLabels;
    headerLabels << "Feature" << "Average" << "Median" << "Minimum" << "Maximum";
    this->tableWidget->setHorizontalHeaderLabels(headerLabels);

    // Initialize variables to store the sum of feature values
    std::map<QString, float> sumFeatures;
    // Initialize variables to store the minimum feature values
    std::map<QString, float> minFeatures;
    // Initialize variables to store the maximum feature values
    std::map<QString, float> maxFeatures;
    // Initialize variables to store the feature values for each spheroid
    std::map<QString, std::vector<float>> spheroidFeatures;

    // Initialize variables to store the count of spheroids
    int totalSpheroids = 0;

    // Iterate over each available feature
    for (const QString& feature : availableFeatures) {
        // Initialize the sum, min, and max for the current feature
        sumFeatures[feature] = 0.0;
        minFeatures[feature] = std::numeric_limits<float>::max();
        maxFeatures[feature] = std::numeric_limits<float>::lowest();

        // Iterate over each spheroid
        for (int idx = 0; idx < ui->Object_list->count(); ++idx) {
            auto it = featureMap.find(QString(feature));
            if (it != featureMap.end()) {
                // Compute the feature value for the current spheroid
                float featureValue = current_spheroid_data->at(idx).*it->second.floatMemberPtr;
                // Add the feature value to the sum
                sumFeatures[feature] += featureValue;
                // Update the minimum feature value
                minFeatures[feature] = std::min(minFeatures[feature], featureValue);
                // Update the maximum feature value
                maxFeatures[feature] = std::max(maxFeatures[feature], featureValue);
                // Store the feature value for the current spheroid
                spheroidFeatures[feature].push_back(featureValue);
                // Increment the total count of spheroids
                totalSpheroids++;
            }
        }
    }

    // Calculate the average features
    std::map<QString, float> averageFeatures;
    for (const auto& pair : sumFeatures) {
        // Calculate the average for each feature
        averageFeatures[pair.first] = pair.second / totalSpheroids;
    }

    // Calculate the median, minimum, and maximum features
    int row = 0;
    for (const auto& pair : spheroidFeatures) {
        const QString& feature = pair.first;
        const std::vector<float>& values = pair.second;
        // Sort the feature values
        std::vector<float> sortedValues = values;
        std::sort(sortedValues.begin(), sortedValues.end());
        // Calculate the median
        float median = values.size() % 2 == 0 ?
                    (sortedValues[values.size() / 2 - 1] + sortedValues[values.size() / 2]) / 2 :
            sortedValues[values.size() / 2];

        // Update the corresponding QLabel with the statistics
        std::map<QString, QLabel*> labelMap = {
            {"Area", &Arealabel},
            {"Perimeter", &Perimeterlabel},
            {"Circularity", &Circulartylabel},
            {"Maximum Diameter", &maxdialabel}
        };
        auto it = labelMap.find(feature);
        if (it != labelMap.end()) {

            // Populate the QTableWidgetItem with the statistics
            QTableWidgetItem* featureItem = new QTableWidgetItem(feature);
            QTableWidgetItem* avgItem = new QTableWidgetItem(QString::number(averageFeatures[feature]));
            QTableWidgetItem* medItem = new QTableWidgetItem(QString::number(median));
            QTableWidgetItem* minItem = new QTableWidgetItem(QString::number(minFeatures[feature]));
            QTableWidgetItem* maxItem = new QTableWidgetItem(QString::number(maxFeatures[feature]));
            this->tableWidget->setItem(row, 0, featureItem);
            // Set the QTableWidgetItem objects to the table
            this->tableWidget->setItem(row, 1, avgItem);
            this->tableWidget->setItem(row, 2, medItem);
            this->tableWidget->setItem(row, 3, minItem);
            this->tableWidget->setItem(row, 4, maxItem);

            ++row;
            //QLabel* label = it->second;
            //label->setText(QString(feature+"\t Avg: %1\t Med: %2\t Min: %3\t Max: %4")
            //               .arg(averageFeatures[feature])
            //               .arg(median)
            //               .arg(minFeatures[feature])
            //               .arg(maxFeatures[feature]));
        }
    }
    //this->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    this->tableWidget->resizeColumnsToContents();
    //this->tableWidget->verticalHeader()
}

