// Script publisher utk komunikasi satu arah

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "cv_bridge/cv_bridge.h"
#include "image_transport/image_transport.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

// Global Variables

Mat frame, erosion_window, dilation_window, element_erosion, element_dilation, hsv_edit, temp, final;
sensor_msgs::ImagePtr msg;
int erosion_value = 1, dilation_value = 1, erosion_type = 1, dilation_type = 1;
int hue_val_min = 1, saturation_val_min = 1, value_val_min = 1;
int hue_val_max = 179, saturation_val_max = 255, value_val_max = 255;
const int maximum_value = 10;
const int maximum_element = 3;

void erosion(int, void *);
void dilation(int, void *);
Mat mask;

int main(int argc, char **argv)
{

    ros::init(argc, argv, "publisher");

    ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);
    image_transport::Publisher pub = it.advertise("video", 1);
    
    // Local Variables
    VideoCapture cap(0);

    // Standard failsafe for the webcam
    if(!cap.isOpened())
    {
        cout << "cam error" << endl;
        return -1;
    }

    cv::namedWindow("filters", WINDOW_AUTOSIZE);

    // Trackbars
    // Trackbar to change HSV values
    createTrackbar("hue min ", "filters", &hue_val_min, 179);
    createTrackbar("hue max ", "filters", &hue_val_max, 179);
    createTrackbar("sat min", "filters", &saturation_val_min, 255);
    createTrackbar("sat max", "filters", &saturation_val_max, 255);
    createTrackbar("val min", "filters", &value_val_min, 255);
    createTrackbar("val max", "filters", &value_val_max, 255);
    // Trackbar to change erosion element value
    createTrackbar("erosion element: \n 1. rectangle \n 2. cross \n 3. ellipse", "filters",
                   &erosion_type, maximum_element, erosion);
    // Trackbar to change erosion value
    createTrackbar("erosion ", "filters", &erosion_value, maximum_value, erosion);
    // Trackbar to change dilation element value
    createTrackbar("dilation element: \n 1. rectangle \n 2. cross \n 3. ellipse", "filters",
                   &dilation_type, maximum_element, dilation);
    // Trackbar to change dilation value
    createTrackbar("dilation ", "filters", &dilation_value, maximum_value, dilation);

    // This loops captures frame from cam and turn it into Mat data type (OpenCV image)
    // Then converts it into ROS image and publish it to a subscriber
        
    while (ros::ok()){
    cap.read(temp);

    if (temp.empty())
    {
        ROS_INFO("Cannot read from webcam \n");
    }
    cvtColor(temp, frame, CV_RGB2HSV);

    inRange(frame,
                Scalar(hue_val_min, saturation_val_min, value_val_min),
                Scalar(hue_val_max, saturation_val_max, value_val_max),
                mask);
    bitwise_and(frame, frame, temp, mask = mask);

    erode(temp, erosion_window, element_erosion);
    dilate(erosion_window, dilation_window, element_dilation);

    cvtColor(erosion_window, final, CV_HSV2RGB);

    imshow("filters", final);

    msg = cv_bridge::CvImage(std_msgs::Header (), "bgr8", final).toImageMsg();
    pub.publish(msg);

    waitKey(1);}

    return 0;
}

// This function processes the variable from the erosion trackbar
void erosion(int, void*)
{
    int type;
    switch(erosion_type)
    {
        case 1: type = MORPH_RECT;
        case 2: type = MORPH_CROSS;
        case 3: type = MORPH_ELLIPSE;
    }
    element_erosion = getStructuringElement(type,
                                        Size(2 * erosion_value + 1, 2 * erosion_value + 1),
                                        Point (erosion_value, erosion_value));
}

// This function processes the variable from the dilation trackbar
void dilation(int, void*)
{
    int type;
    switch(dilation_type)
    {
        case 1: type = MORPH_RECT;
        case 2: type = MORPH_CROSS;
        case 3: type = MORPH_ELLIPSE;
    }
    element_dilation = getStructuringElement(type,
                                        Size(2 * dilation_value + 1, 2 * dilation_value + 1),
                                        Point(dilation_value, dilation_value));
}

//TO RUN NODE SIMULTANEOUSLY: 
// rosrun vision_ros publisher __name:=client1
// rosrun vision_ros publisher __name:=client2
