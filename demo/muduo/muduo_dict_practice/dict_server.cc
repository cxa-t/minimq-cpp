#include "./include/muduo/net/TcpServer.h"
#include "./include/muduo/net/EventLoop.h"
#include "./include/muduo/net/TcpConnection.h"
#include <iostream>
#include <functional>
#include <unordered_map>
#include <string>

class TranslateServer
{
public:
    TranslateServer(int port)
        : _server(&_baseloop,
            muduo::net::InetAddress("0.0.0.0", port), 
            "TranslateServer",
            muduo::net::TcpServer::kReusePort)
    {
        // 将类成员函数设置为服务器回调处理函数
        // std::bind是一个函数适配器函数，对指定的函数进行参数绑定
        auto func1 = std::bind(&TranslateServer::onConnection, this, std::placeholders::_1);
        _server.setConnectionCallback(func1);
        auto func2 = std::bind(&TranslateServer::onMessage, this, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
        _server.setMessageCallback(func2);
    }
    // 启动服务器
    void start() 
    {
        _server.start();// 开始事件监听
        _baseloop.loop();// 开始事件监控，是一个死循环阻塞接口
    }
private:
    // onConnection应该是在一个连接建立成功和关闭的时候被调用
    void onConnection(const muduo::net::TcpConnectionPtr& coon)
    {
        // 新连接建立成功时候的回调函数
        if(coon->connected() == true)
            std::cout << "新连接建立成功" << std::endl;
        else
            std::cout << "新连接关闭\n";
    }
    std::string TranslateWords(const std::string words)
    {
        std::unordered_map<std::string, std::string> dict_map =
        {
            {"computer", "计算机"},
            {"program", "程序"},
            {"function", "函数"},
            {"variable", "变量"},
            {"pointer", "指针"},
            {"memory", "内存"},
            {"thread", "线程"},
            {"process", "进程"},
            {"network", "网络"},
            {"server", "服务器"},
            {"client", "客户端"},
            {"database", "数据库"},
            {"message", "消息"},
            {"queue", "队列"},
            {"protocol", "协议"}
        };
        auto it = dict_map.find(words);
        if(it == dict_map.end())
            return "did not get it";
        return it->second;
    }
    void onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp)
    {
        // 收到请求时候的回调函数
        // 1. 从buf里面取出请求数据
        std::string words_request = buf->retrieveAllAsString();
        // 2. 调用回调函数进行处理
        std::string response = TranslateWords(words_request);
        // 3. 对client进行响应
        conn->send(response);
    }
private:
    muduo::net::EventLoop _baseloop;  // epoll的时间监控，触发事件进行IO
    muduo::net::TcpServer _server;    // 设置服务器回调函数，告诉服务器什么请求怎么处理
};

int main()
{
    TranslateServer server(8085);
    server.start();

    return 0;
}