#ifndef SPHEROID_SELECTOR_H
#define SPHEROID_SELECTOR_H

#include <QWidget>

namespace Ui {
class spheroid_selector;
}

class spheroid_selector : public QWidget
{
    Q_OBJECT

public:
    explicit spheroid_selector(QWidget *parent = nullptr);
    ~spheroid_selector();
signals:
    void fill_list();
public slots:
    void set_list(QString data);
    void list_props();
    std::vector<int> list_checked();
    void clear_list();
private slots:
    void on_pushButton_clicked();

private:
    Ui::spheroid_selector *ui;
};

#endif // SPHEROID_SELECTOR_H
