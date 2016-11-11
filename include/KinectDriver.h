#include "stdlib.h"
#include "stdio.h"

#include <chrono>

#include <mutex>

#include "OniReader.h"

class KinectDriver
{
    public:
        KinectDriver();
        ~KinectDriver();
        bool init(int* width, int* height);
        bool initOni(int* width = NULL, int* height = NULL);
        void setFrameCallback(
                boost::function< void (int16_t*, unsigned char*, 
                    long long , long long) >& callback);
        void getFrameData(int16_t* depthPtr, 
                unsigned char* colorPtr, 
                long long& depthTime, 
                long long& colorTime);
        void close();
        void begin();
        void stop();
        void FrameReceived(VideoFrameRef& depthFrame, VideoFrameRef& colorFrame);

    private:
        OniReader oni;

        unsigned char* ColorFrame;
        int16_t* DepthFrame;

        int _width, _height;
        long long base_time_stamp;
        long long color_time_stamp;
        long long depth_time_stamp;
        long long last_color_time_stamp;
        long long last_depth_time_stamp;

        std::mutex frame_mutex;
        std::mutex data_mutex;

        bool oniInited;
        bool started;
        bool firstFrame;

        std::chrono::high_resolution_clock::time_point imu_base_time;
        std::chrono::high_resolution_clock::time_point imu_last_time;

        boost::function< void (int16_t*, unsigned char*, long long , long long) > frameCallback;
        boost::function< void (float, float, float, float, float, float, long long) > imuCallback;
};
