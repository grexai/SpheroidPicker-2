#ifndef SPHEROID_SELECTOR_H
#define SPHEROID_SELECTOR_H
#include <QListWidgetItem>
#include <QWidget>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QStringList>
#include <QDebug>
#include <QLabel>
#include <variant>
#include <QMap>
#include <QTableWidget>
#include <QTableWidgetItem>


struct sph_props{
    int idx;
    float perimeter;
    float area;
    float circularity;
    float maxdiameter;
    float sphericity;
};

// Struct to hold either float or int member pointer
struct MemberPointer {
    union {
        float sph_props::*floatMemberPtr;
        int sph_props::*intMemberPtr;
    };

    enum Type { FLOAT, INT } type;
};


namespace Ui {
class spheroid_selector;
}

class spheroid_selector : public QWidget
{
    Q_OBJECT

public:

    float getFeatureStatistic(const QString& feature, const QString& statistic);
    explicit spheroid_selector(QWidget *parent = nullptr);
    ~spheroid_selector();
    std::vector<sph_props>*current_spheroid_data=nullptr;
    std::map<QString, MemberPointer> featureMap;
    void update_scene();
    void tickListItems(QListWidget* listWidget, const std::vector<int>& indexesToTick);
    void get_statistics_of_spheroids();
    void check_list_elements(bool check=true);

signals:
    void fill_list();
public slots:

    void set_list(QString data);
    void list_props();
    void set_bbs(std::vector<cv::Mat>& bbs);
    std::vector<int> list_checked();
    void clear_list();


private slots:

    void on_Object_list_itemActivated(QListWidgetItem *item);

    void on_Object_list_currentRowChanged(int currentRow);

    void on_Object_list_activated(const QModelIndex &index);



    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void addFeature();

    void removeAllFeatures();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

protected:


private:

    // Initialize variables to store the sum of feature values
    std::map<QString, float> sumFeatures;
    // Initialize variables to store the minimum feature values
    std::map<QString, float> minFeatures;
    // Initialize variables to store the maximum feature values
    std::map<QString, float> maxFeatures;
    // Initialize variables to store the feature values for each spheroid
    std::map<QString, std::vector<float>> spheroidFeatures;

    std::map<QString, float> averageFeatures;


    QMap<QString, QLabel*> featureLabels;
    QMap<QString, QDoubleSpinBox*> featureSpinBoxes;
    QStringList availableFeatures = {"Area", "Perimeter", "Circularity", "Maximum Diameter","Sphericity"};
    QStringList featuresAdded = {};
    QTableWidget* tableWidget=nullptr;
    QListWidget *availableFeaturesList;

    QImage* qframe = nullptr;
    QGraphicsPixmapItem  im_view_pxmi;
    QGraphicsScene* scene= nullptr;
    std::vector<cv::Mat> *m_bbs= nullptr;
    Ui::spheroid_selector *ui;

};



#endif // SPHEROID_SELECTOR_H
