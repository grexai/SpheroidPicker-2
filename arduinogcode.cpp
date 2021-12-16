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

void arduinogcode::SyncronizedMoveToX(float x_value)
{
    QString msg= "G0";
    apipc_sc.sendAndReceive(msg.append("X").append(QString::number(x_value,'f',2)), EOM);


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


void arduinogcode::moveToYSync(float y_value)
{
    // error check
    QString msg= "G0";
    apipc_sc.send(msg.append("Y").append(QString::number(y_value,'f',2)).append(EOM));
}

void arduinogcode::moveToZSync(float z_value)
{
    QString msg= "G0";
    //this->setabsoluepositioning;
    apipc_sc.send(msg.append("Z").append(QString::number(z_value,'f',2)).append(EOM));
}

void arduinogcode::extrude(float e_value)
{
    QString msg= "G0";
    apipc_sc.send(msg.append("E").append(QString::number(e_value,'f',2)).append(EOM));
}

void arduinogcode::extrude_relative(float e_value)
{
    QString msg= "M83";
    apipc_sc.send(msg.append(EOM));
    this->extrude(e_value);

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
    std::vector<float> coors(3,0.0f);
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
        QRegExp findok("(ok)");
        findok.indexIn(answer);
        QStringList oks = findok.capturedTexts();
        if(!(oks.size()>3)){
            QRegExp findecho("(echo)");

            findecho.indexIn(answer);
            QStringList test = findecho.capturedTexts();
    // auto t21= ;
            std::cout << "find " <<test.begin()->toStdString()<<   "asdsad"<<test.at(0).isEmpty()<< std::endl;
            if(test.at(0).isEmpty()&& resultStrings.size()>2)
            {
                std::cout<< "idejon-e"<< std::endl;
                for (int i=1;i<4;i++)
                {
                    xRegExp.indexIn( resultStrings.at(i));
                    QStringList xList = xRegExp.capturedTexts();
                    try
                    {
                    coors[i-1]=( static_cast<float>(xList.begin()->toFloat()));
                    }
                    catch (...)
                    {
                    std::cout << "extractiing cooordinates error1" << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));
                    return this->getcurrentpos();
                    }
                }
            }
            else if(!test.at(0).isEmpty() && m_counter < 2 )
            {
                m_counter ++;
                std::cout << "extractiing cooordinates error retrial..." << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                return this->getcurrentpos();
            }
        }
        else
        {
            m_counter ++;
            std::cout << "extractiing cooordinates error retrial..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            return this->getcurrentpos();
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << '\n';
        m_counter = 0;
        return coors;
    }
    return coors;
}


//Moves the machine to an XYZ coordinate with syncronized moving, coordinates sent
// one by one

void arduinogcode::MoveToXYZSync(std::vector<float> coords)
{
    setabsoluepositioning();
    // one message
    moveToXSync(coords.at(0));  //X
    moveToZSync(coords.at(2));  //Z
    moveToYSync(coords.at(1));  //Y
}

// A function will override manually the machines position

void arduinogcode::setPipetteposition()
{

}


// TEST function
void arduinogcode::syncronised_move(float val){
    QString msg= "G0";
    apipc_sc.sendAndRecive_sync(msg.append("X").append(QString::number(val,'f',2)), EOM);

}




