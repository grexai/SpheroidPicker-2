#include "imaging.h"
#include <thread>


imaging::imaging()
{

}

cv::Mat imaging::getframe()
{
    return this->frame;
}


cv::Mat imaging::setframe(cv::Mat &input){
   return this->frame= input;
}

cv::VideoCapture imaging::getCamera(){
    return 1;
}

void imaging::setvideodevice(int devn){

}

