#include "code.h"
#include <cstdlib>

Decode::Decode(int width,int height,Channel ch):width(width),height(height),ch(ch){
	if(ch==Channel::ONE){
		bufferSize=width*height;
	}else if(ch==Channel::THREE){
		bufferSize=width*height*3;
	}
	buffer=(uchar*)malloc(bufferSize);
}

Decode::~Decode(){
	free(buffer);
}

uchar* Decode::getBuffer(){
	return buffer;
}

int Decode::size(){
	return bufferSize;
}

cv::Mat& Decode::toMat(cv::Mat& mat){
	int count=0;
	if(ch==Channel::ONE){
		for(int i=0;i<height;++i){
			for(int j=0;j<width;++j){
				mat.at<uchar>(i,j) = buffer[count++];
			}
		}		
	}else if(ch==Channel::THREE){
		for(int i=0;i<height;++i){
			for(int j=0;j<width;++j){
				mat.at<cv::Vec3b>(i,j)[0] = buffer[count++];
				mat.at<cv::Vec3b>(i,j)[1] = buffer[count++];
				mat.at<cv::Vec3b>(i,j)[2] = buffer[count++];
			}
		}
	}
	return mat;
}

cv::Mat Decode::newMat(){
	if(ch==Channel::ONE){
		cv::Mat mat(height,width,CV_8UC1,cv::Scalar::all(0));
	        return mat;
	}else if(ch==Channel::THREE){
	        cv::Mat mat(height,width,CV_8UC3,cv::Scalar::all(0));
		return mat;
	}
	cv::Mat mat(height,width,CV_8UC1,cv::Scalar::all(0));
	return mat;
}

