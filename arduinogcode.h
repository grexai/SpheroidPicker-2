#ifndef PIPETTERCONTROLLER_H
#define PIPETTERCONTROLLER_H
#include "serialcom.h"
#include "types.h"
#include <exception>
#include <iostream>
#include <QObject>

class arduinogcode: public QObject
{
     Q_OBJECT
signals:
    void signal_process_qui();


public:
    arduinogcode(QSerialPort& i_qsp,QString& nport,  QObject* parent = nullptr): QObject(parent), apipc_sc(i_qsp){
        isconnected = this->apipc_sc.openport(nport);

        auto res = connect(&apipc_sc,SIGNAL(signal_process_qui()),this,SIGNAL(signal_process_qui()));

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

    void extrude(float e_value, int speed);

    void extrude_relative(float e_value, int speed);

    void SyncronizedMoveToX(float x_value);

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

    void syncronised_move_X(float val);

    void syncronised_move_Y(float val);

    void syncronised_move_Z(float val);

    void syncronised_move_E(float val);

    std::mutex ardudinogcode_mutex;

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
