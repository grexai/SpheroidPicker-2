#ifndef AUTO_METHODS_H
#define AUTO_METHODS_H
#include "controller.h"
#include <vector>
class auto_methods : public controller
{
public:
    auto_methods();
    ~auto_methods();
    void pickup_sph(float pulse_value,float pulse_time,std::vector<float> objpos);
};

#endif // AUTO_METHODS_H
