#include "PipetteController.h"


//*********************************//
//  Controlling based on:          //
//  https://reprap.org/wiki/G-code //
//*********************************//

void pipetteController::setfeedrate(int acceleration){
    QString msg = "G0F";
    msg= msg.append(QString::number(acceleration)).append(EOM);
    apipc_sc.send(msg);
}

void pipetteController::goHome(bool x, bool y, bool z){
    QString msg = "G28\r\n";
    if (x){
        msg.append("X");
    }
    if(y){
        msg.append("Y");
    }
    if(z){
        msg.append("Z");
    }
    msg.append(EOM);
    apipc_sc.send(msg);
}
void pipetteController::moveAsync(float x, float y, float z,bool movetype){
    QString msg(QString::null);
    if (movetype){
        QString msg= "G0";
    }
    else{
        QString msg = "G1";
    }
/*
    if (x>0){
       msg.append("X").append(QString::number(x));
    }
    if (y>0){
        msg.append("Y").append(QString::number(y));
    }
    if (z>0){
        msg.append("Z").append(QString::number(z));
    }*/
    apipc_sc.send(msg);
}

void pipetteController::moveToXAsync(float x_value){
    QString msg= "G0";
    apipc_sc.send(msg.append("X").append(QString::number(x_value)).append(EOM));
}
void pipetteController::moveToYAsync(float y_value){
    QString msg= "G0";
    apipc_sc.send(msg.append("Y").append(QString::number(y_value)).append(EOM));
}

void pipetteController::moveToZAsync(float z_value){
    QString msg= "G0";
    apipc_sc.send(msg.append("Z").append(QString::number(z_value)).append(EOM));
}

void pipetteController::setabsoluepositioning(){
    QString msg = "G90";
    apipc_sc.send(msg.append(EOM));
}
void pipetteController::setrelativepositioning(){
    QString msg = "G91";
    apipc_sc.send(msg.append(EOM));
}





Float3coor pipetteController::getcurrentpos(QByteArray& answer){
    QString msg = "M114";
    answer=apipc_sc.sendAndReceive(msg,"EOM");


    Float3coor pipcoors;
    return pipcoors;

}

