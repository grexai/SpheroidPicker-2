#include "imagetools.h"
#include <thread>



cv::Mat imagetools::getframe()
{
    return this->frame;
}


cv::Mat imagetools::setframe(cv::Mat &input){
   return this->frame= input;
}

cv::VideoCapture imagetools::getCamera(){
    return 1;
}

void imagetools::setvideodevice(int devn){

}

//void imagetools::
