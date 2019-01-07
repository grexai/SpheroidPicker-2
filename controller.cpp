#include "controller.h"


// Pressure controller

bool controller::connect_pressure_controller()
{
    QString port = "COM5";
    apc = new arduinopressurecontroller(QSP_apc,port);

    if (apc->isconnected)
    {
       return true;
       // show_currentpressure();
    }
    else{
        return false;
    }
}

void controller::request_pressure(const float pressure)
{
    apc->requestPressure(pressure);
}

void controller::request_atm()
{
    this->request_pressure(0.0f);
}

void controller::vaccum_pulse(const float vacuum, float time )
{
    apc->breakIn(vacuum, time);
}

float controller::get_pressure()
{
    return apc->getPipettePressure();
}

// Pipette

bool controller::connect_pipette_controller()
{
    QString port = "COM22";
    apipc = new pipetteController(QSP_apipc,port);
    if (apipc->isconnected)
    {
       return true;
       // show_currentpressure();
    }
    else{
        return false;
    }

}

void controller::pipette_movex_sync(const float x)
{
    apipc->setrelativepositioning();
    apipc->moveToXSync(x);
}

void controller::pipette_movey_sync(const float y)
{
    apipc->setrelativepositioning();
    apipc->moveToYSync(y);
}

void controller::pipette_movez_sync(const float z)
{
    apipc->setrelativepositioning();
    apipc->moveToZSync(z);
}

void controller::pipette_move_async(const std::vector<float> coords){
    apipc->setabsoluepositioning();
    apipc->moveAsync(coords.at(0),coords.at(1),coords.at(2));
}

void controller::pipette_home(){

    apipc->goHome(false,false,false);

}

void controller::pipette_home_x()
{
    apipc->goHome(true,false,false);
}

void controller::pipette_home_y()
{
     apipc->goHome(false,true,false);
}

void controller::pipette_home_z()
{
    apipc->goHome(false,false,true);
}

std::vector<float> controller::pipette_get_coordinates(){
   return apipc->getcurrentpos();
}



// STAGE

bool controller::connect_tango_stage(){
    try
    {
        if(theHardwareModel())
        {
            iop::string name = "";
            pRootUnit = theHardwareModel()->getUnit("");
            QTextStream(stdout) << pRootUnit << endl;
            if(pRootUnit){
                pStageUnit = findUnit(pRootUnit, ahm::MICROSCOPE_STAGE);
                stage = new Stage(pStageUnit) ;
                if(!pStageUnit){
                    QTextStream(stdout) << "yo program  has succesfully crashed. is not works" << endl;
                    return false;
                }
                //  return false;
                //iop::int32 curSpeedX = stage->XAxis().getCurrentSpeed();
                //ui->s_speed_spinbox->setValue(float(curSpeedX));
                }
                return true;
            }
    }catch (ahm::Exception & ex) {
        // a hardware model exception occured:
        std::cout << "a hardware model exception occurred: error code: " << ex.errorClass() << ", error code: " << ex.errorCode() << ", text: " << ex.errorText() << endl;
        return false;
    }
}

void controller::stage_move_x_async(const int x)
{
    stage->XAxis().moveAsync((iop::int32)x);
}

void controller::stage_move_y_async(const int y)
{
    stage->YAxis().moveAsync((iop::int32)y);
}

void controller::stage_move_to_x_async(const int x)
{
    stage->XAxis().moveToAsync((iop::int32)x);
}

void controller::stage_move_to_y_async(const int y)
{
    stage->YAxis().moveToAsync((iop::int32)y);
}

void controller::stage_set_speed(const float speed)
{
    stage->XAxis().setCurrentSpeed((iop::int32)speed);
    stage->YAxis().setCurrentSpeed((iop::int32)speed);
}


