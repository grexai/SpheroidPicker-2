#ifndef COMPS_H
#define COMPS_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include <opencv2\core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <QTextStream>

struct centers{
    cv::Mat img ;//= cv::Mat(2,1,CV_32F);
    cv::Mat pipette;// = cv::Mat(3,1,CV_32F);
};

cv::Mat getpcenter(const cv::Mat& cppoints);

cv::Mat geticenter(const cv::Mat& imagepoints);

cv::Mat calcTMatrix(cv::Mat& cppoints,cv::Mat& imagepoints, centers& centers);

cv::Mat calconimgpipettecoors(cv::Mat T,std::vector<float>mpos,
                                               cv::Mat& icenter,cv::Mat& pcenter);













#endif // COMPS_H
