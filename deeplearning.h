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
    virtual ~i_deeplearning(){}
    virtual void setup_dnn_network(std::string cf, std::string model_w, std::string t_g) = 0;
    std::vector<std::vector<float>> dnn_inference(cv::Mat& input) ;
    virtual std::vector<std::vector<float>> dnn_inference(cv::Mat& input,cv::Mat& output)= 0;
    // Initialize the parameters
protected:
    cv::dnn::Net* net;
    float confThreshold = 0.4f; // Confidence threshold
    float maskThreshold = 0.3f; // Mask threshold
};


class invecption_v2 : public virtual i_deeplearning
{
public:
    invecption_v2();
    ~invecption_v2() override;
    void drawBox(cv::Mat& frame, int classId, float conf, cv::Rect box, cv::Mat& objectMask);
    std::vector<std::vector<float>> postprocess(cv::Mat& frame, const std::vector<cv::Mat>& outs);
    void resize(cv::Mat& input, cv::Mat& out);
    void setup_dnn_network(std::string cf, std::string model_w, std::string t_g) override;
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


class keras_mrcnn :public i_deeplearning
{



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
    static void DontDeleteTensor(void*, size_t, void*)
    {
    }

    static void DeleteBuffer(void* pData, size_t)
    {
        free(pData);
    }
public:
    //image constants...
    const std::vector<int> ANCHOR_SIZES = { 128, 256, 512, 768, 1024, 1536, 2048 };
    cv::Scalar MEAN_PIXEL = cv::Scalar(123.7f, 116.8, 103.9);
  //  const cv::Scalar MEAN_PIXEL(123.7f);

    //constants for this graph type
    static constexpr float DETECTION_CONFIDENCE = 0.9f;
    static constexpr float MASK_CONFIDENCE = 0.9f;


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

