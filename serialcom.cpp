#include "serialcom.h"


bool serialcom::openport(std::string com){
    this->sp.setPortName(com.c_str());
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

};
void serialcom::pflush(){

}
void serialcom::sendHOME(){
    sp.readAll();
    sp.write("G28");

}
