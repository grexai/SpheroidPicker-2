#include "deeplearning.h"
#include "imagetools.h"

i_deeplearning:: ~i_deeplearning()
{

}

invecption_v2::invecption_v2()
{

}

invecption_v2::~invecption_v2()
{

}

// Draw the predicted bounding box, colorize and show the mask on the image
void invecption_v2::drawBox(cv::Mat& frame, int classId, float conf, cv::Rect box, cv::Mat& objectMask)
{
    using namespace cv;
    using namespace dnn;
    using namespace std;
    //Draw a rectangle displaying the bounding box
    cv::rectangle(frame, Point(box.x, box.y), Point(box.x+box.width, box.y+box.height), Scalar(255, 178, 50), 3);

    //Get the label for the class name and its confidence
    string label = format("%.2f", static_cast<double>(conf));
    if (!classes.empty())
    {
        CV_Assert(classId < (int)classes.size());
        label = classes[classId] + ":" + label;
    }

    //Display the label at the top of the bounding box
    int baseLine;
    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    box.y = max(box.y, labelSize.height);
    //rectangle(frame, Point(box.x, box.y - round(1.5*labelSize.height)), Point(box.x + round(1.5*labelSize.width), box.y + baseLine), Scalar(255, 255, 255), FILLED);
    //putText(frame, label, Point(box.x, box.y), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,0),1);


    // Resize the mask, threshold, color and apply it on the image
    cv::resize(objectMask, objectMask, Size(box.width, box.height));
    Mat mask = (objectMask > maskThreshold);
    Mat coloredRoi = (0.5 * colors[classId%colors.size()] + 0.5 * frame(box));
    coloredRoi.convertTo(coloredRoi, CV_8UC3);
    coloredRoi.copyTo(frame(box), mask);
}

// For each frame, extract the boundi ng box and mask for each detected object
std::vector<std::vector<float>> invecption_v2::postprocess(cv::Mat& frame, const std::vector<cv::Mat>& outs)
{
    using namespace cv;
    using namespace std;
    std::cout << "Outs: " << outs.size() << std::endl;
    std::vector<std::vector<float>> objpos;
    if (outs.size() >= 2){ //return 1;

    Mat outDetections = outs[0];
    Mat outMasks = outs[1];

    // Output size of masks is NxCxHxW where
    // N - number of detected boxes
    // C - number of classes (excluding background)
    // HxW - segmentation shape
    const int numDetections = static_cast<int>(outDetections.size[2]);
//    const int numClasses = outMasks.size[1];
  //    std::vector<std::vector<float>> objpos;
    outDetections = outDetections.reshape(1, outDetections.total() / 7);
    for (int i = 0; i < numDetections; ++i)
    {
        float score = outDetections.at<float>(i, 2);
        if (score > confThreshold)
        {
            // Extract the bounding box
            int classId = static_cast<int>(outDetections.at<float>(i, 1));
            int left = static_cast<int>(frame.cols * outDetections.at<float>(i, 3));
            int top = static_cast<int>(frame.rows * 1.422222f* outDetections.at<float>(i, 4));
            int right = static_cast<int>(frame.cols * outDetections.at<float>(i, 5));
            int bottom = static_cast<int>(frame.rows*1.422222f * outDetections.at<float>(i, 6));

            left = max(0, min(left, frame.cols - 1));
            top = max(0, min(top, frame.rows - 1));
            right = max(0, min(right, frame.cols - 1));
            bottom = max(0, min(bottom, frame.rows - 1));
            Rect box = Rect(left, top, right - left + 1, bottom - top + 1);
            std::cout << left << " " << top << " " << right << " " << bottom << std::endl;
            std::vector <float> outcoors;
            outcoors.push_back(left);
            outcoors.push_back(top);
            objpos.push_back(outcoors);
            cout <<"image object size"<< objpos.size()<<  endl;
            // Extract the mask for the object
            Mat objectMask(outMasks.size[2], outMasks.size[3], CV_32F, outMasks.ptr<float>(i,classId));
             // Draw bounding box, colorize and show the mask on the image
            drawBox(frame, classId, score, box, objectMask);
            }
        }
    }
    return objpos;
}

