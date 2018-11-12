#ifndef SERIALCOM_H
#define SERIALCOM_H
#include <QtSerialPort/QSerialPort>
#include <iostream>
#include <QtSerialPort>

class serialcom
{

    std::mutex comm_mutex;

public:
    serialcom(QSerialPort& i_sp): sp(i_sp){}

     ~serialcom(){}

    QSerialPort& sp;

    bool openport(QString& com);

    void send(QString& command);

    QByteArray recive();

    QByteArray sendAndReceive(QString& msg, QString& ansEnd);

    void sp_flush();

};

#endif // SERIALCOM_H