    TF_Graph_Ptr  read_graph(const char* modelPB) {

        TF_Graph_Ptr graph(TF_NewGraph(), TF_DeleteGraph);
                 //Reading graph
        TF_Buffer_Ptr graphDef = ReadFile(modelPB);

        TF_Status_Ptr importStatus(TF_NewStatus(), TF_DeleteStatus);
        TF_ImportGraphDefOptions_Ptr importOptions(TF_NewImportGraphDefOptions(), TF_DeleteImportGraphDefOptions);
        TF_GraphImportGraphDef(graph.get(), graphDef.get(), importOptions.get(), importStatus.get());
        importOptions.reset();
        if (TF_GetCode(importStatus.get()) != TF_OK) throw std::runtime_error(std::string("Cannot import graph: ") + TF_Message(importStatus.get()));

        importStatus.reset();
        graphDef.reset();

        return graph;
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

    int select_anchor(cv::Mat image) {
       int maxDim = (std::max)(image.cols, image.rows);

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

    cv::Mat mold_image(cv::Mat image, const int IMAGE_SIZE, int maxDim) {

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

    void inferencing(cv::Mat image, TF_Session* session, TF_Status* status, cv::Mat moldedInput, int IMAGE_SIZE, TF_Graph* graph, std::vector <float> anchors,const char* outFileName) {
        static constexpr int NUM_CLASSES = 2;
        static constexpr int METADATA_LEN = 1 + 3 + 3 + 4 + 1 + NUM_CLASSES;
        float IMAGE_METADATA[METADATA_LEN] = { 0.0f,                                                                        //image id
            static_cast<float>(IMAGE_SIZE), static_cast<float>(IMAGE_SIZE), 3.0f,        //original image shape (irrelevant)
            static_cast<float>(IMAGE_SIZE), static_cast<float>(IMAGE_SIZE), 3.0f,        //molded image shape
            0.0f, 0.0f, static_cast<float>(IMAGE_SIZE), static_cast<float>(IMAGE_SIZE),  //window (y1, x1, y2, x2)
            1.0f,                                                                        //scale factor: was 1.0f
            0.0f, 1.0f };


        //Defining inputs
        std::vector<TF_Output> inputs;
        std::vector<TF_Tensor*> inputTensors;

        //image tensor ezt
        TF_Operation* inputOpImage = TF_GraphOperationByName(graph, "input_image");
        if (inputOpImage == nullptr) throw std::runtime_error("Missing node!");

        TF_Output inputOutImage = { inputOpImage, 0 };
        inputs.push_back(inputOutImage);

        const int64_t inputTensorDimsImage[4] = { 1, moldedInput.rows, moldedInput.cols, moldedInput.channels() };
        TF_Tensor_Ptr inputTensorImage(TF_NewTensor(TF_FLOAT, inputTensorDimsImage, 4, moldedInput.data, sizeof(float) * IMAGE_SIZE * IMAGE_SIZE * 3, DontDeleteTensor, nullptr), TF_DeleteTensor);
        inputTensors.push_back(inputTensorImage.get());

        //meta tensor
        TF_Operation* inputOpMeta = TF_GraphOperationByName(graph, "input_image_meta");
        if (inputOpMeta == nullptr) throw std::runtime_error("Missing node!");

        TF_Output inputOutMeta = { inputOpMeta, 0 };
        inputs.push_back(inputOutMeta);

        const int64_t inputTensorDimsMeta[2] = { 1, METADATA_LEN };
        TF_Tensor_Ptr inputTensorMeta(TF_NewTensor(TF_FLOAT, inputTensorDimsMeta, 2, IMAGE_METADATA, sizeof(IMAGE_METADATA), DontDeleteTensor, nullptr), TF_DeleteTensor);
        inputTensors.push_back(inputTensorMeta.get());

        //anchor tensor
        TF_Operation* inputOpAnchor = TF_GraphOperationByName(graph, "input_anchors");
        if (inputOpAnchor == nullptr) throw std::runtime_error("Missing node!");

        TF_Output inputOutAnchor = { inputOpAnchor, 0 };
        inputs.push_back(inputOutAnchor);

        const int64_t inputTensorDimsAnchor[3] = { 1, static_cast<int64_t>(anchors.size()) / 4, 4 };
        TF_Tensor_Ptr inputTensorAnchor(TF_NewTensor(TF_FLOAT, inputTensorDimsAnchor, 3, anchors.data(), anchors.size() * sizeof(float), DontDeleteTensor, nullptr), TF_DeleteTensor);
        inputTensors.push_back(inputTensorAnchor.get());


        // ezek
        std::vector<TF_Output> outputs;
        std::vector<TF_Tensor*> outputTensorPtrs;

        TF_Operation* outputOpDetection = TF_GraphOperationByName(graph, "mrcnn_detection/Reshape_1");
        if (outputOpDetection == nullptr) throw std::runtime_error("Missing node!");

        TF_Output outputOutDetection = { outputOpDetection, 0 };
        outputs.push_back(outputOutDetection);
        outputTensorPtrs.push_back(nullptr);

        TF_Operation* outputOpMask = TF_GraphOperationByName(graph, "mrcnn_mask/Reshape_1");
        if (outputOpMask == nullptr) throw std::runtime_error("Missing node!");

        TF_Output outputOutMask = { outputOpMask, 0 };
        outputs.push_back(outputOutMask);
        outputTensorPtrs.push_back(nullptr);

        std::vector<TF_Tensor_Ptr> outputTensors;
        outputTensors.reserve(outputTensorPtrs.size());

        std::cout << " inferencing" << std::endl;
     //   auto start = std::chrono::system_clock::now();

        //Inferencing
        TF_SessionRun(session, nullptr,
            inputs.data(), inputTensors.data(), inputs.size(),
            outputs.data(), outputTensorPtrs.data(), outputs.size(),
            nullptr, 0, nullptr, status);

        std::cout << "wrapping" << std::endl;
        //Wrapping all raw pointers
        for (TF_Tensor* outputTensor : outputTensorPtrs) outputTensors.emplace_back(TF_Tensor_Ptr(outputTensor, TF_DeleteTensor));
        outputTensorPtrs.clear();

        if (TF_GetCode(status) != 0) throw std::runtime_error(std::string("Error during processing: ") + TF_Message(status));

        static constexpr int BATCH_DIM = 0;
        static constexpr int DETECTIONS_DIM = 1;

        static constexpr int MRCNN_DETECTION = 0;

        static constexpr int MRCNN_DETECTION_BB_LO_Y = 0;
        static constexpr int MRCNN_DETECTION_BB_LO_X = 1;
        static constexpr int MRCNN_DETECTION_BB_HI_Y = 2;
        static constexpr int MRCNN_DETECTION_BB_HI_X = 3;
        static constexpr int MRCNN_DETECTION_CLASS = 4;
        static constexpr int MRCNN_DETECTION_SCORE = 5;

        static constexpr int MRCNN_MASK = 1;
        static constexpr int MRCNN_MASK_HEIGHT_DIM = 2;
        static constexpr int MRCNN_MASK_WIDTH_DIM = 3;
        static constexpr int MRCNN_MASK_CLASS_DIM = 4;

        //sanity checks
        const TF_Tensor* detTensor = outputTensors[MRCNN_DETECTION].get();
        if (detTensor == nullptr) throw std::runtime_error("Missing output");
        if (TF_TensorType(detTensor) != TF_FLOAT) throw std::runtime_error("Invalid output format");


        if (TF_NumDims(detTensor) != 3) throw std::runtime_error("Invalid output format");

        const int batchSize = TF_Dim(detTensor, BATCH_DIM);
        const int maxDet = TF_Dim(detTensor, DETECTIONS_DIM);
        if (TF_Dim(detTensor, 2) != 6) throw std::runtime_error("Invalid output format");

        const TF_Tensor* maskTensor = outputTensors[MRCNN_MASK].get();
        if (maskTensor == nullptr) throw std::runtime_error("Missing output");

        if (TF_TensorType(maskTensor) != TF_FLOAT) throw std::runtime_error("Invalid output format");
        if (TF_NumDims(maskTensor) != 5) throw std::runtime_error("Invalid output format");
        if (batchSize != TF_Dim(maskTensor, BATCH_DIM)) throw std::runtime_error("Invalid output format");
        if (maxDet != TF_Dim(maskTensor, DETECTIONS_DIM)) throw std::runtime_error("Invalid output format");

        int32_t maskWidth = TF_Dim(maskTensor, MRCNN_MASK_WIDTH_DIM);
        int32_t maskHeight = TF_Dim(maskTensor, MRCNN_MASK_HEIGHT_DIM);
        if (NUM_CLASSES != TF_Dim(maskTensor, MRCNN_MASK_CLASS_DIM)) throw std::runtime_error("Invalid output format");

        for (std::size_t i = 0; i < outputTensors.size(); ++i)
        {
            const int numdim = TF_NumDims(outputTensors[i].get());
            std::cout << "Dimensions for " << i << ": " << numdim << std::endl;
            for (int j = 0; j < numdim; ++j)
            {
                std::cout << "  " << j << ": " << TF_Dim(outputTensors[i].get(), j) << std::endl;
            }
        }

        const int detectionIncrement = 6;
        const int maskIncrement = maskHeight * maskWidth * NUM_CLASSES;

        std::cout << "batchSize: " << batchSize << std::endl;

        //NOTE: only batch size == 1 is tested
        for (int b = 0; b < batchSize; ++b)
        {
            cv::Mat newImage;
            image.copyTo(newImage);

            std::cout << "image.copyTo(newImage) done" << std::endl;
            cv::Mat labels = cv::Mat::zeros(newImage.size(), CV_32SC1);
            //The prediction is always done on squared images
            //If the image is smaller than the used image size, it is padded with zeros, else it is rescaled to that size
            //This float is used to transfer the bounding boxes to the real size
            float restoreDim = (std::max)(static_cast<float>((std::max)(image.rows, image.cols)), static_cast<float>(IMAGE_SIZE));
            std::cout << "restoreDim casting done" << std::endl;

            //getting tensor data for the batch
            const float* detTensorData = &reinterpret_cast<const float*>(TF_TensorData(outputTensors[MRCNN_DETECTION].get()))[b * maxDet * detectionIncrement];
            float* maskTensorData = &reinterpret_cast<float*>(TF_TensorData(outputTensors[MRCNN_MASK].get()))[b * maxDet * maskIncrement];
            std::cout << "getting tensor data done" << std::endl;

            //setting movable pointer
            const float* detTensorPtr = detTensorData;
            float* maskTensorPtr = maskTensorData;
            std::cout << "setting movable pointer done" << std::endl;

            //iterating through results (moving pointers)
            for (int d = 0; d < maxDet; ++d, detTensorPtr += detectionIncrement, maskTensorPtr += maskIncrement)
            {
                //std::cout << "inside for detTensorPtr: " << detTensorPtr << std::endl;
                //std::cout << "inside for maskTensorPtr: " << *maskTensorPtr << std::endl;

                const float& score = detTensorPtr[MRCNN_DETECTION_SCORE];
                if (score < DETECTION_CONFIDENCE) continue;
                std::cout << "score: " << score << std::endl;
                const float& classId = detTensorPtr[MRCNN_DETECTION_CLASS];

                //order change
                float box[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
                box[0] = detTensorPtr[MRCNN_DETECTION_BB_LO_X];
                box[1] = detTensorPtr[MRCNN_DETECTION_BB_LO_Y];
                box[2] = detTensorPtr[MRCNN_DETECTION_BB_HI_X];
                box[3] = detTensorPtr[MRCNN_DETECTION_BB_HI_Y];

                //Processing result
                int32_t bb[4] = { 0,0,0,0 };

                bb[0] = static_cast<int>(std::round(box[0] * restoreDim));
                bb[1] = static_cast<int>(std::round(box[1] * restoreDim));
                bb[2] = static_cast<int>(std::round(box[2] * restoreDim));
                bb[3] = static_cast<int>(std::round(box[3] * restoreDim));

                //However we want boxes only on the original image
                //box is outside the original image dimensions
                if (bb[0] >= image.cols) continue;
                if (bb[1] >= image.rows) continue;

                bb[0] = (std::min)((std::max)(bb[0], 0), image.cols);
                bb[1] = (std::min)((std::max)(bb[1], 0), image.rows);
                bb[2] = (std::min)((std::max)(bb[2], 0), image.cols);
                bb[3] = (std::min)((std::max)(bb[3], 0), image.rows);

                cv::Mat mask(maskHeight, maskWidth, CV_MAKETYPE(CV_32F, NUM_CLASSES), maskTensorPtr);

                cv::Rect rect(cv::Point(bb[0], bb[1]), cv::Point(bb[2], bb[3]));
                cv::resize(mask, mask, rect.size(), 0.0, 0.0, cv::INTER_LINEAR);


                cv::Mat label = cv::Mat::zeros(mask.size(), CV_32SC1);
                const int64_t total = mask.total();
                const float* maskPtr = reinterpret_cast<const float*>(mask.data);
                int32_t* labelPtr = reinterpret_cast<int32_t*>(label.data);
                for (int64_t index = 0; index < total; ++index, maskPtr += NUM_CLASSES)
                {
                    if (maskPtr[1] > MASK_CONFIDENCE)
                    {
                        labelPtr[index] = d + 1;
                    }
                }

                cv::Mat roi = labels(rect);
                label.copyTo(roi);
            }


            const int64_t total = labels.total();
            uint8_t* imgPtr = reinterpret_cast<uint8_t*>(newImage.data);
            const int32_t* labelPtr = reinterpret_cast<const int32_t*>(labels.data);
            for (int64_t index = 0; index < total; ++index)
            {
                if (labelPtr[index] != 0)
                {
                    imgPtr[(index * 3) + 1] = 255;
                }
            }

          //  auto end = std::chrono::system_clock::now();
            //
        //    std::chrono::duration<double> elapsed_seconds = end - start;

            // CHRONO END
     //       std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
            bool writtenSuccessfully = cv::imwrite(outFileName, newImage);
            std::cout << "imwrite done: " << writtenSuccessfully << " FINISHED" << std::endl;
            cv::imshow("resuls", newImage);
        }



        //Finishing
        //session.CloseAndDelete(status.get());
        //if (TF_GetCode(status.get()) != 0) throw std::runtime_error(std::string("Error during processing: ") + TF_Message(status.get()));


    }

    std::vector<std::vector<float>> dnn_inference(cv::Mat& input,cv::Mat& output) override;

    TF_Session_Wrapper create_session(TF_Graph* graph) {
        TF_SessionOptions_Ptr options(TF_NewSessionOptions(), TF_DeleteSessionOptions);
        TF_Status_Ptr status(TF_NewStatus(), TF_DeleteStatus);
        TF_Session_Wrapper session(TF_NewSession(graph, options.get(), status.get()));
        if (TF_GetCode(status.get()) != 0) throw std::runtime_error(std::string("Cannot establish session: ") + TF_Message(status.get()));
        return session;

    }
};


#endif // DEEPLEARNING_H
