#include "imagetools.h"
#include <thread>

cv::Mat* imagetools::getframe()
{
    return (this->frame);
}

void imagetools::setframe(cv::Mat &input){
    if (this->frame != nullptr){
     //   delete[] (this->frame);
        this->frame->release();
        delete this->frame;
    }
    //kelle pointer ha igen unique de sima cv mat is jolesz
    this->frame = new cv::Mat();
    this->frame = &input;
}

/*
0. CV_CAP_PROP_POS_MSEC Current position of the video file in milliseconds.
1. CV_CAP_PROP_POS_FRAMES 0-based index of the frame to be decoded/captured next.
2. CV_CAP_PROP_POS_AVI_RATIO Relative position of the video file
3. CV_CAP_PROP_FRAME_WIDTH Width of the frames in the video stream.
4. CV_CAP_PROP_FRAME_HEIGHT Height of the frames in the video stream.
5. CV_CAP_PROP_FPS Frame rate.
6. CV_CAP_PROP_FOURCC 4-character code of codec.
7. CV_CAP_PROP_FRAME_COUNT Number of frames in the video file.
8. CV_CAP_PROP_FORMAT Format of the Mat objects returned by retrieve() .
9. CV_CAP_PROP_MODE Backend-specific value indicating the current capture mode.
10. CV_CAP_PROP_BRIGHTNESS Brightness of the image (only for cameras).
11. CV_CAP_PROP_CONTRAST Contrast of the image (only for cameras).
12. CV_CAP_PROP_SATURATION Saturation of the image (only for cameras).
13. CV_CAP_PROP_HUE Hue of the image (only for cameras).
14. CV_CAP_PROP_GAIN Gain of the image (only for cameras).
15. CV_CAP_PROP_EXPOSURE Exposure (only for cameras).
16. CV_CAP_PROP_CONVERT_RGB Boolean flags indicating whether images should be converted to RGB.
17. CV_CAP_PROP_WHITE_BALANCE Currently unsupported
18. CV_CAP_PROP_RECTIFICATION Rectification flag for stereo cameras (note: only supported by DC1394 v 2.x backend currently)
*/

cv::Mat* imagetools::get_display_frm()
{
    return this->dispfrm.get();
}

cv::Mat imagetools::convert_bgr_to_rgb(QSharedPointer<cv::Mat> pinput){
    cv::Mat rgb;
    cvtColor(*pinput,rgb,CV_BGR2RGB,0);
    return rgb;
}

int2coors imagetools::getSphCoors(cv::Mat &img){
    using namespace   std;
    int2coors sphcoors;
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_RGB2GRAY);
    cv::Mat out,out2;

    //	threshold(gray, out, 105, 255, THRESH_BINARY);
    threshold(gray, out, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    //	adaptiveThreshold(gray, out2, 255, CV, THRESH_BINARY,27,2);

    bitwise_not(out, out);

    cv::Mat label ;

//  int la = connectedComponents(out, label,8, CV_16U);

    cv::Mat labels, stats, centroids;

    int nLabels = connectedComponentsWithStats(out, label,stats, centroids, 8, CV_16U);

    cv::Mat seeMyLabels;

    cv::normalize(label, seeMyLabels,0, 255, cv::NORM_MINMAX, CV_8U);

    cv::applyColorMap(seeMyLabels, seeMyLabels, cv::COLORMAP_JET);

    cout << "Number of connected components = " << nLabels << endl << endl;
        // Statistics
    cout << "Show statistics and centroids:" << endl;
    cout << "stats:" << endl << "(left,top,width,height,area)" << endl << stats << endl << endl;
    cout << "centroids:" << endl << "(x, y)" << endl << centroids << endl << endl;

        // Print individual stats for component 1 (component 0 is background)
    cout << "Component 1 stats:" << endl;

    std::cout << "CC_STAT_LEFT   = " << stats.at<int>(1, cv::CC_STAT_LEFT) << endl;
    cout << "CC_STAT_TOP    = " << stats.at<int>(1, cv::CC_STAT_TOP) << endl;
    cout << "CC_STAT_WIDTH  = " << stats.at<int>(1,cv:: CC_STAT_WIDTH) << endl;
    cout << "CC_STAT_HEIGHT = " << stats.at<int>(1,cv:: CC_STAT_HEIGHT) << endl;
    cout << "CC_STAT_AREA   = " << stats.at<int>(1,cv:: CC_STAT_AREA) << endl;

    return sphcoors;

}

// returns a feature vector contour length,area, and circularity
std::vector<double> imagetools::getobjectprops(cv::Mat& input){
    using namespace cv;
        using namespace std;
        input.convertTo(input,CV_8UC1);
    //	waitKey(0);
        Mat canny_output;
        RNG rng(12345);
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        //cvtColor(src, src_gray, CV_BGR2GRAY);
        /// Detect edges using canny
       // copyMakeBorder(input, input, 10, 10, 10, 10, BORDER_CONSTANT, 0);

        Canny(input, canny_output, 0, 255, 3);
        /// Find contours
    //	imshow("asd", canny_output);
        findContours(input, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
        cout << contours.size() << "sizeof c" << endl;
        /// Get the moments
        vector<Moments> mu(contours.size());
        for (int i = 0; i < 1; i++)
        {
            mu[i] = moments(contours[i], true);
        }

        ///  Get the mass centers:
        vector<Point2f> mc(1);
        for (int i = 0; i < 1; i++)
        {
            mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
        }
        std::vector<double> object_features(3,0.0f);
        // Draw contours
        Mat drawing = Mat::zeros(canny_output.size(), CV_32SC1);
        for (int i = 0; i< 1; ++i)
        {
            double area = contourArea(contours[i]);
            double arclength = arcLength(contours[i], true);
            object_features.push_back(arclength);
            object_features.push_back( area);
            object_features.push_back( 4 * CV_PI * area / (arclength * arclength));

            // cout << i << "object circularity " << circularity << endl;
            Scalar color = Scalar(255,  255, 255);
            drawContours(drawing, contours, i, color, 1, 8, hierarchy, 0, Point());
       //     circle(drawing, mc[i], 4, color, -1, 8, 0);
        }

        input = drawing;
        return object_features;
}

void imagetools::addPointToImage(cv::Mat& img,cv::Point point)
{
   cv::circle(img,point, 5, cv::Scalar(0,0,255), -1);
}

// Uses OPENCV save image as png UNCOMPRESSED
void imagetools::saveImg(cv::Mat* outimg, std::string outname)
{
    //NULLCHECK
    if (outimg == nullptr){return;}
    outname = outname + ".png";
    std::vector<int> compression_params;
    compression_params.push_back(16);
    compression_params.push_back(0);
    imwrite(outname, *outimg, compression_params);
//    cout << "image saved as " << outname.str() << endl;

}
