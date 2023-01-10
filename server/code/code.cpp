#include "code.h"
#include <cstdlib>

Encode::Encode(int width, int height, Channel ch) : width(width), height(height), ch(ch)
{
	if (ch == Channel::ONE)
	{
		bufferSize = width * height;
	}
	else if (ch == Channel::THREE)
	{
		bufferSize = width * height * 3;
	}
	buffer = (uchar *)malloc(bufferSize);
}

Encode::~Encode()
{
	free(buffer);
}

uchar *Encode::getBuffer()
{
	return buffer;
}

int Encode::size()
{
	return bufferSize;
}

void Encode::matTo(cv::Mat &mat)
{
	int count = 0;
	if (ch == Channel::ONE)
	{
		for (int i = 0; i < height; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				buffer[count++] = mat.at<uchar>(i, j);
			}
		}
	}
	else if (ch == Channel::THREE)
	{
		for (int i = 0; i < height; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				buffer[count++] = mat.at<cv::Vec3b>(i, j)[0];
				buffer[count++] = mat.at<cv::Vec3b>(i, j)[1];
				buffer[count++] = mat.at<cv::Vec3b>(i, j)[2];
			}
		}
	}
}
