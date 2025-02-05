#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "imagetools.h"
#include <arduinopressurecontroller.h>
#include <arduinogcode.h>
#include <stagecontroller.h>
#include <calibratedialog.h>
#include <comps.h>
#include <QObject>
//#include <QString>
//#include <QSerialPort>
//#include <QThread>
class controller : public QObject
{
    Q_OBJECT
signals:

    void signal_process_qui();

public:

    controller(QObject* parent = nullptr ): QObject(parent)
    {


    }
    ~controller()
    {

    }
    void getTmatrix();
    void getimg_c_p();
    void getpipette_c_p();
    void getcenters();
    void CalculateTransformMatrix();

    // pressurecontroller stuff

    bool connect_pressure_controller(std::string& port);

    void request_pressure(const float pressure);

    void request_atm();

    void vaccum_pulse(const float vacuum,float time);

    float get_pressure();

    void req_pressure_loop();

    QSharedPointer<float> get_current_pressure();

    void spawn_pressure_thread();

    bool connect_pipette_controller(std::string& port);

    void pipette_movex_sync(const float x);

    void pipette_move_to_x_sync(const float x);

    void pipette_movey_sync(const float y);

    void pipette_movez_sync(const float z);

    void pipette_move_to_z_sync(const float z);

    void pipette_blocking_move_x(const float x);

    void pipette_blocking_move_y(const float y);

    void pipette_blocking_move_z(const float z);

    void pipette_blocking_move_e(const float e);

    void pipette_extrude_relative(const float e, int speed=1000);

    void pipette_move(const std::vector<float>coords);

    void pipette_move_async(const std::vector<float>coords);

    void pipette_set_speed(const int speed);

    void pipette_home();

    void pipette_xz_home();

    void pipette_home_x();

    void pipette_home_y();

    void pipette_home_z();

    std::mutex controller_pipette_mutex;

    std::vector<float> pipette_get_coordinates();

    void pipette_move_to_img_coordinates(std::vector<float> coords);

    void pipette_calc_TM(std::vector<float>*pos1,std::vector<float>*pos2 , std::vector<float>*pos3);

//TJC     void saveTM();

    bool connect_tango_stage();

    void center_stage();

    void stage_autocalibrate();

    void stage_move_x_async(int x);

    void stage_move_y_async(int y);

    void stage_move_to_x_async( int x);

    void stage_move_to_y_async( int y);

    void stage_move_to_x_sync( int x);

    void stage_move_to_y_sync( int y);

    void stage_set_speed(const int speed);

    void stage_set_acceleration(const int accel);

    int stage_get_x_speed();

    int stage_get_y_speed();

    std::vector<int> stage_get_speed();

    int stage_get_x_acceleration();

    int stage_get_y_acceleration();

    std::vector<int> stage_get_acceleration();


    int stage_get_x_coords();

    int stage_get_y_coords();

    std::vector<int> stage_get_coords();

    int stage_get_x_min_pos();

    int stage_get_x_max_pos();

    int stage_get_y_min_pos();

    int stage_get_y_max_pos();

    void stage_go_center();

    void stage_run_iniciatlions();

    bool connect_microscope_unit(std::string &pa, std::string &pc);

    bool connect_screening_microscope();
protected:
    //propertyreader* propreader=nullptr;
    //serial ports
    QSerialPort QSP_apc;
    QSerialPort QSP_apipc;
    //ardino drivers
    arduinopressurecontroller* apc = nullptr;
    arduinogcode* apipc = nullptr;
    std::mutex m_pressurevaluebuff;
    QSharedPointer<float> current_pressure;
    std::thread m_get_pressure_t;
    // pipette coordinate transformation
    cv::Mat TM;  // transformation matrix img <==> pipette coordinates
 //   cv::Mat* img_c_p;  //imgcenter points
 //   cv::Mat pipette_c_p; // pipettecenter points
    centers m_centers; // center structure
  //  cv::Mat imgc;
 //   cv::Mat pc;

    std::vector<float>* cpos1 = nullptr;

    std::vector<float>* cpos2 = nullptr;

    std::vector<float>* cpos3= nullptr;

    //Stage drivers
    QThread *m_pthread= nullptr;
    ahm::Unit* pStageUnit= nullptr;
    ahm::Unit* pRootUnit= nullptr;
    Stage *stage= nullptr;
};

#endif // CONTROLLER_H
