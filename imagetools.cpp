#include "imagetools.h"
#include <thread>


cv::Mat* imagetools::getframe()
{
    return (this->frame);
}

void imagetools::setframe(cv::Mat &input){
    if (this->frame != nullptr){
     //   delete[] (this->frame);
        this->frame->release();
    }
    this->frame = new cv::Mat();
    this->frame = &input;
}

cv::Mat* imagetools::getdisplayframe(){
    return (this->dispfrm);

}

void imagetools::setvideodevice(int devid){
    this->camera = new cv::VideoCapture(devid);
    iscameraopen = this->camera->isOpened();
}

void imagetools::resetvideodevice(){
     this->camera->set(CV_CAP_PROP_SETTINGS,0.0);
}

void imagetools::rmvideodevice(){
    camera->release();
    iscameraopen= camera->isOpened();
    camera = nullptr;
    delete  camera;
}

cv::Mat* imagetools::get_current_frm(){
    return this->currentFrame.get();
}

void imagetools::getCameraframe(){
    if (this->frame != nullptr) { delete frame;}
    this->frame = new cv::Mat();
//  currentFrame = QSharedPointer<cv::Mat> (new cv::Mat);
    QSharedPointer<cv::Mat> temp( new cv::Mat );
    camera->read(*temp);
    std::unique_lock<std::mutex> frameBufferLock(mFrameBufferMutex);  //csak most lockoljuk a frame buffert az utolso pillanatban par microsecig, amig a swap megtortenik
    std::swap(currentFrame,temp);
    frameBufferLock.unlock();
    if (this->dispfrm != nullptr) {delete  dispfrm;}
    this->dispfrm = new cv::Mat();
    cvtColor(*this->currentFrame.get(),*this->dispfrm,CV_BGR2RGB,0);
}

void imagetools::freeframe(){
    delete this->frame;
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

void imagetools::setimagewidth(float& imwidth){

   //QTextStream(stdout)  << "width " << imwidth << endl;
   this->camera->set(CV_CAP_PROP_FRAME_WIDTH,imwidth);
}

void imagetools::setimageheight(float& imheight){
   //  QTextStream(stdout)  << "heights " << imheight << endl;
     camera->set(CV_CAP_PROP_FRAME_HEIGHT,imheight);
}

void imagetools::setframerate(int reqframerate){
    this->camera->set(5,reqframerate);
}

void imagetools::setgain(float gain){
   this->camera->set(14,gain);
}

void imagetools::setexposuretime(float exptime){
    this->camera->set(15,exptime);
}

cv::VideoCapture* imagetools::getCamera(){
   return this->camera;
}

int2coors imagetools::getSphCoors(cv::Mat &img){
    using namespace   std;
    int2coors sphcoors;
        cv::Mat gray;
        cv::cvtColor(img, gray, cv::COLOR_RGB2GRAY);
        cv::Mat out,out2;

    //	threshold(gray, out, 105, 255, THRESH_BINARY);
        threshold(gray, out, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    //	adaptiveThreshold(gray, out2, 255, CV, THRESH_BINARY,27,2);

        bitwise_not(out, out);

        cv::Mat label ;

//     int la = connectedComponents(out, label,8, CV_16U);

        cv::Mat labels, stats, centroids;

        int nLabels = connectedComponentsWithStats(out, label,stats, centroids, 8, CV_16U);

        cv::Mat seeMyLabels;

        cv::normalize(label, seeMyLabels,0, 255, cv::NORM_MINMAX, CV_8U);

        cv::applyColorMap(seeMyLabels, seeMyLabels, cv::COLORMAP_JET);

        cout << "Number of connected components = " << nLabels << endl << endl;
        // Statistics
        cout << "Show statistics and centroids:" << endl;
        cout << "stats:" << endl << "(left,top,width,height,area)" << endl << stats << endl << endl;
        cout << "centroids:" << endl << "(x, y)" << endl << centroids << endl << endl;

        // Print individual stats for component 1 (component 0 is background)
        cout << "Component 1 stats:" << endl;

        std::cout << "CC_STAT_LEFT   = " << stats.at<int>(1, cv::CC_STAT_LEFT) << endl;
        cout << "CC_STAT_TOP    = " << stats.at<int>(1, cv::CC_STAT_TOP) << endl;
        cout << "CC_STAT_WIDTH  = " << stats.at<int>(1,cv:: CC_STAT_WIDTH) << endl;
        cout << "CC_STAT_HEIGHT = " << stats.at<int>(1,cv:: CC_STAT_HEIGHT) << endl;
        cout << "CC_STAT_AREA   = " << stats.at<int>(1,cv:: CC_STAT_AREA) << endl;

    return sphcoors;

}

void imagetools::addPointToImage(cv::Point point)
{
   cv::circle(*this->dispfrm,point, 5, (0,0,255), -1);
}

// Uses CV saveimg
void imagetools::saveImg(cv::Mat* outimg, std::string outname)
{
    outname = outname + ".png";
    QTextStream(stdout) << QString::fromStdString(outname);
    std::vector<int> compression_params;
  //  cv::Mat temp = currentFrame.get()->clone();
    //CV_IMWRITE_PNG_COMPRESSION
    compression_params.push_back(16);
    compression_params.push_back(0);
   // currentFrame.
   // imshow("adsad",);
    imwrite(outname, *currentFrame.get(), compression_params);

//    cout << "image saved as " << outname.str() << endl;

}

//void imagetools::
