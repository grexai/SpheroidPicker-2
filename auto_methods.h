#ifndef AUTO_METHODS_H
#define AUTO_METHODS_H
#include "controller.h"
#include "cameracv.h"
#include "deeplearning.h"
#include "imagetools.h"
#include <vector>
class auto_methods
{
protected:
    controller *m_controller;
    CameraCV *m_camera;
    i_deeplearning* dl;
public:
    auto_methods(controller *p_ctrl,CameraCV *p_camera);
    ~auto_methods();
    void pickup_sph(float pulse_value,float pulse_time,std::vector<float> objpos);
    void scan_sample(std::atomic_bool &m_s_t_acitive);
};

#endif // AUTO_METHODS_H
