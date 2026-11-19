#include "../include/muduo/proto/codec.h"
#include "../include/muduo/proto/dispatcher.h"
#include "../include/muduo/base/Logging.h"
#include "../include/muduo/base/Mutex.h"
#include "../include/muduo/net/EventLoop.h"
#include "../include/muduo/net/TcpServer.h"

#include "request.pb.h"

#include <iostream>
#include <functional>
#include <unordered_map>

class Server
{
public:
    using MessagePtr = std::shared_ptr<google::protobuf::Message>;
    using TranslateRequestptr = std::shared_ptr<request::TranslateRequest>;
    using TranslateResponseptr = std::shared_ptr<request::TranslateResponse>;
    using AddRequestptr = std::shared_ptr<request::AddRequest>;
    using AddResponseptr = std::shared_ptr<request::AddResponse>;

    Server(int port)
        : _server(&_baseloop,muduo::net::InetAddress("0.0.0.0", port),"Server",muduo::net::TcpServer::kReusePort)
        , _dispatcher(std::bind(&Server::onUnknownMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
        , _codec(std::bind(&ProtobufDispatcher::onProtobufMessage, &_dispatcher, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
    {
        // 注册请求处理函数
        _dispatcher.registerMessageCallback<request::TranslateRequest>(std::bind(&Server::onTranslateMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        _dispatcher.registerMessageCallback<request::AddRequest>(std::bind(&Server::onAddMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        _server.setMessageCallback(std::bind(&ProtobufCodec::onMessage, &_codec, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        _server.setConnectionCallback(std::bind(&Server::onconnection, this, std::placeholders::_1));
    }
    void start()
    {
        _server.start();
        _baseloop.loop();
    }
private:
    void onconnection(const muduo::net::TcpConnectionPtr& conn)
    {
        if(conn->connected())
        {
            LOG_INFO << "connect success";
        }
        else
        {
            LOG_INFO << "connect close";
        }
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
    void onTranslateMessage(const muduo::net::TcpConnectionPtr& conn, const TranslateRequestptr& message, muduo::Timestamp)
    {
        // 提取有效消息
        std::string msg = message->msg();
        // 翻译得到结果
        std::string rsp_msg = TranslateWords(msg);
        // 组织protobuf相应
        request::TranslateResponse resp;
        resp.set_msg(rsp_msg);
        // 发送响应
        _codec.send(conn, resp);
    }
    void onAddMessage(const muduo::net::TcpConnectionPtr& conn, const AddRequestptr& message, muduo::Timestamp)
    {
        // 提取有效消息
        int num1 = message->num1();
        int num2 = message->num2();
        // 相加得到结果
        int res = num1 + num2;
        // 组织protobuf相应
        request::AddResponse resp;
        resp.set_result(res);
        // 发送响应
        _codec.send(conn, resp);
    }

    void onUnknownMessage(const muduo::net::TcpConnectionPtr& conn, const MessagePtr& message, muduo::Timestamp)
    {
        LOG_INFO << "onUnknownMessage: " << message->GetTypeName();
        conn->shutdown();
    }
private:
    muduo::net::EventLoop _baseloop;
    muduo::net::TcpServer _server; // 服务器对象
    ProtobufDispatcher _dispatcher;// 请求分发器对象
    ProtobufCodec _codec;          // protobuf协议处理器-针对收到的请求对请求数据进行protobuf协议处理
};

int main()
{
    Server server(8080);
    server.start();
    return 0;
}