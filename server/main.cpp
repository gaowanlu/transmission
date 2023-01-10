#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <vector>
#include <memory>
#include "msg/msg_header.h"
#include "code/code.h"
#include "utils/utils.h"
#include "net/socket.h"

#define COLOR 1

using namespace std;

int main(int argc, char **argv)
{
    Utils::newWindow("server");
    // 创建监听套接字
    shared_ptr<Socket> server=make_shared<ServerSocket>();
    server->bind("0.0.0.0",8888);
    server->listen(20);
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
    Encode encode(msg.width, msg.height, Encode::THREE);
    msg.channel = Encode::Channel::THREE;
#else
    Encode encode(msg.width, msg.height, Encode::ONE);
    cvtColor(m_mat, m_mat, cv::COLOR_BGR2GRAY);
    msg.channel = Encode::Channel::ONE;
#endif
    msg.frame_size = encode.size();
    shared_ptr<Socket> client=nullptr;
    // 循环发送图像
    while (1)
    {
        // 客户端连接
        if (client == nullptr)
        {
            client = server->accept();
            // 发送协议头
            if (sizeof(msg_header) != client->write(&msg, sizeof(msg)))
            {
                cout << "头部信息发送出错" << endl;
                continue;
            }
        }
        // 读帧与编码
        capture >> m_mat;
        if (m_mat.empty())
        {
            cout << "读帧失败" << endl;
            continue;
        }
#if COLOR
#else
        cvtColor(m_mat, m_mat, cv::COLOR_BGR2GRAY);
#endif
        encode.matTo(m_mat);
        cv::imshow("server", m_mat);
        cv::waitKey(1);
        // 发送头部
        ssize_t count = client->write(&msg, sizeof(msg_header));
        if (count != sizeof(msg_header))
        {
            client->close();
            client=nullptr;
            continue;
        }
        { // 发送编码内容
            int buffer_len = 0, sended = 0;
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
                ssize_t count = client->write(buffer, buffer_len);
                if (!count)
                {
                    client->close();
                    client = nullptr;
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
            temp = client->read(&msg, sizeof(msg_header) - len);
            if (temp <= 0)
            {
                client->close();
                client = nullptr;
                break;
            }
            len += temp;
        }
    }
    return 0;
}
