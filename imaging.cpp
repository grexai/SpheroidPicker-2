#include "imaging.h"


imaging::imaging()
{

}


void imaging::cameraloop() {
    while (1) {
        Mat frame;
        cap >> frame;
        imshow("Camera Live", frame);
        waitKey(1);
    }
}
