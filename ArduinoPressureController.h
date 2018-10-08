#ifndef ARDUINOPRESSURECONTROLLER_H
#define ARDUINOPRESSURECONTROLLER_H

//#include "QtSerialPort"
#include "serialcom.h"
#include <iostream>
#include <sstream>
#include <string>

class arduinopressurecontroller
{
public:
    arduinopressurecontroller(QSerialPort& i_qsp,QString& nport): acp_sc(i_qsp){
        isconnected = this->acp_sc.openport(nport);
        this->acp_sc.sp_flush(); }

    ~arduinopressurecontroller(){}

    bool isconnected= false;
    int error;
    QString EOM= ";\r\n"; //end of message

    void checkAckOrSetError(const std::string& answer);
    void requestPressure(const float pressure);
    float getRequestedPressureValue();
    float getPipettePressure();
    float getTankPressure();
    std::string* getHelpMessage();
    void breakIn(const float vacuumValue,const float delaySeconds);
    void calibrate();
    std::string getStatus();
protected:
    static const int UnknownError;
    serialcom acp_sc;
    enum commands
        {
          kAcknowledge,                   //  0
          kError,                         //  1
          kGetRequestedPressure,          //  2
          kGetRequestedPressureAnswer,    //  3
          kSetRequestedPressure,          //  4
          kGetTankPressure,               //  5
          kGetTankPressureAnswer,         //  6
          kSetTankPressure,               //  7
          kHelp,                          //  8
          kBreakIn,                       //  9
          kCalibration,                   // 10
          kGetPipettePressure,            // 11
          kGetPipettePressureAnswer       // 12
        };

};

#endif // ARDUINOPRESSURECONTROLLER_H
