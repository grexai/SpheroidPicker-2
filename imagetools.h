#ifndef IMAGING_H
#define IMAGING_H
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include <opencv2\core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <types.h>
#include <QString>

class imagetools
{
public:
    imagetools(){}

    ~imagetools(){}

    void setimagewidth(float& imwidth);

    void setimageheight(float& imheight);

    void setframerate(int reqframerate);

    void setgain(float gain);

    void setexposuretime( float exptime);

 //   void cameraloop();

//    void SpawnThreadAndLaunch();

    cv::Mat* getframe();

    cv::Mat* getdisplayframe();

    void setframe(cv::Mat &input);

    void freeframe();

    void getCameraframe();

    cv::VideoCapture* getCamera();

    void setvideodevice(int devid);

    void resetvideodevice();

    void rmvideodevice();

    void addPointToImage(cv::Point point);

    cv::Mat threshimage(cv::Mat input,int thresh);

    int2coors getSphCoors(cv::Mat &img);

    bool iscameraopen=false;

    void saveImg(cv::Mat &outimg, std::string outname);

protected:
    cv::VideoCapture* camera=nullptr;
    cv::Mat* frame= nullptr;
    cv::Mat* dispfrm = nullptr;
    cv::Mat* temp = nullptr;
private:


};

#endif // IMAGING_H
