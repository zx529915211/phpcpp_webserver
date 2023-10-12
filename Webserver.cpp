//
// Created by EDZ on 2023/10/10.
//

#include "Webserver.h"

int startServer(const WebServer &server);

WebServer::WebServer() = default;

WebServer::~WebServer() = default;

void WebServer::__construct(Php::Parameters &param) {
    address = param[0].stringValue();
    port = param[1].numericValue();
    setting["work_num"] = 1;
    setting["daemon"] = 0;
}

void WebServer::set(Php::Parameters &param) {
    Php::Array setting = param[0];
    if (setting["work_num"]) {
        this->setting["work_num"] = setting["work_num"];
    }
    if (setting["daemon"]) {
        this->setting["daemon"] = setting["daemon"];
    }
    std::cout << this->setting["work_num"] << std::endl;
}

std::string WebServer::getAddress() const {
    return address;
}

int WebServer::getPort() const {
    return port;
}

Php::Value WebServer::getCallback(std::string name) const {
//    Php::Value value;
//    auto it = callbacks.find(name);
//    if (it != callbacks.end()) {
//        value = it->second;
//    } else {
//        std::cout << "Key 'request' not found in callbacks." << std::endl;
//    }
    return this->mapFindValue(callbacks, name);
}

Php::Value WebServer::mapFindValue(std::map<std::string,Php::Value> t_map,std::string str) const{
    Php::Value value;
    auto it = t_map.find(str);
    if (it != t_map.end()) {
        value = it->second;
    } else {
        std::cout << "Key 'request' not found in callbacks." << std::endl;
    }
    return value;
}


void WebServer::on(Php::Parameters &param) {
    std::string event = param[0].stringValue();
    Php::Value callback = param[1];
//    callbacks.insert(std::pair<std::string,Php::Value>(event,callback));
//    callbacks.insert(std::make_pair(event,callback));
    callbacks[event] = callback;
}

Php::Value WebServer::start() const {
    if (this->mapFindValue(setting,"daemon").numericValue() == 1) {
        this->daemon();
    }
    Php::Value res = startServer((*this));
    return res;
}

void WebServer::daemon() const {
    // 创建子进程
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    // 使父进程退出
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    // 在子进程中创建新会话
    if (setsid() < 0) {
        perror("setsid failed");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    // 改变当前工作目录到根目录
//    if (chdir("/") < 0) {
//        perror("chdir failed");
//        exit(EXIT_FAILURE);
//    }

    // 关闭不再需要的文件描述符
    int fd = open("/dev/null", O_RDWR);    // 打开空设备文件
    if (fd < 0) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }

    dup2(fd, STDIN_FILENO);    // 标准输入重定向到空设备文件
    dup2(fd, STDOUT_FILENO);   // 标准输出重定向到空设备文件
    dup2(fd, STDERR_FILENO);   // 标准错误重定向到空设备文件

    if (fd > STDERR_FILENO) {
        close(fd);
    }
}