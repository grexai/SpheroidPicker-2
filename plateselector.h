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



private:
    Ui::Plateselector *ui;
};

#endif // PLATESELECTOR_H
