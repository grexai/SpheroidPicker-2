#ifndef PLATESELECTOR_H
#define PLATESELECTOR_H

#include <QDialog>

namespace Ui {
class Plateselector;
}

class Plateselector : public QDialog
{
    Q_OBJECT

public:
    explicit Plateselector(QWidget *parent = nullptr);
    ~Plateselector();
    int m_selected_source = 2;
    int m_selected_target = 2;
    void select_source_plate();
private slots:
    void on_s_p_selector_currentIndexChanged(int index);
    void on_t_p_selector_currentIndexChanged(int index);

    void on_ps_button_clicked();

signals:
    void signal_s_p_changed();
    void signal_t_p_changed();

private:
    Ui::Plateselector *ui;
};

#endif // PLATESELECTOR_H
