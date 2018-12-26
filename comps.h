#ifndef COMPS_H
#define COMPS_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include <opencv2\core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <QTextStream>

typedef struct{
    cv::Mat img;
    cv::Mat pipette;
} centers;

extern cv::Mat getpcenter(cv::Mat& cppoints);

extern cv::Mat geticenter(cv::Mat& imagepoints);

extern cv::Mat coorsToMatrix(std::vector<float> in_vec, int coordims);

extern cv::Mat getTMatrix(cv::Mat& cppoints,cv::Mat& imagepoints, centers& centers);

extern cv::Mat getonimgpipettecoors(cv::Mat T,std::vector<float>mpos,
                                               cv::Mat& icenter,cv::Mat& pcenter);













#endif // COMPS_H
