#include <opencv/cv.h>
#include <opencv/highgui.h>

int main()
{
    CvCapture* cam = cvCaptureFromCAM(0);
    cvReleaseCapture(&cam);
    return 0;
}
