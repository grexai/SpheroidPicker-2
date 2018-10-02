#ifndef SERIALCOM_H
#define SERIALCOM_H
#include <QtSerialPort/QSerialPort>

class serialcom
{


   // ~serialcom(){}
public:
    QSerialPort& sp;
    serialcom(QSerialPort& i_sp): sp(i_sp){

    }
    void openport();
    void pflush();
    void sendHOME();
};

#endif // SERIALCOM_H
