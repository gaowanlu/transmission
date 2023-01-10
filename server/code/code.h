#pragma once
#include <opencv2/opencv.hpp>

class Encode
{
public:
	enum Channel
	{
		ONE = 0,
		THREE
	};
	Encode(int width, int height, Channel ch);
	~Encode();
	uchar *getBuffer();
	int size();
	void matTo(cv::Mat &mat);

private:
	int width;
	int height;
	Channel ch;
	uchar *buffer;
	int bufferSize;
};
