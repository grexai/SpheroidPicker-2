#ifndef CALIBRATEDIALOG_H
#define CALIBRATEDIALOG_H
#include <types.h>
#include <QDialog>
#include <counter.h>
namespace Ui {
class calibratedialog;
}

class calibratedialog : public QDialog
{
    Q_OBJECT

public:
    explicit calibratedialog(QWidget *parent = nullptr);
    ~calibratedialog();
   int clicks;
   Counter a,b;
   bool Iscalibrating= false;
   std::vector<float>* calibpos;

//   std::vector<float>* cpos1 = nullptr;
//   std::vector<float>* cpos2 = nullptr;
//   std::vector<float>* cpos3= nullptr;
private slots:
    void on_pushButton_clicked();

//  void on_buttonBox_destroyed();

    void on_buttonBox_accepted();

    void on_calibratedialog_destroyed();



private:
    Ui::calibratedialog *ui;

};

#endif // CALIBRATEDIALOG_H
