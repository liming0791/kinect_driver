#include "stdio.h"
#include "stdlib.h"

#include <iostream>
#include <fstream>

#include <opencv2/opencv.hpp>

#include "KinectDriver.h"

using namespace std;

int main(int argc, char** argv)
{

    int width, height;
    int16_t *depthPtr;
    unsigned char * colorPtr;
    long long depthTime = 0, colorTime = 0 ;
    int frameid = 0;
    char name[50] = "";

    cv::namedWindow("depth");
    cv::namedWindow("color");

    KinectDriver kinectDevice;
    kinectDevice.initOni(&width, &height);

    depthPtr = (int16_t *) malloc(width*height*2*sizeof(char));
    colorPtr = (unsigned char *)malloc(width*height*3*sizeof(char));

    cv::Mat depthMat(height, width, CV_16UC1, (char *)depthPtr);
    cv::Mat colorMat(height, width, CV_8UC3, (char *)colorPtr);

    kinectDevice.begin();

    while( cv::waitKey(3) != 'q')
    {
        kinectDevice.getFrameData((int16_t *)depthMat.data, (unsigned char*)colorMat.data, depthTime, colorTime );

        cv::cvtColor(colorMat, colorMat, CV_RGB2BGR);
        cv::imshow("color", colorMat);
        cv::imshow("depth", depthMat);

        frameid++;
    }

    kinectDevice.close();

    return 0;
}
