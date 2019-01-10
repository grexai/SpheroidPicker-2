#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "imagetools.h"
#include <arduinopressurecontroller.h>
#include <PipetteController.h>
#include <stagecontroller.h>
#include <calibratedialog.h>
#include <comps.h>
#include <QString>
#include <QSerialPort>

class controller
{
public:
    controller(){}
    ~controller(){}
    void getTmatrix();
    void getimg_c_p();
    void getpipette_c_p();
    void getcenters();
    void CalculateTransformMatrix();

    // pressurecontroller stuff

    bool connect_pressure_controller();

    void request_pressure(const float pressure);

    void request_atm();

    void vaccum_pulse(const float vacuum,float time);

    float get_pressure();

    bool connect_pipette_controller();

    void pipette_movex_sync(const float x);

    void pipette_movey_sync(const float y);

    void pipette_movez_sync(const float z);

    void pipette_move_async(const std::vector<float>coords);

    void pipette_home();

    void pipette_home_x();

    void pipette_home_y();

    void pipette_home_z();

    std::vector<float> pipette_get_coordinates();

    bool connect_tango_stage();

    void center_stage();

    void stage_move_x_async(int x);

    void stage_move_y_async(int y);

    void stage_move_to_x_async( int x);

    void stage_move_to_y_async( int y);

    void stage_set_speed(const float speed);

    int stage_get_x_speed();

    int stage_get_y_speed();

    std::vector<int> stage_get_speed();

    int stage_get_x_coords();

    int stage_get_y_coords();

    std::vector<int> stage_get_coords();

protected:
    cv::Mat TM;
    cv::Mat* img_c_p;
    cv::Mat pipette_c_p;
    centers centers;
    //serials
    QSerialPort QSP_apc;
    QSerialPort QSP_apipc;
    //ardino drivers
    pipetteController* apipc;
    arduinopressurecontroller* apc;
    //Stage
    ahm::Unit* pStageUnit= nullptr;
    ahm::Unit* pRootUnit= nullptr;
    Stage *stage= nullptr;

};

#endif // CONTROLLER_H