//resize the image for 512x512 keeping ratio
//empty space = 0
void invecption_v2::resize(cv::Mat &input, cv::Mat &out)
{
    using namespace cv;
        float n = input.cols / 512;
        std::cout << "mod: "<< n << std::endl;
        Mat rf;
        cv::resize(input,rf,Size(512,static_cast<int>(input.rows/n)));
        std::cout << "fr/n" << (input.rows / n) << std::endl;
        Mat resized;
        resized = cv::Mat::zeros(cv::Size(512, 512), CV_8UC3);

        for (int i = 1; i < rf.rows; ++i)
        {
            for (int j = 1; j < rf.cols; ++j)
            {
                resized.at<Vec3b>(i, j)[0] = rf.at<Vec3b>(i, j)[0];
                resized.at<Vec3b>(i, j)[1] = rf.at<Vec3b>(i, j)[1];
                resized.at<Vec3b>(i, j)[2] = rf.at<Vec3b>(i, j)[2];
            }
        }
        out = resized;
}

//Setup a DNN network
void invecption_v2::setup_dnn_network(const char* cf, const char* model_w, const char* t_g)
{
    using namespace cv;
    using namespace std;
    using namespace dnn;

    // Load names of classes
    string classesFile = cf;          //"d:/dev/cpp/mscoco_labels.names";
 //   std::cout <<
    ifstream ifs(classesFile.c_str());
    string line;
    while (getline(ifs, line)) classes.push_back(line);

    String modelWeights = model_w;    // "d:/dev/cpp/model.pb";
    String textGraph = t_g;          //"d:/dev/cpp/model.pbtxt";
    net = new cv::dnn::Net();
    cout << model_w << endl;
    *net = readNetFromTensorflow(modelWeights, textGraph);
    std::cerr << "Read OK" << std::endl;
    net->setPreferableBackend(DNN_BACKEND_OPENCV);
    //net.setPreferableBackend(DNN_BACKEND_INFERENCE_ENGINE);
    net->setPreferableTarget(DNN_TARGET_CPU);
    //net.setPreferableTarget(DNN_TARGET_OPENCL_FP16);

    //net.setPreferableTarget(DNN_TARGET_OPENCL);
};

std::vector<std::vector<float>> invecption_v2::dnn_inference(cv::Mat& input,cv::Mat& output)
{
    using namespace cv;
    using namespace std;
    using namespace dnn;
     std::vector<std::vector<float>> objpos;
    std::cerr << "Read START" << std::endl;
    // Read image

    cv::Mat frame = input;
    frame.convertTo(frame, CV_8UC3);
    Mat frameResized;
    // resize to 512*512 keeping the ratio
    this->resize(frame,frameResized);
 //   resize(frame, frameResized, Size(1024, 1024));

    // Create a 4D blob from a frame.
    cv::Mat blob;
    blobFromImage(frameResized, blob, 1.0, cv::Size(frameResized.cols, frameResized.rows), cv::Scalar(), true, false);

    std::cerr << "BLOB OK" << std::endl;
    colors.push_back(Scalar(255, 100, 45, 255.0));
    //Sets the input to the network
    net->setInput(blob, "image_tensor");
    net->enableFusion(1);
    // Runs the forward pass to get output from the output layers
    std::vector<String> outNames(2);
    outNames[0] = "detection_out_final";
    outNames[1] = "detection_masks";
    std::vector<cv::Mat> outs;

    {
    std::cerr << "Start..." << std::endl;
        // CHRONO START
    auto start = std::chrono::system_clock::now();

    try
    {
        net->forward(outs, outNames);
    } catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "ex" << std::endl;
    }

    std::cerr << "ok" << std::endl;

    // Extract the bounding box and mask for each of the detected objects
    objpos = postprocess(frame, outs);

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;

        // CHRONO END
    std::cout << "[Inference] elapsed time: " << elapsed_seconds.count() << "s\n";
    }

  //  Mat detectedFrame;
    frame.convertTo(output, CV_8U);
 //   cv::namedWindow("detected object", WINDOW_KEEPRATIO);
 //   imshow("detected object", detectedFrame);
 //   cv::resizeWindow("detected object", 1280, 720);
    waitKey(1);
    return objpos;
}

