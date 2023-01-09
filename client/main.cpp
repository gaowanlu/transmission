#include <iostream>
#include <sys/types.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <opencv2/opencv.hpp>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <opencv2/imgcodecs/legacy/constants_c.h>
#include "msg_header.h"
#include "code.h"

using namespace std;

int main(int argc, char **argv)
{
    // 创建套接字
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client < 0)
    {
        cout << "创建套接字失败" << endl;
        return 1;
    }
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(8888);
    if (0 != connect(client, (struct sockaddr *)&server_address, sizeof(server_address)))
    {
        cout << "连接失败" << endl;
        return 1;
    }
    cout << "连接成功" << endl;
    // 获取协议头
    msg_header msg;
    int len = read(client, &msg, sizeof(msg));
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
    cv::namedWindow("show");
    cv::Mat m_mat1 = decode->newMat();
    cv::Mat m_mat2 = decode->newMat();
    int now_mat_flag = 1;
    while (1)
    {
        len = read(client, &msg, sizeof(msg));
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
            len = read(client, buffer, msg.frame_size - rec);
            if (len <= 0)
            {
                return 1;
            }
            rec += len;
        }
        len = write(client, &msg, sizeof(msg_header)); // 反馈
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
            cv::imshow("show", m_mat2);
        }
        else
        {
            decode->toMat(m_mat2);
            now_mat_flag = 1;
            cv::imshow("show", m_mat1);
        }
        cv::waitKey(80);
    }
    return 0;
}
