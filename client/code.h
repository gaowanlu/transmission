#pragma once
#include <opencv2/opencv.hpp>

class Decode{
public:
	enum Channel{
		ONE=0,
		THREE
	};
	Decode(int width,int height,Channel ch);
	~Decode();
	uchar* getBuffer();
	int size();
	cv::Mat& toMat(cv::Mat& mat);
	cv::Mat newMat();
private:
	int width;
	int height;
	Channel ch;
	uchar* buffer;
	int bufferSize;
};
