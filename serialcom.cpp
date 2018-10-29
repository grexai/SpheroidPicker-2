#include "serialcom.h"

bool serialcom::openport(QString& com){
    this->sp.setPortName(com);
    this->sp.setBaudRate(QSerialPort::Baud115200);
    this->sp.setDataBits(QSerialPort::Data8);
    this->sp.setParity(QSerialPort::NoParity);
    this->sp.setStopBits(QSerialPort::OneStop);
    this->sp.setFlowControl(QSerialPort::NoFlowControl);
    this->sp.open(QIODevice::ReadWrite);
   if ( sp.isOpen() && sp.isWritable() )
   {
      return true;
   }
   else{
      return false;
   }
}

void serialcom::send(QString& command){
    std::lock_guard<std::mutex> lock(comm_mutex);
    QByteArray byte_command =command.toLocal8Bit();
    sp.write(byte_command);
    sp.waitForBytesWritten(100);
};

QByteArray  serialcom::recive(){
    std::lock_guard<std::mutex> lock(comm_mutex);
    QByteArray answer = sp.readLine();
    sp.waitForReadyRead(100);
    return answer;
}

QByteArray serialcom::sendAndReceive(QString& msg, QString& ansEnd)
{
    std::lock_guard<std::mutex> lock(comm_mutex);
    QString cmd = msg.append(ansEnd);
    QByteArray byte_command = msg.toLocal8Bit();
    sp.write(byte_command);
    sp.waitForBytesWritten(200);
    sp.waitForReadyRead(200);
    QByteArray answer = sp.readAll();
  //  QByteArray readData = sp.readAll();
    while ( sp.waitForReadyRead(200))
        answer.append(sp.readAll());

   // answer.append(sp.readAll());
    return answer;
}

void serialcom::sp_flush(){
    sp.flush();
}
