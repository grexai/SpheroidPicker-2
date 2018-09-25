#ifndef IMAGING_H
#define IMAGING_H
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>

using namespace cv;


class imaging
{
public:
    imaging();
    void cameraloop();
private:
    VideoCapture cap;
    bool cam = false;


};

#endif // IMAGING_H
