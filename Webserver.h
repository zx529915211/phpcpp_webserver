//
// Created by EDZ on 2023/10/10.
//

#ifndef PHPCPP_WEBSERVER_WEBSERVER_H
#define PHPCPP_WEBSERVER_WEBSERVER_H
#include <phpcpp.h>
#include <time.h>
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <vector>
#include <arpa/inet.h>
#include <sys/types.h>
#include <pthread.h>
#include <map>
class WebServer : public Php::Base{
private:
    //地址
    Php::Value address;
    //端口
    Php::Value port;
    //回调事件
    std::map<std::string,Php::Value> callbacks;
public:
    WebServer();

    virtual ~WebServer();

    void __construct(Php::Parameters &param);

    std::string getAddress() const;

    int getPort() const;

    Php::Value getCallback(std::string) const;


    void on(Php::Parameters &param);

    Php::Value start();
};



#endif //PHPCPP_WEBSERVER_WEBSERVER_H
