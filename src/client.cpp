// g++ `pkg-config opencv4 --cflags` client.cpp -o client `pkg-config opencv4 --libs`
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
    server_address.sin_port = htons(2023);
    if (0 != connect(client, (struct sockaddr *)&server_address, sizeof(server_address)))
    {
        cout << "连接失败" << endl;
        return 1;
    }
    cout << "连接成功" << endl;
    // read 读取控制信息
    msg_header msg;
    int len = read(client, &msg, sizeof(msg));
    if (len != sizeof(msg_header))
    {
        cout << "头部信息接收出错" << endl;
        return 1;
    }
    cout << msg.width << " " << msg.height << " " << msg.frame_size << endl;
    uchar *buffer = new uchar[msg.frame_size];
    int rec = 0;
    len = 0;
    cv::namedWindow("show");
    cv::Mat m_mat;
    while (1)
    {
        len = read(client, &msg, sizeof(msg));
        if (len != sizeof(msg_header))
        {
            cout << "头部信息接收出错" << endl;
            return 1;
        }
        cout<<"width="<<msg.width<<" "<<"height="<<msg.height<<endl;
        while (rec != msg.frame_size)
        {
            len = read(client, buffer, msg.frame_size - rec);
            if (len <= 0)
            {
                return 1;
            }
            rec += len;
        }
        len=write(client,&msg,sizeof(msg_header));//反馈
        if(len!=sizeof(msg_header)){
            cout<<"发送反馈错误"<<endl;
            return 1;
        }
        cout<<"rec="<<rec<<" frame_size="<<msg.frame_size<<endl;
        rec = 0;
        len = 0;
        cv::_InputArray pic_arr(buffer,msg.frame_size);
        m_mat=cv::imdecode(pic_arr,CV_LOAD_IMAGE_COLOR);
        cv::imshow("show", m_mat);
        cv::waitKey(200);
    }
    return 0;
}