#include "auto_methods.h"

auto_methods::auto_methods()
{
}
auto_methods::~auto_methods(){}
void auto_methods::pickup_sph(float pulse_value,float pulse_time,std::vector<float> objpos){
    //   QTextStream(stdout) << "pciking up" << endl;
       this->pipette_move_to_img_coordinates(objpos);
       std::this_thread::sleep_for(std::chrono::milliseconds(7000));
       this->vaccum_pulse(pulse_value,pulse_time);
       std::this_thread::sleep_for(std::chrono::milliseconds(3000));
       this->pipette_home_z();
}
