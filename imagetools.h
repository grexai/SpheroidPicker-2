#ifndef IMAGING_H
#define IMAGING_H
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>




class imagetools
{
public:
    imagetools(){}

    ~imagetools(){}

    void setimagewidth(cv::VideoCapture& device,int imwidth);

    void setimageheight(cv::VideoCapture& device, int imheight);

    void setframerate(cv::VideoCapture& device,int reqframerate);

    void setgain(cv::VideoCapture& device,float gain);

    void setexposuretime(cv::VideoCapture& device, float exptime);

 //   void cameraloop();

//    void SpawnThreadAndLaunch();

    cv::Mat getframe();

    cv::Mat setframe(cv::Mat &input);

    void getCameraframe();

    cv::VideoCapture getCamera();

    void setvideodevice(int devid);

    void rmvideodevice();

    cv::Mat threshimage(cv::Mat input,int thresh);

    int getSphCoors();

    bool iscameraopen=false;

protected:

    cv::VideoCapture* camera=nullptr;
private:

    cv::Mat frame;

};

#endif // IMAGING_H
