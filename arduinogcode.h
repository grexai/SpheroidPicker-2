#ifndef PIPETTERCONTROLLER_H
#define PIPETTERCONTROLLER_H
#include "serialcom.h"
#include "types.h"
#include <exception>
#include <iostream>
//#include <stagecontroller.h>
class arduinogcode
{
public:
    arduinogcode(QSerialPort& i_qsp,QString& nport): apipc_sc(i_qsp){

        isconnected = this->apipc_sc.openport(nport);
        if(isconnected){
            while (this->apipc_sc.sp.waitForReadyRead(1000));

            this->apipc_sc.sp.clear();
            this->apipc_sc.send(EOM);
            this->apipc_sc.sp_flush();
         }else{
            isconnected= false;
        }
    }
    ~arduinogcode(){}

    bool isconnected = false;
     //end of message

    QString EOM= "\r\n";
    int m_counter= 0;
    //G0 F<speed>
    void setfeedrate(int speed);

    //M201
    void setacceleration(int acc);
    //M205
    void setjerk(int jerk);

    //G0
    // move Linear or interpolated default movement type is linear else Interpolated movement
    void moveAsync(float x, float y, float z); // G0

    void moveToXSync(float x_value);

    void moveToYSync(float y_value);

    void moveToZSync(float z_value);

    //G28
    void goHome(bool x, bool y, bool z);

    //G90
    void setabsoluepositioning();

    // G91
    void setrelativepositioning();

    bool getEndstopstatus(); //M119

    std::vector<float> getcurrentpos(); //M114

    void MoveToXYZSync(std::vector<float> coords);

    void setPipetteposition();
    class ardinogcodeexeption: public std::exception{
    virtual  const char* what() const throw()
        {
            return "coordinate extraction error";
        }
    }ardinogcodeexeption;
protected:

    serialcom apipc_sc;

};




#endif // PIPETTERCONTROLLER_H
