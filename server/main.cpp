#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <opencv2/opencv.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include "msg_header.h"
#include "code.h"

#define COLOR 0

using namespace std;

int main(int argc, char **argv)
{
    cv::namedWindow("show1");
    // 创建监听套接字
    int server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int client = -1;
    if (server <= 0)
    {
        cout << "监听套接字创建失败" << endl;
        return 1;
    }
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("0.0.0.0");
    server_address.sin_port = htons(8888); // 转网络字节序
    bind(server, (struct sockaddr *)&server_address, sizeof(server_address));
    listen(server, 20);
    cout << "正在监听" << endl;
    socklen_t client_len = sizeof(sockaddr_in);
    // 打开相机
    cv::Mat m_mat;
    cv::VideoCapture capture(0);
    if (!capture.isOpened())
    {
        cout << "相机打开失败" << endl;
        return 1;
    }
    capture >> m_mat;
    if (m_mat.empty())
    {
        return 1;
    }
    msg_header msg;
    msg.width = m_mat.cols;
    msg.height = m_mat.rows;
#if COLOR
    Encode encode(msg.width,msg.height,Encode::THREE);
    msg.channel = Encode::Channel::THREE;
#else
    Encode encode(msg.width,msg.height,Encode::ONE);
    cvtColor(m_mat,m_mat,cv::COLOR_BGR2GRAY);
    msg.channel = Encode::Channel::ONE;
#endif
    msg.frame_size = encode.size();
    // 循环发送图像
    while (1)
    {
	//客户端连接
	if(client == -1){
		client = accept(server,(struct sockaddr*)&client_address,&client_len);
		if(client<0){
			cout<<"accept error: "<<errno<<endl;
			return -1;
		}
		//发送协议头
		if(sizeof(msg_header) != write(client,&msg,sizeof(msg))){
			cout<<"头部信息发送出错"<<endl;
			continue;
		}
	}
	//读帧与编码
        capture >> m_mat;
        if (m_mat.empty())
        {
            cout<<"读帧失败"<<endl;
            continue;
        }
#if COLOR
#else
	cvtColor(m_mat,m_mat,cv::COLOR_BGR2GRAY);	
#endif
	encode.matTo(m_mat);
	cv::imshow("show1",m_mat);
	cv::waitKey(1);
	//发送头部
        ssize_t count = write(client, &msg, sizeof(msg_header));
	if(count!=sizeof(msg_header)){
	    close(client);
	    client = -1;
	    continue;
	}
        { // 发送编码内容
            int buffer_len = 0,sended = 0;
	    uchar buffer[2048];
            while (sended < encode.size())
            {
                for (; sended < encode.size();)
                {
                    buffer[buffer_len++] = encode.getBuffer()[sended++];
                    if (buffer_len == 2048)
                    {
                        break;
                    }
                }
                ssize_t count = write(client, buffer, buffer_len);
		if(!count){
		    close(client);
		    client = -1;
	            continue;
		}
                buffer_len = 0;
            }
        }
        //  等待反馈
        int len = 0;
        int temp = 0;
        while (len != sizeof(msg_header))
        {
            temp = read(client, &msg, sizeof(msg_header) - len);
            if (temp <= 0)
            {
                close(client);
		client = -1;
		break;
            }
            len += temp;
        }
    }
    return 0;
}
