// Script subscriber utk komunikasi satu arah

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "cv_bridge/cv_bridge.h"
#include "image_transport/image_transport.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

static const std::string WINDOW_TITLE = "display";

class ImageConverter
{
    ros::NodeHandle nh;
    image_transport::ImageTransport it;
    image_transport::Subscriber sub;

    public:
        ImageConverter() : it(nh)
        {
            cv::namedWindow(WINDOW_TITLE, cv::WINDOW_FREERATIO);
            sub = it.subscribe("video", 1, &ImageConverter::imageCb, this);

        }

        ~ImageConverter() {cv::destroyWindow(WINDOW_TITLE);}

        void imageCb (const sensor_msgs::ImageConstPtr &msg)
        {
            cv_bridge::CvImagePtr ptr;

            try {
                ptr = cv_bridge::toCvCopy(msg); 
            }

            catch (cv_bridge::Exception &e) {
                ROS_ERROR("cv_bridge exception: %s", e.what());
                return;
            }

            cv::imshow(WINDOW_TITLE, ptr->image);

            cv::waitKey(1);
        }
};

int main (int argc, char **argv)
{
    ros::init(argc, argv, "subscriber");
    ImageConverter ic;
    ros::spin();
    
    return 0;
}