std::vector<std::vector<float>> invecption_v2::dnn_inference(cv::Mat& input)
{
    using namespace cv;
    using namespace std;
    using namespace dnn;

    std::vector<std::vector<float>> objpos;
    std::cerr << "Read START" << std::endl;
    // Read image

    cv::Mat frame = input;
    frame.convertTo(frame, CV_8UC3);
    Mat frameResized;
    // resize to 512*512 keeping the ratio
    this->resize(frame,frameResized);
 //   resize(frame, frameResized, Size(1024, 1024));

    // Create a 4D blob from a frame.
    cv::Mat blob;
    blobFromImage(frameResized, blob, 1.0, cv::Size(frameResized.cols, frameResized.rows), cv::Scalar(), true, false);

    std::cerr << "BLOB OK" << std::endl;
    colors.push_back(Scalar(255, 100, 45, 255.0));
    //Sets the input to the network
    net->setInput(blob, "image_tensor");
    net->enableFusion(1);
    // Runs the forward pass to get output from the output layers
    std::vector<String> outNames(2);
    outNames[0] = "detection_out_final";
    outNames[1] = "detection_masks";
    std::vector<cv::Mat> outs;

    {
    std::cerr << "Start..." << std::endl;
        // CHRONO START
    auto start = std::chrono::system_clock::now();

    try
    {
        net->forward(outs, outNames);
    } catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "ex" << std::endl;
    }

    std::cerr << "ok" << std::endl;

    // Extract the bounding box and mask for each of the detected objects
    objpos = postprocess(frame, outs);

    auto end = std::chrono::system_clock::now();

        std::chrono::duration<double> elapsed_seconds = end - start;

        // CHRONO END
        std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    }

   // Mat detectedFrame;
//    frame.convertTo(detectedFrame, CV_8U);
 //   cv::namedWindow("detected object", WINDOW_KEEPRATIO);
 //   imshow("detected object", detectedFrame);
 //   cv::resizeWindow("detected object", 1280, 720);
    waitKey(1);
    return objpos;
    //return detectedFrame;
}

matterport_mrcnn::~matterport_mrcnn(){
    //Finishing
    TF_CloseSession(m_session,m_status);
    TF_DeleteSession(m_session,m_status);
    TF_DeleteGraph(m_graph);
    TF_DeleteSessionOptions(m_options);
    TF_DeleteStatus(m_status);

//    m_session.CloseAndDelete(status.get());

}

