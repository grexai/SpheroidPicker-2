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
        this->frame->release();
    }
    this->frame = new cv::Mat();
    this->frame = &input;
}

cv::Mat* CameraCV::getdisplayframe(){
    return (this->dispfrm);

}

void CameraCV::setvideodevice(int devid){
    this->camera = new cv::VideoCapture(devid);

}

bool CameraCV::is_camera_open(){
    return  this->camera->isOpened();
}

void CameraCV::resetvideodevice(){
     this->camera->set(CV_CAP_PROP_SETTINGS,0.0);
}

void CameraCV::rmvideodevice(){
    camera->release();
    QTextStream(stdout) << camera->isOpened();
    camera->~VideoCapture();
}

QSharedPointer<cv::Mat> CameraCV::get_current_frm(){
   std::unique_lock<std::mutex> frameBufferLock(mFrameBufferMutex);
   return this->currentFrame;
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

void CameraCV::spawnCameraLoop()
{
   // mActive= true;
    mBGThread = std::thread(&CameraCV::getCameraframe, this);
  //  mBGThread.detach();
  //  std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //ez elinditja a hatter szalat

}

cv::VideoCapture* CameraCV::getCamera(){
   return this->camera;
}

void CameraCV::freeframe(){
    delete this->frame;
}



void CameraCV::setimagewidth(float& imwidth){

   //QTextStream(stdout)  << "width " << imwidth << endl;
   this->camera->set(CV_CAP_PROP_FRAME_WIDTH,imwidth);
}

void CameraCV::setimageheight(float& imheight){
   //  QTextStream(stdout)  << "heights " << imheight << endl;
     camera->set(CV_CAP_PROP_FRAME_HEIGHT,imheight);
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
