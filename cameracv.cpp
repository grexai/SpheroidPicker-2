#include "cameracv.h"

CameraCV::CameraCV(int devid):
mActive(true)
{
    {
        this->setvideodevice(devid);
        this->camera->open(devid);

        //+ cv::CAP_DSHOW);
    }
}

CameraCV::~CameraCV(){
    mActive = false;
    if(mBGThread.joinable())
    {
        mBGThread.join();
    }
    this->rmvideodevice();
}

cv::Mat* CameraCV::getframe()
{
    return (this->frame);
}

void CameraCV::setframe(cv::Mat &input){
    if (this->frame != nullptr){
     //   delete[] (this->frame);
        //nem kell a pointer
        this->frame->release();
    }
    this->frame = new cv::Mat();
    this->frame = &input;
}

cv::Mat* CameraCV::getdisplayframe(){
    return (this->dispfrm);

}

void CameraCV::setvideodevice(int devid){
    delete camera;
    this->camera = new cv::VideoCapture(devid);

}

bool CameraCV::is_camera_open(){
    return  this->camera->isOpened();
}

void CameraCV::resetvideodevice(){
     this->camera->set(cv::CAP_PROP_SETTINGS,0.0);
}

/*
0. CV_CAP_PROP_POS_MSEC Current position of the video file in milliseconds.
1. CV_CAP_PROP_POS_FRAMES 0-based index of the frame to be decoded/captured next.
2. CV_CAP_PROP_POS_AVI_RATIO Relative position of the video file
3. CV_CAP_PROP_FRAME_WIDTH Width of the frames in the video stream.
4. CV_CAP_PROP_FRAME_HEIGHT Height of the frames in the video stream.
5. CV_CAP_PROP_FPS Frame rate.
6. CV_CAP_PROP_FOURCC 4-character code of codec.
7. CV_CAP_PROP_FRAME_COUNT Number of frames in the video file.
8. CV_CAP_PROP_FORMAT Format of the Mat objects returned by retrieve() .
9. CV_CAP_PROP_MODE Backend-specific value indicating the current capture mode.
10. CV_CAP_PROP_BRIGHTNESS Brightness of the image (only for cameras).
11. CV_CAP_PROP_CONTRAST Contrast of the image (only for cameras).
12. CV_CAP_PROP_SATURATION Saturation of the image (only for cameras).
13. CV_CAP_PROP_HUE Hue of the image (only for cameras).
14. CV_CAP_PROP_GAIN Gain of the image (only for cameras).
15. CV_CAP_PROP_EXPOSURE Exposure (only for cameras).
16. CV_CAP_PROP_CONVERT_RGB Boolean flags indicating whether images should be converted to RGB.
17. CV_CAP_PROP_WHITE_BALANCE Currently unsupported
18. CV_CAP_PROP_RECTIFICATION Rectification flag for stereo cameras (note: only supported by DC1394 v 2.x backend currently)
*/

void CameraCV::rmvideodevice(){
    camera->release();

    QTextStream(stdout) << camera->isOpened();
    delete camera;
    camera = nullptr;
}

QSharedPointer<cv::Mat> CameraCV::get_current_frm(){
   std::unique_lock<std::mutex> frameBufferLock(mFrameBufferMutex);
   return this->currentFrame;
}

void CameraCV::set_current_frm(cv::Mat &p_frm)
{

    QSharedPointer<cv::Mat> temp( &p_frm);
    this->currentFrame = temp;
}

void CameraCV::getCameraframe(){
    while(mActive== true)
    {
        QSharedPointer<cv::Mat> temp( new cv::Mat );

        camera->read(*temp);

        //*temp = camera->grab();
 //       camera->retrieve(*temp);
        std::unique_lock<std::mutex> frameBufferLock(mFrameBufferMutex);  //csak most lockoljuk a frame buffert az utolso pillanatban par microsecig, amig a swap megtortenik
        std::swap(currentFrame,temp);
        frameBufferLock.unlock();
    }
}

//starts an endless loop on a new thread,
void CameraCV::spawnCameraLoop()
{
    float w =1920;
    float h = 1080;
    QTextStream(stdout)<< "setting sizes";
    this->setimagewidth(w);
    this->setimageheight(h);
    mActive= true;
    mBGThread = std::thread(&CameraCV::getCameraframe, this);
  //  mBGThread.detach();

}

void CameraCV::stopCameraLoop(){
    mActive=false;
    mBGThread.join();

}

cv::VideoCapture* CameraCV::getCamera(){
   return this->camera;
}

void CameraCV::freeframe(){
    delete this->frame;
}

void CameraCV::setimagewidth(float& imwidth){

   //QTextStream(stdout)  << "width " << imwidth << endl;
   this->camera->set(cv::CAP_PROP_FRAME_WIDTH,imwidth);
}

void CameraCV::setimageheight(float& imheight){
   //  QTextStream(stdout)  << "heights " << imheight << endl;
     camera->set(cv::CAP_PROP_FRAME_HEIGHT,imheight);
}

void CameraCV::setframerate(int reqframerate){
    this->camera->set(5,reqframerate);
}

void CameraCV::setgain(float gain){
   this->camera->set(14,gain);
}

void CameraCV::setexposuretime(float exptime){
    this->camera->set(15,exptime);
}
