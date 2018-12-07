#ifndef PIPETTERCONTROLLER_H
#define PIPETTERCONTROLLER_H
#include "serialcom.h"
#include "types.h"
//#include <stagecontroller.h>
class pipetteController
{
public:
    pipetteController(QSerialPort& i_qsp,QString& nport): apipc_sc(i_qsp){

        isconnected = this->apipc_sc.openport(nport);
    if(isconnected){
        while (this->apipc_sc.sp.waitForReadyRead(1000));
        this->apipc_sc.sp.clear();
        this->apipc_sc.send(EOM);
        this->apipc_sc.sp_flush();
    }else
        isconnected= false;
    }
    ~pipetteController(){}
    bool isconnected = false;

    QString EOM= "\r\n"; //end of message
    //sets acceleration in: mmperminute
    void setfeedrate(int acceleration); //G0
    // move Linear or interpolated default movement type is linear else Interpolated movement
    void moveAsync(float x, float y, float z,bool movetype = true); // G0

    void moveToXSync(float x_value);

    void moveToYSync(float y_value);

    void moveToZSync(float z_value);


    //G28
    void goHome(bool x, bool y, bool z);

    //G90
    void setabsoluepositioning();

    // G91
    void setrelativepositioning();

    // to define bool3

    bool getEndstopstatus(); //M119

    float3coors getcurrentpos(QByteArray& answer); //M114

    void moveThereSync(float3coors there);

    void setPipetteposition();
protected:

    serialcom apipc_sc;

};

#endif // PIPETTERCONTROLLER_H
