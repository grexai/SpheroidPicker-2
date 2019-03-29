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
    void resize(cv::Mat& input, cv::Mat& out);
    void setup_dnn_network(std::string cf, std::string model_w, std::string t_g);
    void dnn_prediction(cv::Mat& input);
    std::vector<std::vector<float>> objpos;
    std::vector<std::string> classes;
    std::vector<cv::Scalar> colors;
    // Initialize the parameters
protected:
    cv::dnn::Net* net;
    float confThreshold = 0.4f; // Confidence threshold
    float maskThreshold = 0.3f; // Mask threshold
};

#endif // DEEPLEARNING_H
