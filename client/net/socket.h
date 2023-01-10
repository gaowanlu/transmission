#pragma once
#include <string>
#include <memory>
#include <unistd.h>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

class Socket
{
public:
    Socket();
    virtual ~Socket();
    virtual int bind(const std::string &ip, int port) = 0;
    virtual int listen(int queueSize) = 0;
    virtual std::shared_ptr<Socket> accept() = 0;
    virtual int connect(const std::string &ip, int port) = 0;
    ssize_t read(void *buf, size_t count);
    ssize_t write(const void *buf, size_t count);
    int setFd(const int &fd);
    int getFd();
    void close();

protected:
    int fd;
};

class ClientSocket : public Socket
{
public:
    ClientSocket();
    ~ClientSocket();
    int bind(const std::string &ip, int port) override;
    int listen(int queueSize) override;
    std::shared_ptr<Socket> accept() override;
    int connect(const std::string &ip, int port) override;
};

class ServerSocket : public Socket
{
public:
    ServerSocket();
    ~ServerSocket();
    int bind(const std::string &ip, int port) override;
    int listen(int queueSize) override;
    std::shared_ptr<Socket> accept() override;
    int connect(const std::string &ip, int port) override;
};
