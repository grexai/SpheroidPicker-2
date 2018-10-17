#ifndef PIPETTERCONTROLLER_H
#define PIPETTERCONTROLLER_H
#include "serialcom.h"

class pipettecontroller
{
public:
    pipettecontroller(QSerialPort& i_qsp,QString& nport): apipc_sc(i_qsp){
        isconnected = this->apipc_sc.openport(nport);
        this->apipc_sc.sp_flush();

    }
    ~pipettecontroller(){}
    bool isconnected = false;
protected:
    serialcom apipc_sc;
    enum commads{
        cabs,  //0
        cHome=29,






    };

};

#endif // PIPETTERCONTROLLER_H
