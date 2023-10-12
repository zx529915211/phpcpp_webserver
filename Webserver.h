//
// Created by EDZ on 2023/10/10.
//

#ifndef PHPCPP_WEBSERVER_WEBSERVER_H
#define PHPCPP_WEBSERVER_WEBSERVER_H
#include <phpcpp.h>
#include <stdlib.h>
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
#include <thread>
#include <sys/stat.h>
#include <fcntl.h>
class WebServer : public Php::Base{
private:
    //地址 点分十进制字符串
    std::string address;
    //端口
    int port;
    //回调事件
    std::map<std::string,Php::Value> callbacks;
    //配置信息 是否守护进程 创建子进程数量....
    std::map<std::string,Php::Value> setting;

    void daemon() const;

    Php::Value mapFindValue(std::map<std::string,Php::Value> t_map,std::string str) const;
public:
    WebServer();

    virtual ~WebServer();

    void __construct(Php::Parameters &param);

    std::string getAddress() const;

    int getPort() const;

    Php::Value getCallback(std::string) const;

    void set(Php::Parameters &param);


    void on(Php::Parameters &param);

    Php::Value start() const;

};



#endif //PHPCPP_WEBSERVER_WEBSERVER_H
