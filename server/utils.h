#pragma once
#include <opencv2/opencv.hpp>
#include <string.h>

class Utils
{
public:
    static void newWindow(std::string name)
    {
        cv::namedWindow(name.c_str());
    }
};