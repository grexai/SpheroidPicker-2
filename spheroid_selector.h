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

struct sph_props{
    int idx;
    float perimeter;
    float area;
    float circularity;
    float maxdiameter;
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
    explicit spheroid_selector(QWidget *parent = nullptr);
    ~spheroid_selector();
    std::vector<sph_props>*current_spheroid_data=nullptr;
    std::map<QString, MemberPointer> featureMap;
    void update_scene();
    void tickListItems(QListWidget* listWidget, const std::vector<int>& indexesToTick);
    void get_statistics_of_spheroids();

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

protected:


private:
    QMap<QString, QDoubleSpinBox*> featureSpinBoxes;
    QStringList availableFeatures = {"Area", "Perimeter", "Circularity", "Maximum Diameter"};
    QStringList featuresAdded = {};
    QListWidget *availableFeaturesList;
    QImage* qframe = nullptr;
    QGraphicsPixmapItem  im_view_pxmi;
    QGraphicsScene* scene= nullptr;
    std::vector<cv::Mat> *m_bbs= nullptr;
    Ui::spheroid_selector *ui;

};



#endif // SPHEROID_SELECTOR_H
