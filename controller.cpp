#include "controller.h"


// Pressure controller

bool controller::connect_pressure_controller()
{
    QString port = QString::fromStdString(propreader->cfg.port_pressurecontrooler);
    QTextStream(stdout)<< port<< endl;
   //QTextStream(stdout) << *propreader->cfg->port_pressurecontrooler<< endl;
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
    QString port = QString::fromStdString(propreader->cfg.port_pipette);
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
            if(pRootUnit)
            {
                pStageUnit = findUnit(pRootUnit, ahm::MICROSCOPE_STAGE);
                stage = new Stage(pStageUnit) ;
                if(!pStageUnit)
                {
                    QTextStream(stdout) << "stage_sample: no stage found!" << endl;
                    return false;
                }


            }
            else
            {
                QTextStream(stdout) << "there is something strange: no unit was delivered" << endl;
                return false;
            }
                return true;
            }
    }catch (ahm::Exception & ex) {
        // a hardware model exception occured:
        std::cout << "a hardware model exception occurred: error code: " << ex.errorClass() << ", error code: " << ex.errorCode() << ", text: " << ex.errorText() << endl;
        return false;
    }
}

void controller::stage_move_to_x_async(const int x)
{
    stage->XAxis().moveToAsync((iop::int32)x);
}

void controller::stage_move_to_y_async(const int y)
{
    stage->YAxis().moveToAsync(((iop::int32)y));
}


void controller::stage_move_to_x_sync(const int x)
{
    stage->XAxis().moveTo((iop::int32)x);
}

void controller::stage_move_to_y_sync(const int y)
{
    stage->YAxis().moveTo(((iop::int32)y));
}



void controller::stage_set_speed(const float speed)
{
    stage->XAxis().setCurrentSpeed((iop::int32)speed);
    stage->YAxis().setCurrentSpeed((iop::int32)speed);
}

int controller::stage_get_x_speed()
{
    return int(stage->XAxis().getCurrentSpeed());
}

int controller::stage_get_y_speed()
{
    return int(stage->YAxis().getCurrentSpeed());
}

std::vector<int> controller::stage_get_speed()
{
    std::vector<int> speedvec;
    speedvec.push_back(this->stage_get_x_speed());
    speedvec.push_back(this->stage_get_y_speed());
    return speedvec;
}

int controller::stage_get_x_coords()
{
    iop::int32 xp1= stage->XAxis().getCurrentPosition();
    return int(stage->XAxis().getCurrentPosition());
}

int controller::stage_get_y_coords()
{
    return int(stage->YAxis().getCurrentPosition());
}

std::vector<int>  controller::stage_get_coords()
{
    std::vector<int> coords;
    coords.push_back(this->stage_get_x_coords());
    coords.push_back(this->stage_get_y_coords());
    return coords;
}


void controller::stage_move_x_async(const int x)
{
   int x0 = this->stage_get_x_coords();
   this->stage_move_to_x_async(x0+x);
}

void controller::stage_move_y_async(const int y)
{
    int y0 = this->stage_get_y_coords();
    this->stage_move_to_y_async(y0+y);
}

int controller::stage_get_x_min_pos()
{
   return int(stage->XAxis().getMinPosition());
}

int controller::stage_get_x_max_pos()
{
   return int(stage->XAxis().getMaxPosition());
}

int controller::stage_get_y_min_pos()
{
    return int(stage->YAxis().getMinPosition());
}

int controller::stage_get_y_max_pos()
{
   return int(stage->YAxis().getMaxPosition());
}

void controller::stage_go_center()
{
    iop::int32 x0 = stage->XAxis().getMinPosition();
    iop::int32 x1 = stage->XAxis().getMaxPosition();

    iop::int32 y0 = stage->YAxis().getMinPosition();
    iop::int32 y1 = stage->YAxis().getMaxPosition();

    iop::int32 x = x0 + (iop::int32) ( (x1-x0)/2.0);
    iop::int32 y = y0 + (iop::int32) ( (y1-y0)/2.0);
    stage->moveToAsync(x,y, false);
}

