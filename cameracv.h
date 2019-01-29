#ifndef CAMERACV_H
#define CAMERACV_H
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include <opencv2\core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <thread>
#include <QString>
#include <QtCore>

class CameraCV
{
public:

    CameraCV(int devid);

    ~CameraCV();

    cv::Mat* getframe();

    void setframe(cv::Mat &input);

    cv::Mat* getdisplayframe();

    void setvideodevice(int devid);

    void resetvideodevice();

    void rmvideodevice();

    QSharedPointer<cv::Mat> get_current_frm();

    void getCameraframe();

    void spawnCameraLoop();

    cv::VideoCapture* getCamera();

    void freeframe();

    void setimagewidth(float& imwidth);

    void setimageheight(float& imheight);

    void setframerate(int reqframerate);

    void setgain(float gain);

    void setexposuretime( float exptime);

    bool is_camera_open();

    bool is_loop=false;
protected:
    std::atomic_bool mActive;
    QSharedPointer<cv::Mat> currentFrame;
    std::thread mBGThread;
    std::mutex mFrameBufferMutex;
    cv::VideoCapture* camera=nullptr;
    cv::Mat* frame= nullptr;
    cv::Mat* dispfrm = nullptr;
};

#endif // CAMERACV_H
