#ifndef DEEPLEARNING_H
#define DEEPLEARNING_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <string.h>

#include <chrono>
#include <ctime>

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

class deeplearning
{
public:
    deeplearning();
    ~deeplearning();
    void drawBox(cv::Mat& frame, int classId, float conf, cv::Rect box, cv::Mat& objectMask);
    void postprocess(cv::Mat& frame, const std::vector<cv::Mat>& outs);
    void rundnn(cv::Mat& input);
    std::vector<std::string> classes;
    std::vector<cv::Scalar> colors;
    // Initialize the parameters
protected:
    float confThreshold = 0.4; // Confidence threshold
    float maskThreshold = 0.3; // Mask threshold
};

#endif // DEEPLEARNING_H
