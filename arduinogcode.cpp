#include "arduinogcode.h"


//*********************************//
//  Controlling based on:          //
//  https://reprap.org/wiki/G-code //
//*********************************//


// Sets the speed in the (in current units-per-second)
void arduinogcode::setfeedrate(int speed){
    QString msg = "G0F";
    msg= msg.append(QString::number(speed)).append(EOM);
    apipc_sc.send(msg);
}

//Set the max acceleration for all axes (in current units-per-second squared)
void arduinogcode::setacceleration(int acc){
    QString msg = "M201";
    msg= msg.append(QString::number(acc)).append(EOM);
    apipc_sc.send(msg);
}
//Unusable yet
void arduinogcode::setjerk(int jerk){
    QString msg = "M205";
    msg= msg.append(QString::number(jerk)).append(EOM);
    apipc_sc.send(msg);
}

// Sends one or more axis to the initial position
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

//Asynchronous movements
void arduinogcode::moveAsync(float x, float y, float z){
    QString msg = "G1";
    msg.append("X").append(QString::number(x));
    msg.append("Y").append(QString::number(y));
    msg.append("Z").append(QString::number(z));
    msg.append(this->EOM);
    apipc_sc.send(msg);
}


//Synchronous movements
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

// set the GCODE interpreter to ABSOLUTE POSIITING MODE
// sends to desired coordinates
void arduinogcode::setabsoluepositioning(){
    QString msg = "G90";
    apipc_sc.send(msg.append(EOM));

}

// set the GCODE interpreter to RELATIVE POSTIONING MODE
// incrementing coordinates
void arduinogcode::setrelativepositioning(){
    QString msg = "G91";
    apipc_sc.send(msg.append(EOM));
}

// extracting the coordinates from the answer of GCODE interpreter
//                                            0,1,2
// and returns with coordinate vector<float> (x,y,z)

std::vector<float> arduinogcode::getcurrentpos()
{
    std::vector<float> coors;
    try
    {
        QString msg = "M114";
        QByteArray answer=apipc_sc.sendAndReceive(msg,EOM);

        QString s(answer);
        //split strings by ":"
        QStringList resultStrings =  s.split(':');
        // get floating point number regularexpressions
        QRegExp xRegExp("(-?\\d+(?:[\\.,]\\d+(?:e\\d+)?)?)");
        // experiment to find echo...
        // tries to get the coordinates
        //recall the function if echo thrown, else error
        QRegExp findecho("(echo)");
      //  QString asd("echo: asdasdsad");
        findecho.indexIn(answer);
        QStringList test = findecho.capturedTexts();
    // auto t21= ;
        std::cout << "find " <<test.begin()->toStdString()<<   "asdsad"<<test.at(0).isEmpty()<< std::endl;
        if(test.at(0).isEmpty()==true)
        {
            for (int i=1;i<4;i++)
            {
            xRegExp.indexIn( resultStrings.at(i));
            QStringList xList = xRegExp.capturedTexts();
            try
            {
            coors.push_back( static_cast<float>(xList.begin()->toFloat()));
            }
            catch (...)
            {
            std::cout << "extractiing cooordinates error" << std::endl;
            }
        }
        return coors;
    }
    else if(test.at(0).isEmpty()==false && m_counter < 2 )
    {
        m_counter ++;
        std::cout << "extractiing cooordinates error retrial..." << std::endl;
        return this->getcurrentpos();
    }
    else
    {
        throw ardinogcodeexeption;
    }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << '\n';
        m_counter = 0;
        return coors = {0,0,0};
    }
}


//Moves the machine to an XYZ coordinate with syncronized moving, coordinates sent
// one by one

void arduinogcode::MoveToXYZSync(std::vector<float> coords)
{
    setabsoluepositioning();
    moveToXSync(coords.at(0));  //X
    moveToZSync(coords.at(2));  //Z
    moveToYSync(coords.at(1));  //Y
}

// A function will override manually the machines position

void arduinogcode::setPipetteposition()
{

}




