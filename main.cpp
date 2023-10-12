#include "WebServer.h"

#define MAX_EVENTS 10

struct handleStruct {
    int clientSocket;
    Php::Value callback;
};


void *handleRequest(void *arg) {
    handleStruct *param = (handleStruct *) arg;
    int clientSocket = param->clientSocket;
    Php::Value callback = param->callback;
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    std::string response = "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "\r\n";
    Php::Value res = callback();
    response += res.stringValue();
    send(clientSocket, response.c_str(), response.size(), 0);
    std::cout << "send message~~" << std::endl;
    close(clientSocket);
    pthread_exit(NULL);
}

void *handleRequestForCpp(int clientSocket,Php::Value callback) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    std::string response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html\r\n"
                           "\r\n";
    Php::Value res = callback();
    response += res.stringValue();
    send(clientSocket, response.c_str(), response.size(), 0);
    std::cout << "send message~~" << std::endl;
    close(clientSocket);
    pthread_exit(NULL);
}

int startServer(const WebServer& server) {
    int serverSocket;
    struct sockaddr_in serverAddress;
    int epoll_fd, nfds;
    struct epoll_event ev, events[MAX_EVENTS];
    char buffer[1024];
    std::vector<int> connectedSockets;

    // 创建 TCP 套接字
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Error: Failed to create socket" << std::endl;
        return -1;
    }

    // 设置端口复用
    int enable = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        std::cerr << "Error: Failed to set socket options" << std::endl;
        return -1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(server.getPort()); // 指定服务器监听的端口
    inet_pton(AF_INET, server.getAddress().c_str(), &(serverAddress.sin_addr));
//    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // 将套接字绑定到指定地址和端口
    if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Error: Failed to bind" << std::endl;
        return -1;
    }


    // 监听传入的连接请求
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Error: Failed to listen" << std::endl;
        return -1;
    }

    std::cout << "Server listening on port " << server.getPort() << std::endl;
    if (fork() == -1) {
        std::cerr << "Error: Failed to fork" << std::endl;
        return -1;
    }
    // 创建 epoll 实例
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        std::cerr << "Error: Failed to create epoll instance" << std::endl;
        return -1;
    }

    // 监听 serverSocket 上的事件
    ev.events = EPOLLIN;
    ev.data.fd = serverSocket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serverSocket, &ev) == -1) {
        std::cerr << "Error: Failed to add serverSocket to epoll" << std::endl;
        return -1;
    }

    while (true) {
        // 查看已触发的事件
        nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            std::cerr << "Error: epoll_wait failed" << std::endl;
            return -1;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == serverSocket) {
                struct sockaddr_in clientAddress;
                socklen_t clientLength;
                int newSocket;

                // 接受传入的连接请求
                clientLength = sizeof(clientAddress);
                newSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientLength);
                if (newSocket < 0) {
                    std::cerr << "Error: Failed to accept" << std::endl;
                    return -1;
                }

                // 将新的套接字添加到 epoll 实例中
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = newSocket;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, newSocket, &ev) == -1) {
                    std::cerr << "Error: Failed to add newSocket to epoll" << std::endl;
                    return -1;
                }

                connectedSockets.push_back(newSocket); // 将新的套接字添加到已连接套接字的列表中
                std::cout << "New client connected" << std::endl;
            } else {
                int clientSocket = events[i].data.fd;
                ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (bytesRead <= 0) {
                    // 客户端关闭连接或出现错误，从 epoll 实例中移除套接字并关闭连接
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, clientSocket, nullptr);
                    close(clientSocket);
                    std::cout << "Client disconnected" << std::endl;
                } else {
                    std::cout << "New message!!" << std::endl;
                    // 创建线程来处理客户端请求
                    //1.C语言pthread库方式
//                    pthread_t thread;
//                    handleStruct *arg = new handleStruct;
//                    arg->clientSocket = clientSocket;
//                    arg->callback = server.getCallback("request");
//                    if (pthread_create(&thread, NULL, handleRequest, (void *) arg) != 0) {
//                        std::cerr << "Error: Failed to create thread" << std::endl;
//                        delete arg;
//                        continue;
//                    }
//                    // 分离线程，使其在完成处理后能自动释放资源
//                    pthread_detach(thread);
                    //2.c++线程lambda方式
//                    Php::Value callback = server.getCallback("request");
//                    std::thread handleThread([clientSocket, &callback] {
//                        char buffer[1024];
//                        memset(buffer, 0, sizeof(buffer));
//                        std::string response = "HTTP/1.1 200 OK\r\n"
//                                               "Content-Type: text/html\r\n"
//                                               "\r\n";
//                        Php::Value res = callback();
//                        response += res.stringValue();
//                        send(clientSocket, response.c_str(), response.size(), 0);
//                        std::cout << "send message~~" << std::endl;
//                        close(clientSocket);
//                    });
//                    handleThread.detach();
                    //3.c++和std::bind的方式
                    Php::Value callback = server.getCallback("request");
                    auto handleRequest =  std::bind(handleRequestForCpp,clientSocket,callback);
                    std::thread handleThread(handleRequest);
                    handleThread.detach();

                }
            }
        }
    }

    // 关闭服务器套接字和 epoll 实例
    close(serverSocket);
    close(epoll_fd);

    return 0;
}
/**
 *  告诉编译器get_module是个纯C函数
 */
extern "C" {

/**
 *  本函数在PHP进程一打开就会被访问，并返回一个描述扩展信息的PHP结构指针
 */
PHPCPP_EXPORT void *get_module() {
    // 必须是static类型，因为扩展对象需要在PHP进程内常驻内存
    static Php::Extension extension("web_server", "1.0.0");
    //初始化导出类
    Php::Class<WebServer> webServer("WebServer");
    //注册类的构造函数
    webServer.method<&WebServer::__construct>("__construct", Php::Public, {
            Php::ByVal("address", Php::Type::String, true),
            Php::ByVal("port", Php::Type::Numeric, true),
    });

    webServer.method<&WebServer::set>("set", Php::Public, {
            Php::ByVal("setting", Php::Type::Array, true),
    });
    //注册导出类的可访问普通函数
    webServer.method<&WebServer::start>("start", Php::Public, {
            Php::ByVal("callback", Php::Type::Callable, true)
    });
    //注册导出类的可访问普通函数
    webServer.method<&WebServer::on>("on", Php::Public, {
            Php::ByVal("event", Php::Type::String, true),
            Php::ByVal("callback", Php::Type::Callable, true)
    });
    //这里可以添加你要暴露给PHP调用的函数
    extension.add(std::move(webServer));

    // 返回扩展对象指针
    return extension;
}
}
