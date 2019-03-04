#ifndef IMAGING_H
#define IMAGING_H
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include <opencv2\core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <types.h>
#include <QString>
#include <QtCore>
class imagetools
{
public:
    imagetools(){}

    ~imagetools(){}

    cv::Mat* getframe();

    void setframe(cv::Mat &input);

    cv::Mat* get_display_frm();

    void addPointToImage(cv::Mat& img,cv::Point point);

    int2coors getSphCoors(cv::Mat &img);

    void saveImg(cv::Mat* outimg, std::string outname);

    cv::Mat convert_bgr_to_rgb(QSharedPointer<cv::Mat> pinput);

protected:
    cv::Mat* frame= nullptr;
    QSharedPointer<cv::Mat> dispfrm = nullptr;
    cv::Mat* temp = nullptr;

private:


};

#endif // IMAGING_H
