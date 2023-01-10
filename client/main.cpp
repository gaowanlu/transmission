#include <iostream>
#include <string>
#include <cstring>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs/legacy/constants_c.h>
#include <memory>
#include "msg/msg_header.h"
#include "code/code.h"
#include "net/socket.h"

using namespace std;

int main(int argc, char **argv)
{
    // 创建套接字
    shared_ptr<Socket> client = make_shared<ClientSocket>();
    client->connect("127.0.0.1",8888);
    cout << "连接成功" << endl;
    // 获取协议头
    msg_header msg;
    int len = client->read(&msg, sizeof(msg));
    if (len != sizeof(msg_header))
    {
        cout << "头部信息接收出错" << endl;
        return 1;
    }
    cout << "width=" << msg.width << " height=" << msg.height << " size=" << msg.frame_size << " channel=" << msg.channel << endl;
    Decode *decode = nullptr;
    if (msg.channel == Decode::Channel::ONE)
    {
        decode = new Decode(msg.width, msg.height, Decode::Channel::ONE);
    }
    else
    {
        decode = new Decode(msg.width, msg.height, Decode::Channel::THREE);
    }

    uchar *buffer = decode->getBuffer();
    int rec = 0;
    len = 0;
    cv::namedWindow("client");
    cv::Mat m_mat1 = decode->newMat();
    cv::Mat m_mat2 = decode->newMat();
    int now_mat_flag = 1;
    while (1)
    {
        len = client->read(&msg, sizeof(msg));
        if (len != sizeof(msg_header))
        {
            cout << "头部信息接收出错" << endl;
            return 1;
        }
        // cout<<"width="<<msg.width<<" "<<"height="<<msg.height<<endl;
        rec = 0;
        len = 0;
        while (rec != msg.frame_size)
        {
            len = client->read(buffer, msg.frame_size - rec);
            if (len <= 0)
            {
                return 1;
            }
            rec += len;
        }
        len = client->write(&msg, sizeof(msg_header)); // 反馈
        if (len != sizeof(msg_header))
        {
            cout << "发送反馈错误" << endl;
            return 1;
        }
        // cout<<"rec="<<rec<<" frame_size="<<msg.frame_size<<endl;
        if (now_mat_flag == 1)
        {
            decode->toMat(m_mat1);
            now_mat_flag = 2;
            cv::imshow("client", m_mat2);
        }
        else
        {
            decode->toMat(m_mat2);
            now_mat_flag = 1;
            cv::imshow("client", m_mat1);
        }
        cv::waitKey(80);
    }
    return 0;
}
