#ifndef SPHEROID_SELECTOR_H
#define SPHEROID_SELECTOR_H
#include <QListWidgetItem>
#include <QWidget>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
namespace Ui {
class spheroid_selector;
}

class spheroid_selector : public QWidget
{
    Q_OBJECT

public:
    explicit spheroid_selector(QWidget *parent = nullptr);
    ~spheroid_selector();
    void update_scene();
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

private:
    QImage* qframe = nullptr;
    QGraphicsPixmapItem  im_view_pxmi;
    QGraphicsScene* scene= nullptr;
    std::vector<cv::Mat> *m_bbs= nullptr;
    Ui::spheroid_selector *ui;
};


struct sph_props{
    int idx;
    float area;
    float perimeter;
    float circularity;
};


#endif // SPHEROID_SELECTOR_H
