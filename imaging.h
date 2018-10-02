#ifndef IMAGING_H
#define IMAGING_H
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>




class imaging
{
public:
    imaging();
//    ~imaging();


 //   void cameraloop();
//    void SpawnThreadAndLaunch();
    cv::Mat getframe();
    cv::Mat setframe(cv::Mat &input);
    cv::VideoCapture getCamera();
    void setvideodevice(int devn);

private:
    cv::Mat frame;


};

#endif // IMAGING_H
