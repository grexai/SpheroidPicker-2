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

    QByteArray sendAndRecive_sync(QString& msg, QString& ansEnd);

    QByteArray sendAndReceive(QString& msg, QString& ansEnd);

    void sp_flush();

    void close_port();
};

#endif // SERIALCOM_H
