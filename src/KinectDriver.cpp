#include "KinectDriver.h"
#include <chrono>
#include "opencv2/opencv.hpp"

using namespace std;

KinectDriver::KinectDriver()
{
    oniInited = false;
    started = false;
    firstFrame = false;

    ColorFrame = NULL;
    DepthFrame = NULL;
}

KinectDriver::~KinectDriver()
{
    if(ColorFrame){
        delete(ColorFrame);
        ColorFrame = NULL;
    }

    if(DepthFrame){
        delete(DepthFrame);    
        DepthFrame = NULL;
    }
}

/*
 * INIT
 */

bool KinectDriver::init(int* width, int* height)
{
    return (initOni(width, height));
}

bool KinectDriver::initOni(int* width, int* height)
{
    printf("init oni......\n");
    if(oniInited){
        printf("OpenNI has inited !");
        return true;
    }
    if(!oni.init(&_width, &_height)) 
        return false;
    oni.setCallback(boost::bind(&KinectDriver::FrameReceived, this, _1, _2));
    if(width!=NULL) *width = _width;
    if(height!=NULL) *height = _height;
    ColorFrame = (unsigned char*)malloc(_width*_height*3*sizeof(char));
    DepthFrame = (int16_t *)malloc(_width*_height*2*sizeof(char));
    oniInited = true;
    printf("init oni done\n");
    return true;
}

void KinectDriver::FrameReceived(
        VideoFrameRef& depthFrame, 
        VideoFrameRef& colorFrame)
{

    DepthPixel* pDepth;
    RGB888Pixel* pColor;
    int height = colorFrame.getHeight();
    int width = colorFrame.getWidth();

    pDepth = (DepthPixel*)depthFrame.getData();
    pColor = (RGB888Pixel*)colorFrame.getData();

    {
        std::lock_guard<std::mutex> lock(data_mutex);

        memcpy(DepthFrame, pDepth, height*width*2);
        depth_time_stamp = (long long)depthFrame.getTimestamp();
        memcpy(ColorFrame, pColor, height*width*3);
        color_time_stamp = (long long)colorFrame.getTimestamp();
    }

    if(firstFrame){
        base_time_stamp = depth_time_stamp < color_time_stamp ? depth_time_stamp : color_time_stamp;
        firstFrame = false;
//        printf("base frame timestamp: %lld\n", base_time_stamp);
    }else{
        depth_time_stamp -= base_time_stamp;
        color_time_stamp -= base_time_stamp;
    } 

}

/*
 * SET CALLBACK
 */

void KinectDriver::setFrameCallback(
        boost::function< void (int16_t*, unsigned char*, long long, long long)>& callback)
{
    frameCallback = callback;
}

/*
 * BEGIN
 */

void KinectDriver::begin()
{
    if(started){
        printf("already started !");
        return;
    }

    started = true;
    oni.begin();
    printf("start oni_imu\n");
}

/*
 * GETDATA
 */

//get oni data
void KinectDriver::getFrameData(int16_t* depthPtr, unsigned char* colorPtr, 
        long long& depthTime, long long& colorTime)
{
    if(!started){
        printf(" Oni_IMU Has not started !\n");
        memset(depthPtr, 0, _width*_height*2);
        memset(colorPtr, 0, _width*_height*3);
        return;
    }

    {
        std::lock_guard<std::mutex> lock_data(data_mutex);
        memcpy(depthPtr, DepthFrame, _width*_height*2*sizeof(char));
        memcpy(colorPtr, ColorFrame, _width*_height*3*sizeof(char));  

        depthTime = depth_time_stamp;
        colorTime = color_time_stamp;
    }
}

/*
 *STOP
 */

void KinectDriver::stop()
{

}

/*
 * CLOSE
 */

void KinectDriver::close()
{
    oni.close();
}
