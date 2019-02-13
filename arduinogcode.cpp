#include "arduinogcode.h"


//*********************************//
//  Controlling based on:          //
//  https://reprap.org/wiki/G-code //
//*********************************//

void arduinogcode::setfeedrate(int acceleration){
    QString msg = "G0F";
    msg= msg.append(QString::number(acceleration)).append(EOM);
    apipc_sc.send(msg);
}

void arduinogcode::goHome(bool x, bool y, bool z){
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

void arduinogcode::moveAsync(float x, float y, float z){

    QString msg = "G1";
    msg.append("X").append(QString::number(x));
    msg.append("Y").append(QString::number(y));
    msg.append("Z").append(QString::number(z));
    msg.append(this->EOM);
    apipc_sc.send(msg);
}

void arduinogcode::moveToXSync(float x_value){
    QString msg= "G0";
    apipc_sc.send(msg.append("X").append(QString::number(x_value,'f',2)).append(EOM));
}

void arduinogcode::moveToYSync(float y_value){
    QString msg= "G0";
    apipc_sc.send(msg.append("Y").append(QString::number(y_value,'f',2)).append(EOM));
}

void arduinogcode::moveToZSync(float z_value){
    QString msg= "G0";
    //this->setabsoluepositioning;
    apipc_sc.send(msg.append("Z").append(QString::number(z_value,'f',2)).append(EOM));
}

void arduinogcode::setabsoluepositioning(){
    QString msg = "G90";
    apipc_sc.send(msg.append(EOM));

}

void arduinogcode::setrelativepositioning(){
    QString msg = "G91";
    apipc_sc.send(msg.append(EOM));
}

std::vector<float> arduinogcode::getcurrentpos(){
//  apipc_sc.sp.clear();
    QByteArray answer;
    QString msg = "M114";
    answer=apipc_sc.sendAndReceive(msg,EOM);
    std::vector<float> coors;
    //split strings by :
    QString s(answer);
    QStringList resultStrings =  s.split(':');
    // get floating point number regularexpressions
    QRegExp xRegExp("(-?\\d+(?:[\\.,]\\d+(?:e\\d+)?)?)");
    for (int i=1;i<4;i++){
         xRegExp.indexIn( resultStrings.at(i));
         QStringList xList = xRegExp.capturedTexts();
         try {
             coors.push_back( static_cast<float>(xList.begin()->toFloat()));
         }
         catch (...)
         {
             std::cout << "extractiing cooordinates error error " << std::endl;
            //error extracting coord
         }
    }
    return coors;
}

void arduinogcode::MoveToXYZSync(std::vector<float> coords){
    setabsoluepositioning();
    moveToXSync(coords.at(0));  //X
    moveToZSync(coords.at(2));  //Z
    moveToYSync(coords.at(1));  //Y
}


void arduinogcode::setPipetteposition(){



}




