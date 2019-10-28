#include "deeplearning.h"

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
void invecption_v2::setup_dnn_network(std::string cf, std::string model_w, std::string t_g)
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
        std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
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

void keras_mrcnn::setup_dnn_network( const char* modelPB, const char* modelPath){
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
    std::vector<float> anchors = load_anchor(modelPath, IMAGE_SIZE);
    this->create_session();

}
