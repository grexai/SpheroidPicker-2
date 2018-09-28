#ifndef IMAGING_H
#define IMAGING_H
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>

using namespace cv;


class imaging
{
public:
    imaging();
//    ~imaging();


 //   void cameraloop();
//    void SpawnThreadAndLaunch();
    Mat getframe();
    Mat setframe(Mat &input);
    VideoCapture getCamera();
    void setvideodevice(int devn);

private:
    Mat frame;


};

#endif // IMAGING_H