void controller::stage_run_iniciatlions()
{
    stage->printWhatIsSupported();

    std::cout << "current position: " << stage->XAxis().toMicrons(stage->XAxis().getCurrentPosition()) << ";" << stage->YAxis().toMicrons(stage->YAxis().getCurrentPosition()) << std::endl;
    iop::int32 x0 = stage->XAxis().getMinPosition();
    iop::int32 x1 = stage->XAxis().getMaxPosition();

    iop::int32 y0 = stage->YAxis().getMinPosition();
    iop::int32 y1 = stage->YAxis().getMaxPosition();
    std::cout << "moving to minimum" << std::endl;
    stage->moveToAsync(x0, y0, true); // move to min corner and wait

    std::cout << "current position: " << stage->XAxis().toMicrons(stage->XAxis().getCurrentPosition()) << ";" << stage->YAxis().toMicrons(stage->YAxis().getCurrentPosition()) << std::endl;

    iop::int32 x = x0 + (iop::int32) ((x1 - x0) / 2.0);
    iop::int32 y = y0 + (iop::int32) ((y1 - y0) / 2.0);

    std::cout << "moving to center " << x << "," << y << std::endl;


    Stage::PositionRecorder recorder;

    stage->subscribe(&recorder);

    DWORD t0 = ::GetTickCount();

    stage->moveToAsync(x, y, false);

 //   ::Sleep(1000);

    Stage::PositionRecorder::Records records;
    recorder.getRecords(records, true);

    bool flagMoving = stage->isMoving();

    std::cout << "after one second the stage is " << (flagMoving ? "still" : "not") << " moving" << std::endl;
    std::cout << "in this time " << records.size() << " events were received" << std::endl;

    for (size_t i = 0; i<records.size(); i++) {
        std::cout << "[" << i << "] - ";
        stage->print(std::cout, records[i], t0);
        std::cout << std::endl;
    }

    iop::int32 maxSpeedX = stage->XAxis().getMaxSpeed();
    iop::int32 curSpeedX = stage->XAxis().getCurrentSpeed();

    std::cout << "maximum speed is " << stage->XAxis().toMicronsPerSecond(maxSpeedX) << " microns/sec" << std::endl;
    std::cout << "current speed is " << stage->XAxis().toMicronsPerSecond(curSpeedX) << " microns/sec" << std::endl;

    iop::int32 oldSpeedX = curSpeedX; // save original speed

    curSpeedX = curSpeedX / 2;

    std::cout << "setting half speed" << std::endl;
    stage->XAxis().setCurrentSpeed(curSpeedX);
    std::cout << "current speed is " << stage->XAxis().toMicronsPerSecond(stage->XAxis().getCurrentSpeed()) << " microns/sec" << std::endl;

    // lets move back to min

    std::cout << "now moving X back to minimum.." << std::endl;

    t0 = ::GetTickCount();

    ahm::AsyncResult *pAsyncResult = stage->XAxis().moveToAsync(x0);
    if (pAsyncResult) {// discard AsyncResult
        pAsyncResult->dispose();
        pAsyncResult = 0;
    }
//    ::Sleep(1000);

    recorder.getRecords(records, true);

    flagMoving = stage->isMoving();

    std::cout << "after one second the stage is " << (flagMoving ? "still" : "not") << " moving" << std::endl;
    std::cout << "in this time " << records.size() << " events were received" << std::endl;

    for (size_t i = 0; i<records.size(); i++) {
        std::cout << "[" << i << "] - ";
        stage->print(std::cout, records[i], t0);
        std::cout << std::endl;
    }

    stage->XAxis().setCurrentSpeed(oldSpeedX); // restore original speed
    std::cout << "current speed is " << stage->XAxis().toMicronsPerSecond(stage->XAxis().getCurrentSpeed()) << " microns/sec" << std::endl;
}

bool controller::connect_microscope_unit()
{
    std::map<std::string, std::string> settings;
    propreader = new propertyreader;
    propreader->read_settings("config.txt",settings);
    propreader->apply_settings(settings);

    bool apipc=this->connect_pipette_controller();
    QTextStream(stdout) << "pipette unit succesfully connected"<< endl;

    bool apc = this->connect_pressure_controller();
    QTextStream(stdout) << "pressurecontroller unit succesfully connected"<< endl;

    bool ts =this->connect_tango_stage();
    QTextStream(stdout) << "stage succesfully connected"<< endl;

    if (apipc && apc && ts)
    {
        QTextStream(stdout) << "all microscope unit succesfully connected"<< endl;
        return true;
    }else
    return false;
}
