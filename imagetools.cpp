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

cv::Mat* imagetools::get_display_frm()
{
    return this->dispfrm.get();
}

cv::Mat imagetools::convert_bgr_to_rgb(QSharedPointer<cv::Mat> p_input){
    cv::Mat rgb;
   // cvtColor(*p_input,rgb,CV_BGR2RGB,0);
    cvtColor(*p_input,rgb,cv::COLOR_BGR2RGB,0);
    return rgb;
}

int2coords imagetools::getSphCoors(cv::Mat &img){
    using namespace   std;
    int2coords sphcoors;
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

/*!
 * Returns a feature vector in the following order
 * contour length,area, circularity , masscenters: mx,my
 * Handles only one object
 */

std::vector<float> imagetools::getobjectprops(cv::Mat& input){
        using namespace cv;
        using namespace std;
        cv::Mat c_input;
        input.convertTo(c_input,CV_8UC1);
    //	waitKey(0);
        std::vector<float> object_features(7,0.0f);
        Mat canny_output;
        RNG rng(12345);
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        //cvtColor(src, src_gray, CV_BGR2GRAY);
        /// Detect edges using canny
       // copyMakeBorder(input, input, 10, 10, 10, 10, BORDER_CONSTANT, 0);

        Canny(c_input, canny_output, 0, 255, 3);
        /// Find contours
    //	imshow("asd", canny_output);
        findContours(c_input, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
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
        // Draw contours
        float radius;
        Point2f center;


        Mat drawing = Mat::zeros(canny_output.size(), CV_32SC1);
        int i =0;
        double area = contourArea(contours[i]);
        double arclength = arcLength(contours[i], true);
        double circularity = (4 * CV_PI * area / (arclength * arclength));

        drawContours(drawing, contours, i, Scalar(255,  255, 255), 1, 8, hierarchy, 0, Point());
        minEnclosingCircle(contours[0],center,radius);
        double sphericity = CV_PI*sqrt(4  * area/ CV_PI )/ (arclength);
        object_features.at(0) = (static_cast <float>(arclength));
        object_features.at(1) =  static_cast <float>(area);
        object_features.at(2)= static_cast <float>(circularity);
        object_features.at(3) = mc[0].x;
        object_features.at(4) = mc[0].y;
        object_features.at(5) = radius;
        object_features.at(6) = sphericity;
        // ADD solidity

        // area/convex area
        //
        cout << "object perimeter raw " << object_features.at(0) << std::endl;

        cout << "object max radius raw" << radius << std::endl;
        cout << "object max radius raw" << sphericity << std::endl;

     //   input = drawing;
        return object_features;
}

void imagetools::addPointToImage(cv::Mat& img,cv::Point point)
{
   cv::circle(img,point, 5, cv::Scalar(0,0,255), -1);
}

/*!
 * Saves Mat image as Uncompressed png,
 *
 */

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
