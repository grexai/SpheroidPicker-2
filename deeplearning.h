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

class i_deeplearning
{
public:
    i_deeplearning(){}
    virtual ~i_deeplearning()= 0;
    virtual void setup_dnn_network(std::string cf, std::string model_w, std::string t_g) = 0;
    virtual std::vector<std::vector<float>> dnn_inference(cv::Mat& input,cv::Mat& output)= 0;
};

class invecption_v2 : public virtual i_deeplearning
{
public:
    invecption_v2();
    ~invecption_v2() override;
    void drawBox(cv::Mat& frame, int classId, float conf, cv::Rect box, cv::Mat& objectMask);
    std::vector<std::vector<float>> postprocess(cv::Mat& frame, const std::vector<cv::Mat>& outs);
    void resize(cv::Mat& input, cv::Mat& out);
    virtual void setup_dnn_network(std::string cf, std::string model_w, std::string t_g) override;
    std::vector<std::vector<float>> dnn_inference(cv::Mat& input);
    std::vector<std::vector<float>> dnn_inference(cv::Mat& input,cv::Mat& output) override;
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
public:
   // matterport_mrcnn();



    matterport_mrcnn(){}
    ~matterport_mrcnn() override;



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

    TF_Graph *m_graph;
   // TF_Session_Wrapper *m_session;
   TF_Session* m_session;
    TF_Status *m_status;
    TF_SessionOptions *m_options;
    std::vector<float> m_anchors;
    //image constants...
    const std::vector<int> ANCHOR_SIZES = { 128, 256, 512, 768, 1024, 1536, 2048 };
    cv::Scalar MEAN_PIXEL = cv::Scalar(123.7f, 116.8, 103.9);
  //  const cv::Scalar MEAN_PIXEL(123.7f);

    //constants for this graph type
    static constexpr float DETECTION_CONFIDENCE = 0.9f;
    static constexpr float MASK_CONFIDENCE = 0.9f;
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

    void  read_graph(const char* modelPB) {

        m_graph = TF_NewGraph() ;
                 //Reading graph


        TF_Buffer_Ptr graphDef = ReadFile(modelPB);

        TF_Status_Ptr importStatus(TF_NewStatus(), TF_DeleteStatus);

        TF_ImportGraphDefOptions_Ptr importOptions(TF_NewImportGraphDefOptions(), TF_DeleteImportGraphDefOptions);

        TF_GraphImportGraphDef(m_graph, graphDef.get(), importOptions.get(), importStatus.get());

        importOptions.reset();
        if (TF_GetCode(importStatus.get()) != TF_OK) throw std::runtime_error(std::string("Cannot import graph: ") + TF_Message(importStatus.get()));

        importStatus.reset();
        graphDef.reset();

        //return m_graph;
    }

    std::vector <float> load_anchor(const char* modelPath,const int IMAGE_SIZE) {
        std::ifstream file(modelPath + std::to_string(IMAGE_SIZE) + ".anc");
        if (file.is_open() == false)
        {
            std::cout << "anchpath " << modelPath + std::to_string(IMAGE_SIZE) + ".anc ";
            throw std::runtime_error("file is not opened ");
        }

        std::string str;
        std::vector<float> anchors;
        std::string segment;
        while (std::getline(file, str))
        {
            std::istringstream tokenStream(str);
            std::string out;
            while (std::getline(tokenStream, out, ';'))
            {
                anchors.push_back(atof(out.c_str()));
            }

        }
        file.close();
        return anchors;
    }

    int select_anchor() {
    //   int maxDim = (std::max)(image.cols, image.rows);
        int maxDim = 1024;
        int anchorSize = ANCHOR_SIZES.back();
        //anchorSize = 512;
        for (const int anchor : ANCHOR_SIZES)
        {
            std::cout << "anchor :: " << anchor << std::endl;
            anchorSize = anchor;
            if (anchorSize >= maxDim) break;
        }

        return anchorSize;
    }

    cv::Mat mold_image(cv::Mat &image, const int IMAGE_SIZE, int maxDim) {

        //matterport's mrcnn needs squared images
        cv::Mat squareImage;
        //image is larger than the largest anchor set, need to resize
        if (IMAGE_SIZE < maxDim)
        {
            cv::Mat temp = cv::Mat::zeros(maxDim, maxDim, image.type());
            image.copyTo(temp(cv::Rect(0, 0, image.cols, image.rows)));
            cv::resize(temp, squareImage, cv::Size(IMAGE_SIZE, IMAGE_SIZE), 0, 0, cv::INTER_LINEAR);
        }
        //image is square and equal to the anchor size
        else if (IMAGE_SIZE == maxDim && image.cols == image.rows)
        {
            squareImage = image;
        }
        //image is smaller than the anchor size
        else
        {
            squareImage = cv::Mat::zeros(IMAGE_SIZE, IMAGE_SIZE, image.type());
            image.copyTo(squareImage(cv::Rect(0, 0, image.cols, image.rows)));
        }

        cv::Mat moldedInput(squareImage.size(), CV_32FC3);
        squareImage.convertTo(moldedInput, CV_32FC3);
        cv::subtract(moldedInput, MEAN_PIXEL, moldedInput);
        return moldedInput;
    }

    void inferencing(cv::Mat& image) ;

    void create_session() {
     //   TF_SessionOptions_Ptr m_options(TF_NewSessionOptions(), TF_DeleteSessionOptions);
    //    TF_Status_Ptr status(TF_NewStatus(), TF_DeleteStatus);
//        m_session = TF_Session_Wrapper(TF_NewSession(m_graph, m_options, m_status));
        m_options = TF_NewSessionOptions();
        m_status =  TF_NewStatus();
        m_session = TF_NewSession(m_graph, m_options, m_status);
        // m_session();
       // m_s= TF_NewSession(m_graph, m_options, m_status);
        if (TF_GetCode(m_status) != 0) throw std::runtime_error(std::string("Cannot establish session: ") + TF_Message(m_status));


      //  return m_session;

    }

    std::vector<std::vector<float>> dnn_inference(cv::Mat& input,cv::Mat& output) override{

        inferencing(input);
        output = input;
        std::vector<std::vector<float>> obj;
        return obj;
 //   }
      }

    virtual void setup_dnn_network(std::string modelPB, std::string modelPath, std::string empty) override;

    class TF_Session_Wrapper
    {
    public:
        TF_Session_Wrapper(TF_Session* pSession) : mSession(pSession) {}
        TF_Session* Get() noexcept { return mSession; }
        const TF_Session* Get() const noexcept  { return mSession; }
        ~TF_Session_Wrapper() { CloseAndDelete(); }
        void CloseAndDelete(TF_Status* pStatus = nullptr)
        {
            if(mSession != nullptr)
            {
                if(pStatus == nullptr)
                {
                    TF_Status_Ptr status(TF_NewStatus(), TF_DeleteStatus);
                    TF_CloseSession(mSession, status.get());
                    TF_DeleteSession(mSession, status.get());
                }
                else
                {
                    TF_CloseSession(mSession, pStatus);
                    TF_DeleteSession(mSession, pStatus);
                }
            }
            mSession = nullptr;
        }
        TF_Session_Wrapper(const TF_Session_Wrapper&) = delete;
        TF_Session_Wrapper& operator=(const TF_Session_Wrapper&) = delete;
        TF_Session_Wrapper(TF_Session_Wrapper&& pOther);
        TF_Session_Wrapper& operator=(TF_Session_Wrapper&& pOther);
    private:
        TF_Session* mSession = nullptr;
    };


}
;


#endif // DEEPLEARNING_H
