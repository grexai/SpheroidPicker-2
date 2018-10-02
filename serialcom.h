#ifndef SERIALCOM_H
#define SERIALCOM_H
#include <QtSerialPort/QSerialPort>
#include <iostream>

class serialcom
{


   // ~serialcom(){}
public:
    QSerialPort& sp;
    serialcom(QSerialPort& i_sp): sp(i_sp){

    }
    bool openport(std::string com);
    void pflush();
    void sendHOME();
};

#endif // SERIALCOM_H
