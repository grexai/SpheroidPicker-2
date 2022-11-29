#ifndef SERIALCOM_H
#define SERIALCOM_H
#include <QtSerialPort/QSerialPort>
#include <iostream>
#include <QtSerialPort>
#include <QObject>

class serialcom : public QObject
{
    Q_OBJECT
signals:
    void signal_process_qui();

public:
    serialcom(QSerialPort& i_sp, QObject* parent = nullptr): QObject(parent), sp(i_sp){}

    ~serialcom(){}

    QSerialPort& sp;

    bool openport(QString& com);

    void send(QString& command);

    QByteArray recive();

    QByteArray sendAndRecive_sync(QString& msg, QString& ansEnd);

    QByteArray sendAndReceive(QString& msg, QString& ansEnd);


    void sp_flush();

    void close_port();
    std::mutex comm_mutex;
};

#endif // SERIALCOM_H
