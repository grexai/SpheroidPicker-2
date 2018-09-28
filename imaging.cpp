#include "imaging.h"
#include <thread>

imaging::imaging()
{

}

Mat imaging::getframe()
{
    return this->frame;
}


Mat imaging::setframe(Mat &input){
   return this->frame= input;
}

VideoCapture imaging::getCamera(){
    return 1;
}

void imaging::setvideodevice(int devn){

}

