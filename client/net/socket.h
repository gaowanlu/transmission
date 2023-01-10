#pragma once
#include <string>
#include <memory>
#include <unistd.h>

class Socket
{
public:
    Socket();
    virtual ~Socket();
    virtual int bind(const std::string &ip, int port);
    virtual int listen(int queueSize);
    virtual std::shared_ptr<Socket> accept();
    virtual int connect();
    ssize_t read(int fd, void *buf, size_t count);
    ssize_t write(int fd, const void *buf, size_t count);
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
