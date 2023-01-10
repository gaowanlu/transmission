#include "socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>

Socket::Socket()
{
    fd = -1;
}

Socket::~Socket()
{
    if (fd != -1)
    {
        close(fd);
        fd = -1;
    }
}

int Socket::getFd()
{
    return fd;
}

int Socket::setFd(const int &fd)
{
    this->fd = fd;
    return fd;
}

void Socket::close()
{
    if (fd != -1)
    {
        close(fd);
        fd = -1;
    }
}

ssize_t Socket::read(void *buf, size_t count)
{
    if (fd < 0)
    {
        throw new runtime_error("read error fd<0");
    }
    return ::read(fd, buf, count);
}

ssize_t Socket::write(const void *buf, size_t count)
{
    if (fd < 0)
    {
        throw new runtime_error("write error fd<0");
    }
    return ::write(buf, count);
}

ClientSocket::ClientSocket()
{
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client < 0)
    {
        throw new runtime_error("套接字创建失败");
    }
    fd = client;
}

ClientSocket::~ClientSocket()
{
    if (fd != -1)
    {
        close(fd);
        fd = -1;
    }
}

int ClientSocket::bind(const std::string &ip, int port)
{
    return -1;
}

int ClientSocket::listen(int queueSize)
{
    return -1;
}

std::shared_ptr<Socket> ClientSocket::accept()
{
    return nullptr;
}

int ClientSocket::connect(const std::string &ip, int port)
{
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ip.c_str());
    server_address.sin_port = htons(port);
    if (0 != connect(client, (struct sockaddr *)&server_address, sizeof(server_address)))
    {
        throw new runtime_error("connect error");
    }
    return 0;
}

ServerSocket::ServerSocket()
{
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0)
    {
        throw new runtime_error("套接字创建失败");
    }
}

ServerSocket::~ServerSocket()
{
    if (fd != -1)
    {
        close(fd);
        fd = -1;
    }
}

int ServerSocket::bind(const std::string &ip, int port)
{
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ip.c_str());
    server_address.sin_port = htons(port); // 转网络字节序
    return bind(fd, (struct sockaddr *)&server_address, sizeof(server_address));
}

int ServerSocket::listen(int queueSize)
{
    return listen(fd, queueSize);
}

std::shared_ptr<Socket> ServerSocket::accept()
{
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(sockaddr_in);
    int clientFd = accept(server, (struct sockaddr *)&client_address, &client_len);
    std::shared_ptr<Socket> client = std::make_shared<ClientSocket>();
    client->setFd(clientFd);
    return client;
}

int ServerSocket::connect(const std::string &ip, int port)
{
    return -1;
}
