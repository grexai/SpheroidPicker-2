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
    QString msg = "G28";
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
    }
*/
    apipc_sc.send(msg);
}

void pipetteController::moveToXAsync(float x_value){
    QString msg= "G0";
    apipc_sc.send(msg.append("X").append(QString::number(x_value,'f',2)).append(EOM));
}

void pipetteController::moveToYAsync(float y_value){
    QString msg= "G0";
    apipc_sc.send(msg.append("Y").append(QString::number(y_value,'f',2)).append(EOM));
}

void pipetteController::moveToZAsync(float z_value){
    QString msg= "G0";
    apipc_sc.send(msg.append("Z").append(QString::number(z_value,'f',2)).append(EOM));
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
//  apipc_sc.sp.clear();
    QString msg = "M114";
    answer=apipc_sc.sendAndReceive(msg,EOM);
    //split strings by :
    QString s(answer);
    QStringList resultStrings =  s.split(':');
    Float3coor pipcoors;
    // get floating point number regularexpressions
    QRegExp xRegExp("(-?\\d+(?:[\\.,]\\d+(?:e\\d+)?)?)");
    std::vector<float> coors;
    for (int i=1;i<4;i++){
         xRegExp.indexIn( resultStrings.at(i));
         QStringList xList = xRegExp.capturedTexts();
         try {
             coors.push_back( xList.begin()->toFloat());
         }
         catch (...)
         {
             std::cout << "extractiing cooordinates error error " << std::endl;
            //error extracting coord
         }
    }

    pipcoors.x = coors.at(0);
    pipcoors.y = coors.at(1);
    pipcoors.z = coors.at(2);

    return pipcoors;
}

