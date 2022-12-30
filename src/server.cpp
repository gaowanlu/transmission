// g++ `pkg-config opencv4 --cflags` server.cpp -o server `pkg-config opencv4 --libs`
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

using namespace std;

int main(int argc, char **argv)
{
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
    server_address.sin_port = htons(2023); // 转网络字节序
    bind(server, (struct sockaddr *)&server_address, sizeof(server_address));
    listen(server, 20);
    cout << "正在监听" << endl;
    socklen_t client_len = sizeof(sockaddr_in);
    client = accept(server, (struct sockaddr *)&client_address, &client_len);
    cout << "接收端连接成功" << endl;
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
    msg.frame_size = msg.width * msg.height * 3;
    int sended_len = write(client, &msg, sizeof(msg));
    cout << "已发送头部信息长度 " << sended_len << endl;
    if (sended_len != sizeof(msg_header))
    {
        cout << "头部信息发送出错" << endl;
    }
    // 循环发送图像
    while (1)
    {
        capture >> m_mat;
        if (m_mat.empty())
        {
            cout<<"读帧失败"<<endl;
            continue;
        }
        vector<uchar> data_encode;
        cv::imencode(".jpg", m_mat, data_encode);
        msg.frame_size = data_encode.size();
        write(client, &msg, sizeof(msg_header));
        { // 发送vector内的内容
            int sended = 0;
            uchar buffer[2048];
            int buffer_len = 0;
            while (sended < data_encode.size())
            {
                for (; sended < data_encode.size();)
                {
                    buffer[buffer_len++] = data_encode[sended++];
                    if (buffer_len == 2048)
                    {
                        break;
                    }
                }
                write(client, buffer, buffer_len);
                buffer_len = 0;
            }
        }
        // cout << "length=" << msg.frame_size << endl;
        //  等待反馈
        int len = 0;
        int temp = 0;
        while (len != sizeof(msg_header))
        {
            temp = read(client, &msg, sizeof(msg_header) - len);
            if (temp <= 0)
            {
                cout << "反馈错误" << endl;
                return 1;
            }
            len += temp;
        }
    }
    return 0;
}