
#include "deeplearning.h"
deeplearning::deeplearning()
{



}


deeplearning::~deeplearning(){}

// Draw the predicted bounding box, colorize and show the mask on the image
void deeplearning::drawBox(cv::Mat& frame, int classId, float conf, cv::Rect box, cv::Mat& objectMask)
{
    using namespace cv;
    using namespace dnn;
    using namespace std;
    //Draw a rectangle displaying the bounding box
    cv::rectangle(frame, Point(box.x, box.y), Point(box.x+box.width, box.y+box.height), Scalar(255, 178, 50), 3);

    //Get the label for the class name and its confidence
    string label = format("%.2f", conf);
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
    resize(objectMask, objectMask, Size(box.width, box.height));
    Mat mask = (objectMask > maskThreshold);
    Mat coloredRoi = (0.5 * colors[classId%colors.size()] + 0.5 * frame(box));
    coloredRoi.convertTo(coloredRoi, CV_8UC3);
    coloredRoi.copyTo(frame(box), mask);
}

// For each frame, extract the bounding box and mask for each detected object
void deeplearning::postprocess(cv::Mat& frame, const std::vector<cv::Mat>& outs)
{
    using namespace cv;
    using namespace std;
    std::cout << "Outs: " << outs.size() << std::endl;

    if (outs.size() < 2) return;

    Mat outDetections = outs[0];
    Mat outMasks = outs[1];

    // Output size of masks is NxCxHxW where
    // N - number of detected boxes
    // C - number of classes (excluding background)
    // HxW - segmentation shape
    const int numDetections = outDetections.size[2];
    const int numClasses = outMasks.size[1];

    outDetections = outDetections.reshape(1, outDetections.total() / 7);
    for (int i = 0; i < numDetections; ++i)
    {
        float score = outDetections.at<float>(i, 2);
        if (score > confThreshold)
        {
            // Extract the bounding box
            int classId = static_cast<int>(outDetections.at<float>(i, 1));
            int left = static_cast<int>(frame.cols * outDetections.at<float>(i, 3));
            int top = static_cast<int>(frame.rows * outDetections.at<float>(i, 4));
            int right = static_cast<int>(frame.cols * outDetections.at<float>(i, 5));
            int bottom = static_cast<int>(frame.rows * outDetections.at<float>(i, 6));

            left = max(0, min(left, frame.cols - 1));
            top = max(0, min(top, frame.rows - 1));
            right = max(0, min(right, frame.cols - 1));
            bottom = max(0, min(bottom, frame.rows - 1));
            Rect box = Rect(left, top, right - left + 1, bottom - top + 1);

            std::cout << left << " " << top << " " << right << " " << bottom << std::endl;

            // Extract the mask for the object
            Mat objectMask(outMasks.size[2], outMasks.size[3], CV_32F, outMasks.ptr<float>(i,classId));

            // Draw bounding box, colorize and show the mask on the image
            drawBox(frame, classId, score, box, objectMask);

        }
    }
}

void deeplearning::rundnn(cv::Mat& input){
    using namespace cv;
    using namespace std;
    using namespace dnn;

    // Load names of classes
    //string classesFile = "/media/baran/B8703F33703EF828/baran/Devel/OpencvTest/BUILD/CL2M/cl2m.names";
    string classesFile = "d:/dev/cpp/mscoco_labels.names";
    ifstream ifs(classesFile.c_str());
    string line;
    while (getline(ifs, line)) classes.push_back(line);

    // Load the colors
    //std::vector<Scalar> colors;
    string colorsFile = "colors.txt";
    ifstream colorFptr(colorsFile.c_str());
    while (getline(colorFptr, line)) {
        char* pEnd;
        double r, g, b;
        r = strtod (line.c_str(), &pEnd);
        g = strtod (pEnd, NULL);
        b = strtod (pEnd, NULL);
        colors.push_back(Scalar(r, g, b, 255.0));
    }

    String modelWeights = "d:/dev/cpp/frozen_inference_graph_sort.pb";
    String textGraph = "d:/dev/cpp/frozen_inference_graph_sort.pbtxt";

    std::cerr << "Read START" << std::endl;

    // Load the network

    Net net = readNetFromTensorflow(modelWeights, textGraph);

    std::cerr << "Read OK" << std::endl;

    net.setPreferableBackend(DNN_BACKEND_OPENCV);
    //net.setPreferableBackend(DNN_BACKEND_INFERENCE_ENGINE);
    //net.setPreferableTarget(DNN_TARGET_CPU);
    //net.setPreferableTarget(DNN_TARGET_OPENCL_FP16);

    net.setPreferableTarget(DNN_TARGET_OPENCL);

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


    // Read image

     cv::Mat frame = input;
     frame.convertTo(frame, CV_8UC3);

    Mat frameResized = frame;
 //   resize(frame, frameResized, Size(1024, 1024));

    // Create a 4D blob from a frame.
    cv::Mat blob;
    blobFromImage(frameResized, blob, 1.0, cv::Size(frameResized.cols, frameResized.rows), cv::Scalar(), true, false);

    std::cerr << "BLOB OK" << std::endl;

    //Sets the input to the network
    net.setInput(blob, "image_tensor");
    net.enableFusion(1);
    // Runs the forward pass to get output from the output layers
    std::vector<String> outNames(2);
    outNames[0] = "detection_out_final";
    outNames[1] = "detection_masks";
    std::vector<cv::Mat> outs;

    //for (int i = 0; i < 3; ++i)
    {
        std::cerr << "Start..." << std::endl;

        // CHRONO START
        auto start = std::chrono::system_clock::now();

    try
    {
            net.forward(outs, outNames);
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
    postprocess(frameResized, outs);


    auto end = std::chrono::system_clock::now();

        std::chrono::duration<double> elapsed_seconds = end - start;

        // CHRONO END
        std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    }

    //Mat frameResult;
    //resize(frameResized, frameResult, Size(frame.cols, frame.rows));
    // Write the frame with the detection boxes
    Mat detectedFrame;

    frameResized.convertTo(detectedFrame, CV_8U);

    imshow("test", detectedFrame);
    //imshow("asdasd", frame);
    waitKey(0);
    imwrite("result.jpg", detectedFrame);
    waitKey(0);
}