void matterport_mrcnn::read_graph(const char *modelPB){

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

std::vector<float> matterport_mrcnn::load_anchor(const char *modelPath, const int IMAGE_SIZE){
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

int matterport_mrcnn::select_anchor(){
    //   int maxDim = (std::max)(image.cols, image.rows);
        int maxDim = 1024;
        int anchorSize = ANCHOR_SIZES.back();
        //anchorSize = 512;
        for (const int anchor : ANCHOR_SIZES)
        {
            anchorSize = anchor;
            if (anchorSize >= maxDim) break;
        }

        return anchorSize;
    }

cv::Mat matterport_mrcnn::mold_image(cv::Mat &image, const int IMAGE_SIZE, int maxDim){

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

void matterport_mrcnn::setup_dnn_network( const char* modelPB, const char* modelPath, const char* empty){
    read_graph(modelPB);

    std::cout << "Successfully imported graph" << std::endl;

    static constexpr int NUM_CLASSES = 2;
    static constexpr int METADATA_LEN = 1 + 3 + 3 + 4 + 1 + NUM_CLASSES;
    float IMAGE_METADATA[METADATA_LEN] = { 0.0f,                                                                        //image id
        static_cast<float>(IMAGE_SIZE), static_cast<float>(IMAGE_SIZE), 3.0f,        //original image shape (irrelevant)
        static_cast<float>(IMAGE_SIZE), static_cast<float>(IMAGE_SIZE), 3.0f,        //molded image shape
        0.0f, 0.0f, static_cast<float>(IMAGE_SIZE), static_cast<float>(IMAGE_SIZE),  //window (y1, x1, y2, x2)
        1.0f,                                                                        //scale factor: was 1.0f
        0.0f, 1.0f };                                                                 //classes (NUM_CLASSES)

//TODO the binary
    std::ifstream anchor_file;
    //TODO LOAD ANCHOR ALL ANCHOR? ONLY 1?
    //IMAGE_SIZE = select_anchor();
    m_anchors = load_anchor(modelPath, IMAGE_SIZE);
    this->create_session();
}

std::vector<std::vector<float>> matterport_mrcnn::dnn_inference(cv::Mat& input,cv::Mat& output) {
    std::vector<std::vector<float>> obj = this->inferencing(input);
    output = input;
    return obj;
}

void matterport_mrcnn::create_session(){
    m_options = TF_NewSessionOptions();
    m_status =  TF_NewStatus();
    m_session = TF_NewSession(m_graph, m_options, m_status);
    if (TF_GetCode(m_status) != 0) throw std::runtime_error(std::string("Cannot establish session: ") + TF_Message(m_status));
}

std::vector<std::vector<float>> matterport_mrcnn::inferencing(cv::Mat &image){

    auto start = std::chrono::system_clock::now();
    image.convertTo(image, CV_8UC3);
    cv::resize(image, image, cv::Size(1024, 576));
  //  cv::resize(image, image, cv::Size(512, 288));
    float  nx = 1920.0f/1024.0f;
 //   float  nx = 1920.0f/512.0f;
    const int maxDim = (std::max)(image.cols, image.rows);
    std::vector<std::vector<float>> objpos;
    //IMAGE_SIZE = select_anchor();

    std::cout << "IMAGE_SIZE: " << IMAGE_SIZE << std::endl;

    //matterport's mrcnn only works for image sizes that are multiples of 64
    if(IMAGE_SIZE < 128 || IMAGE_SIZE % 64 != 0) throw std::runtime_error(std::string("Invalid image size: ") + std::to_string(IMAGE_SIZE));

    cv::Mat moldedInput = mold_image(image, IMAGE_SIZE, maxDim);

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
    TF_Operation* inputOpImage = TF_GraphOperationByName(m_graph, "input_image");
    if (inputOpImage == nullptr) throw std::runtime_error("Missing node!");

    TF_Output inputOutImage = { inputOpImage, 0 };
    inputs.push_back(inputOutImage);

    const int64_t inputTensorDimsImage[4] = { 1, moldedInput.rows, moldedInput.cols, moldedInput.channels() };
    TF_Tensor_Ptr inputTensorImage(TF_NewTensor(TF_FLOAT, inputTensorDimsImage, 4, moldedInput.data, sizeof(float) * IMAGE_SIZE * IMAGE_SIZE * 3, DontDeleteTensor, nullptr), TF_DeleteTensor);
    inputTensors.push_back(inputTensorImage.get());

    //meta tensor
    TF_Operation* inputOpMeta = TF_GraphOperationByName(m_graph, "input_image_meta");
    if (inputOpMeta == nullptr) throw std::runtime_error("Missing node!");

    TF_Output inputOutMeta = { inputOpMeta, 0 };
    inputs.push_back(inputOutMeta);

    const int64_t inputTensorDimsMeta[2] = { 1, METADATA_LEN };
    TF_Tensor_Ptr inputTensorMeta(TF_NewTensor(TF_FLOAT, inputTensorDimsMeta, 2, IMAGE_METADATA, sizeof(IMAGE_METADATA), DontDeleteTensor, nullptr), TF_DeleteTensor);
    inputTensors.push_back(inputTensorMeta.get());

    //anchor tensor
    TF_Operation* inputOpAnchor = TF_GraphOperationByName(m_graph, "input_anchors");
    if (inputOpAnchor == nullptr) throw std::runtime_error("Missing node!");

    TF_Output inputOutAnchor = { inputOpAnchor, 0 };
    inputs.push_back(inputOutAnchor);

    const int64_t inputTensorDimsAnchor[3] = { 1, static_cast<int64_t>(m_anchors.size()) / 4, 4 };
    TF_Tensor_Ptr inputTensorAnchor(TF_NewTensor(TF_FLOAT, inputTensorDimsAnchor, 3, m_anchors.data(), m_anchors.size() * sizeof(float), DontDeleteTensor, nullptr), TF_DeleteTensor);
    inputTensors.push_back(inputTensorAnchor.get());


    // ezek
    std::vector<TF_Output> outputs;
    std::vector<TF_Tensor*> outputTensorPtrs;

    TF_Operation* outputOpDetection = TF_GraphOperationByName(m_graph, "mrcnn_detection/Reshape_1");
    if (outputOpDetection == nullptr) throw std::runtime_error("Missing node!");

    TF_Output outputOutDetection = { outputOpDetection, 0 };
    outputs.push_back(outputOutDetection);
    outputTensorPtrs.push_back(nullptr);

    TF_Operation* outputOpMask = TF_GraphOperationByName(m_graph, "mrcnn_mask/Reshape_1");
    if (outputOpMask == nullptr) throw std::runtime_error("Missing node!");

    TF_Output outputOutMask = { outputOpMask, 0 };
    outputs.push_back(outputOutMask);
    outputTensorPtrs.push_back(nullptr);

    std::vector<TF_Tensor_Ptr> outputTensors;
    outputTensors.reserve(outputTensorPtrs.size());

    //Inferencing
    TF_SessionRun(m_session, nullptr,
        inputs.data(), inputTensors.data(), inputs.size(),
        outputs.data(), outputTensorPtrs.data(), outputs.size(),
        nullptr, 0, nullptr, m_status);

    std::cout << "wrapping" << std::endl;
    //Wrapping all raw pointers
    for (TF_Tensor* outputTensor : outputTensorPtrs) outputTensors.emplace_back(TF_Tensor_Ptr(outputTensor, TF_DeleteTensor));
    outputTensorPtrs.clear();

    if (TF_GetCode(m_status) != 0) throw std::runtime_error(std::string("Error during processing: ") + TF_Message(m_status));

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
        for (int j = 0; j < numdim; ++j)
        {
            std::cout << "  " << j << ": " << TF_Dim(outputTensors[i].get(), j) << std::endl;
        }
    }

    const int detectionIncrement = 6;
    const int maskIncrement = maskHeight * maskWidth * NUM_CLASSES;

    //NOTE: only batch size == 1 is tested
    for (int b = 0; b < batchSize; ++b)
    {
        cv::Mat newImage;
        image.copyTo(newImage);

        cv::Mat labels = cv::Mat::zeros(newImage.size(), CV_32SC1);
        //The prediction is always done on squared images
        //If the image is smaller than the used image size, it is padded with zeros, else it is rescaled to that size
        //This float is used to transfer the bounding boxes to the real size
        float restoreDim = (std::max)(static_cast<float>((std::max)(image.rows, image.cols)), static_cast<float>(IMAGE_SIZE));

        //getting tensor data for the batch
        const float* detTensorData = &reinterpret_cast<const float*>(TF_TensorData(outputTensors[MRCNN_DETECTION].get()))[b * maxDet * detectionIncrement];
        float* maskTensorData = &reinterpret_cast<float*>(TF_TensorData(outputTensors[MRCNN_MASK].get()))[b * maxDet * maskIncrement];

        //setting movable pointer
        const float* detTensorPtr = detTensorData;
        float* maskTensorPtr = maskTensorData;

        //iterating through results (moving pointers)
        for (int d = 0; d < maxDet; ++d, detTensorPtr += detectionIncrement, maskTensorPtr += maskIncrement)
        {
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


        //    std:: cout << "t:" << bb[0]*nx << "t:"<< bb[1]*ny<< "t:"<< bb[2]<< "t:" <<bb[3] << std::endl;

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

            //saving coordinates and features
            float bx = static_cast<float>(bb[0])*nx;
            float by = static_cast<float>(bb[1]*nx);
            float bb_x = static_cast<float>(bb[2])*nx;
            float bb_y = static_cast<float>(bb[3])*nx;
            std::vector <float> outcoors;
            outcoors.push_back( bx);
            outcoors.push_back(by);
            imagetools asd;
            std::vector<float> features = asd.getobjectprops(label);
            outcoors.push_back(features.at(0)*nx);//length
            outcoors.push_back(features.at(1)*nx*nx);//area
            outcoors.push_back(features.at(2));//circularity
            outcoors.push_back((features.at(3)*nx) + bx); //mx // resize and recoordinate
            outcoors.push_back((features.at(4)*nx) + by); //my
            objpos.push_back(outcoors);
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
                imgPtr[(index * 3) +1] = 255;
                imgPtr[(index * 3) +2] = 90;
            }
        }

        auto end = std::chrono::system_clock::now();
        //
        std::chrono::duration<double> elapsed_seconds = end - start;

        // CHRONO END
        std::cout << "[INFERENCE] elapsed time: " << elapsed_seconds.count() << "s\n";
        cv::resize(newImage, newImage, cv::Size(1920, 1080));
        newImage.copyTo(image);
    }
    return objpos;
}


