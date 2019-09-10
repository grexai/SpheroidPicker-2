#include "controller.h"
#include <comps.h>

// Pressure controller

bool controller::connect_pressure_controller(std::string& port)
{
    QString p1= QString::fromStdString(port);
  //  QString port = QString::fromStdString(propreader->cfg.port_pressurecontrooler);
    apc = new arduinopressurecontroller(QSP_apc,p1);
    //arduinopressurecontroller sd(QSP_apc,p1);
    if (apc->isconnected == true)
    {

       QTextStream(stdout)<< "Pressure controller connected"<< endl;
       return true;
    }
    else{
        QTextStream(stdout)<< "Could not connect to pressure controller"<< endl;
        return false;
    }
}

void controller::request_pressure(const float pressure)
{
    apc->requestPressure(pressure);
}

void controller::request_atm()
{
    if(apc == nullptr){return;}
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

void controller::req_pressure_loop()
{
    while (1)
    {
        QSharedPointer<float> temp(new float);
        *temp = this->get_pressure();
        std::unique_lock<std::mutex> pressurevaluelock(m_pressurevaluebuff);  //csak most lockoljuk a frame buffert az utolso pillanatban par microsecig, amig a swap megtortenik
        std::swap(current_pressure,temp);
        pressurevaluelock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

QSharedPointer<float> controller::get_current_pressure(){
    std::unique_lock<std::mutex> pressurevaluelock(m_pressurevaluebuff);
    return this->current_pressure;
}

void controller::spawn_pressure_thread()
{
  //  m_pthread = QThread::create([this]{req_pressure_loop();});
    //m_pthread->setObjectName("pressure");
   // m_pthread->start();
 //   std::this_thread::sleep_for(std::chrono::milliseconds(300));
  //  connect(thread, &QThread::started, gui, &Gui::threadHasStarted);
 //   thread->start();
//    m_get_pressure_t =std::thread(&controller::req_pressure_loop, this);
 //   std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

class pipette_driver: public arduinogcode{
public:
    void movex(){

    }

};

// INTERFACE TEST
//pipette_driver* pd(arduinogcode apic);
class i_stagecontroll {
public:
    i_stagecontroll(){}
    virtual ~i_stagecontroll(){}
    virtual void move_x() = 0;

};

class microscope  : public arduinogcode, public virtual i_stagecontroll{
public:
    void move_x() override{

    }
};

class TangoStage : public virtual i_stagecontroll{
public:
    void move_x() override{

    }
};

//i_stagecontroll* asd  = new TangoStage();
//asd->move_x
    //i_stagecontroll* asd  = new TangoStage();
    //asd->move_x();

// Pipette
bool controller::connect_pipette_controller(std::string& port)
{

    QString p1 = QString::fromStdString(port);
    apipc = new arduinogcode(QSP_apipc,p1);
    if (apipc->isconnected == true)
    {
       QTextStream(stdout)<< "Pipette connected!"<< endl;
       return true;
    }
    else{
        QTextStream(stdout)<< "Could not connect to pipette!"<< endl;
        return false;
    }
}

void controller::pipette_movex_sync(const float x)
{
    if(apipc == nullptr){return;}
    apipc->setrelativepositioning();
    apipc->moveToXSync(x);
}

void controller::pipette_movey_sync(const float y)
{
    if(apipc == nullptr){return;}
    apipc->setrelativepositioning();
    apipc->moveToYSync(y);
}

void controller::pipette_movez_sync(const float z)
{

    apipc->setrelativepositioning();
    apipc->moveToZSync(z);
}

void controller::pipette_extrude_relative(const float e)
{

    apipc->extrude_relative(e);
}

void controller::pipette_move(const std::vector<float> coords)
{
    apipc->setabsoluepositioning();
    apipc->moveToXSync(coords.at(0));
    apipc->moveToYSync(coords.at(1));
    apipc->moveToZSync(coords.at(2));
}

void controller::pipette_move_async(const std::vector<float> coords)
{
    apipc->setabsoluepositioning();
    apipc->moveAsync(coords.at(0),coords.at(1),coords.at(2));
}

void controller::pipette_set_speed(const int speed)
{
    apipc->setfeedrate(speed);
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



void controller::pipette_move_to_img_coordinates(std::vector<float> coords){
    cv::Mat pipc = calconimgpipettecoors(TM,coords,m_centers.img,m_centers.pipette);
    QTextStream(stdout) <<"calculated pipette coords; "<<"X: "<<pipc.at<float>(0,0)<<" Y: "<< pipc.at<float>(0,1) <<" Z:" << pipc.at<float>(0,2) << endl;
    this->pipette_move(pipc);
}

void controller::pipette_calc_TM(std::vector<float>*pos1,std::vector<float>*pos2 , std::vector<float>*pos3){

    float ipdata2[6] = {960.0f,100.0f,1820.0f,100.0f,980.0f,980.0f};
    cv::Mat cip = cv::Mat(2,3,CV_32F,ipdata2);

    /****************************************************
    // image coordinates 2 x 3 matrix image
    //expected format

      Center point
         ||
         ||
        \  /
         \/

       | x1  x2  x3 |
       | y1  y2  y3 |

    *************************************************/
    // pushing saved pipette positions to a Matrix
    cv::Mat pipette_mat;
    pipette_mat.push_back(*pos1);
    pipette_mat.push_back(*pos2);
    pipette_mat.push_back(*pos3);
    //std::cout << pipette_mat << "before reshape"<< std::endl;

    /*******************************************
    // converting to pipette coordinates a 3 x 3 matrix
      Center point
         ||
         ||
        \  /
         \/
       | x1  x2  x3 |
       | y1  y2  y3 |
       | z1  z2  z3 |
    *********************************************/

    pipette_mat =  pipette_mat.reshape(0,3);
    cv::transpose(pipette_mat,pipette_mat);

//    std::cout << pipette_mat<< "pipette_mat"<< std::endl;
    TM = calcTMatrix(pipette_mat,cip,m_centers);
    QTextStream(stdout)<<"done" << endl;

}




// STAGE

bool controller::connect_tango_stage(){
    try
    {
        if(theHardwareModel())
        {
            iop::string name = "";
            pRootUnit = theHardwareModel()->getUnit(name);
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
                QTextStream(stdout)<< "Stage controller connected"<< endl;
                return true;
            }
    }catch (ahm::Exception & ex) {
        // a hardware model exception occured:
        std::cout << "a hardware model exception occurred: error code: " << ex.errorClass() << ", error code: " << ex.errorCode() << ", text: " << ex.errorText() << std::endl;
        return false;
    }
    return false;
}

void controller::stage_move_to_x_async(const int x)
{
    stage->XAxis().moveToAsync(static_cast<iop::int32>(x));
}

void controller::stage_move_to_y_async(const int y)
{
    stage->YAxis().moveToAsync(static_cast<iop::int32>(y));
}

void controller::stage_move_to_x_sync(const int x)
{
    stage->XAxis().moveTo(static_cast<iop::int32>(x));
}

void controller::stage_move_to_y_sync(const int y)
{
    stage->YAxis().moveTo(static_cast<iop::int32>(y));
}

void controller::stage_set_speed(const float speed)
{
    stage->XAxis().setCurrentSpeed(static_cast<iop::int32>(speed));
    stage->YAxis().setCurrentSpeed(static_cast<iop::int32>(speed));
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

    iop::int32 x = x0 + static_cast<iop::int32>( (x1-x0)/2.0);
    iop::int32 y = y0 + static_cast<iop::int32>( (y1-y0)/2.0);
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

    iop::int32 x = x0 + static_cast<iop::int32>(((x1 - x0) / 2.0) );
    iop::int32 y = y0 + static_cast<iop::int32>( ((y1 - y0) / 2.0));

    std::cout << "moving to center " << x << "," << y << std::endl;

    Stage::PositionRecorder recorder;

    stage->subscribe(&recorder);

    DWORD t0 = ::GetTickCount();

    stage->moveToAsync(x, y, false);

    std::this_thread::sleep_for(std::chrono::microseconds(1000) );
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
    std::this_thread::sleep_for(std::chrono::microseconds(1000) );
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

bool controller::connect_microscope_unit(std::string &pa, std::string &pc)
{
    /*
    std::map<std::string, std::string> settings;
    propreader = new propertyreader;
    propreader->read_settings("config.txt",settings);
    propreader->apply_settings(settings);*/
    this->connect_pipette_controller(pa) ;
    this->connect_pressure_controller(pc);
    this->connect_tango_stage();
//  this->spawn_pressure_thread();
    return true;
}

bool controller::connect_screening_microscope(){
    return false;
}

