//
// Created by EDZ on 2023/10/10.
//

#include "Webserver.h"

int startServer(const WebServer *);

WebServer::WebServer() = default;

WebServer::~WebServer() = default;

void WebServer::__construct(Php::Parameters &param){
    address = param[0];
    port = param[1];
}

std::string WebServer::getAddress() const{
    return address.stringValue();
}

int WebServer::getPort() const{
    return port.numericValue();
}

Php::Value WebServer::getCallback(std::string name) const{
    Php::Value value;
    auto it = callbacks.find("request");
    if (it != callbacks.end()) {
        value = it->second;
    } else {
        std::cout << "Key 'request' not found in callbacks." << std::endl;
    }
    return value;
}


void WebServer::on(Php::Parameters &param){
    std::string event = param[0].stringValue();
    Php::Value callback = param[1];
    callbacks.insert(std::pair<std::string,Php::Value>(event,callback));
}

Php::Value WebServer::start() {
    Php::Value res = startServer(this);
    return res;
}