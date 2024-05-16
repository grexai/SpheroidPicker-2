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


//
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <inttypes.h>
#include <iomanip>
#include <fstream>
#include <memory>
#include <exception>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <tensorflow/c/c_api.h>
#include <QMutex>
#include <QMutexLocker>


class i_deeplearning
{
public:
    i_deeplearning(){}
    virtual ~i_deeplearning()= 0;
    bool m_isruning = false;
    virtual void setup_dnn_network(const char* cf, const char* model_w, const char* t_g) = 0;
    virtual std::vector<std::vector<float>> dnn_inference(cv::Mat& input,cv::Mat& output,cv::Mat& maskimage,std::vector<cv::Mat>& bboxes,float det_conf, float mask_conf)= 0;
};

class invecption_v2 : public virtual i_deeplearning
{
public:
    invecption_v2();
    ~invecption_v2() override;
    void drawBox(cv::Mat& frame, int classId, float conf, cv::Rect box, cv::Mat& objectMask);
    std::vector<std::vector<float>> postprocess(cv::Mat& frame, const std::vector<cv::Mat>& outs);
    void resize(cv::Mat& input, cv::Mat& out);
    virtual void setup_dnn_network(const char* cf, const char* model_w, const char* t_g) override;
  //  std::vector<std::vector<float>> dnn_inference(cv::Mat& input);
    virtual std::vector<std::vector<float>> dnn_inference(cv::Mat& input,cv::Mat& output,cv::Mat& maskimage,std::vector<cv::Mat>& bboxes,float det_conf, float mask_conf) override;
  //  std::vector<std::vector<float>> objpos;
    std::vector<std::string> classes;
    std::vector<cv::Scalar> colors;
    // Initialize the parameters
protected:
    cv::dnn::Net* net;
    float confThreshold = 0.4f; // Confidence threshold
    float maskThreshold = 0.3f; // Mask threshold
};

class matterport_mrcnn : public virtual i_deeplearning
{

    static void DontDeleteTensor(void*, size_t, void*)
    {
    }

    static void DeleteBuffer(void* pData, size_t)
    {
        free(pData);
    }
protected:
    TF_Graph *m_graph = nullptr;
    TF_Session* m_session = nullptr;
    TF_Status *m_status = nullptr;
    TF_SessionOptions *m_options= nullptr;
    std::vector<float> m_anchors;

public:
    matterport_mrcnn(){}
    ~matterport_mrcnn() override;
    bool m_isruning = false;
    #define TF_DELETER(X) typedef void(*TF_Delete ## X ## _Func)(TF_ ## X *);
    #define TF_POINTER(X) typedef std::unique_ptr< TF_ ## X , TF_Delete ## X ## _Func> TF_ ## X ## _Ptr;

    TF_DELETER(Buffer)
    TF_DELETER(Graph)
    TF_DELETER(Status)
    TF_DELETER(ImportGraphDefOptions)
    TF_DELETER(SessionOptions)
    TF_DELETER(Tensor)

    TF_POINTER(Buffer)
    TF_POINTER(Graph)
    TF_POINTER(Status)
    TF_POINTER(ImportGraphDefOptions)
    TF_POINTER(SessionOptions)
    TF_POINTER(Tensor)

    //image constants...
    const std::vector<int> ANCHOR_SIZES = { 128, 256, 512, 768, 1024, 1536, 2048 };
    cv::Scalar MEAN_PIXEL = cv::Scalar(123.7, 116.8, 103.9);
  //  const cv::Scalar MEAN_PIXEL(123.7f);

    //constants for this graph type
    static constexpr float DETECTION_CONFIDENCE = 0.5f;

    static constexpr float MASK_CONFIDENCE = 0.5f;

    int IMAGE_SIZE=1024;

    TF_Buffer_Ptr ReadFile(const char* pFile)
    {
        TF_Buffer_Ptr buf(TF_NewBuffer(), TF_DeleteBuffer);

        FILE *f = std::fopen(pFile, "rb");
        fseek(f, 0, SEEK_END);
        long fsize = std::ftell(f);
        fseek(f, 0, SEEK_SET);  //same as rewind(f);

        void* data = malloc(fsize);
        fread(data, fsize, 1, f);
        std::fclose(f);

        buf->data_deallocator = DeleteBuffer;
        buf->data = data;
        buf->length = fsize;
        return buf;
    }

    void  read_graph(const char* modelPB) ;

    std::vector <float> load_anchor(const char* modelPath,const int IMAGE_SIZE) ;

    int select_anchor();

    cv::Mat mold_image(cv::Mat &image, const int IMAGE_SIZE, int maxDim) ;

    // std::vector<std::vector<float>> inferencing(cv::Mat& image,std::vector<cv::Mat>& bboxes) ;

    void create_session();
    virtual void setup_dnn_network(const char* modelPB, const char* modelPath, const char* empty) override;

    virtual std::vector<std::vector<float>> dnn_inference(cv::Mat& input,cv::Mat& output,cv::Mat& maskimage,std::vector<cv::Mat>& bboxes,float det_conf, float mask_conf) override;
private:

    QMutex m_mutex;

};


#endif // DEEPLEARNING_H
