#include "serialcom.h"

bool serialcom::openport(QString& com){
    this->sp.setPortName(com);
    this->sp.setBaudRate(QSerialPort::Baud115200);
    this->sp.setDataBits(QSerialPort::Data8);
    this->sp.setParity(QSerialPort::NoParity);
    this->sp.setStopBits(QSerialPort::OneStop);
    this->sp.setFlowControl(QSerialPort::NoFlowControl);

   if ( this->sp.open(QIODevice::ReadWrite))
   {
      return true;
   }
   else{

      return false;
   }
}

void serialcom::send(QString& command){
    QByteArray byte_command =command.toLocal8Bit();
    std::lock_guard<std::mutex> lock(comm_mutex);
    sp.write(byte_command);
    sp.waitForBytesWritten(-1);
};

QByteArray  serialcom::recive(){
    std::lock_guard<std::mutex> lock(comm_mutex);
    QByteArray answer = sp.readLine();
    sp.waitForReadyRead(-1);
    return answer;
}

QByteArray serialcom::sendAndReceive(const QString& msg,const QString& ansEnd)
{
    std::lock_guard<std::mutex> lock(comm_mutex);
    QByteArray byte_command =msg.toLocal8Bit();
    sp.write(byte_command);
    sp.waitForBytesWritten(-1);
    QByteArray answer = sp.readLine();
    sp.waitForReadyRead(-1);
    return answer;
}

void serialcom::sp_flush(){
    sp.flush();
}
