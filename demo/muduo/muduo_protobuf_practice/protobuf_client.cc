#include "../include/muduo/proto/codec.h"
#include "../include/muduo/proto/dispatcher.h"

#include "../include/muduo/base/Logging.h"
#include "../include/muduo/base/Mutex.h"
#include "../include/muduo/net/EventLoopThread.h"
#include "../include/muduo/net/TcpClient.h"
#include "../include/muduo/base/CountDownLatch.h"

#include "request.pb.h"
#include <iostream>
#include <string>
#include <functional>
#include <unistd.h>

class Client
{
public:
    using MessagePtr = std::shared_ptr<google::protobuf::Message>;
    using TranslateResponseptr = std::shared_ptr<request::TranslateResponse>;
    using AddResponseptr = std::shared_ptr<request::AddResponse>;
    Client(const std::string& sip, int port)
        : _latch(1)
        , _client(_loopthread.startLoop(), muduo::net::InetAddress(sip, port), "Client")
        , _dispatcher(std::bind(&Client::onUnknownMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
        , _codec(std::bind(&ProtobufDispatcher::onProtobufMessage, &_dispatcher, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
    {
        // 注册请求处理函数
        _dispatcher.registerMessageCallback<request::TranslateResponse>(std::bind(&Client::onTranslateMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        _dispatcher.registerMessageCallback<request::AddResponse>(std::bind(&Client::onAddMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        _client.setMessageCallback(std::bind(&ProtobufCodec::onMessage, &_codec, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        _client.setConnectionCallback(std::bind(&Client::onConnection, this, std::placeholders::_1));

    }
    void connect()
    {
        _client.connect();
        _latch.wait();
    }
    void Translate(const std::string& msg)
    {
        request::TranslateRequest req;
        req.set_msg(msg);
        send(&req);
    }
    void Add(int num1, int num2)
    {
        request::AddRequest req;
        req.set_num1(num1);
        req.set_num2(num2);
        send(&req);
    }
private:
    bool send(const google::protobuf::Message* msg)
    {
        if(_conn->connected())
        {
            _codec.send(_conn, *msg);
            return true;
        }
        return false;
    }
    // 连接建立成功和关闭的时候的回调函数
    // 建立成功后  唤醒上面的阻塞
    void onConnection(const muduo::net::TcpConnectionPtr& conn)
    {
        if(conn->connected())
        {
            _latch.countDown();// 唤醒阻塞
            _conn = conn;
        }
        else
        {
            // 连接关闭
            _conn.reset();
        }
    }
    void onTranslateMessage(const muduo::net::TcpConnectionPtr& conn, const TranslateResponseptr& message, muduo::Timestamp)
    {
        std::cout << "translate result " << message->msg() << std::endl;
    }
    void onAddMessage(const muduo::net::TcpConnectionPtr& conn, const AddResponseptr& message, muduo::Timestamp)
    {
        std::cout << "add result " << message->result() << std::endl;
    }
    void onUnknownMessage(const muduo::net::TcpConnectionPtr& conn, const MessagePtr& message, muduo::Timestamp)
    {
        LOG_INFO << "onUnknownMessage: " << message->GetTypeName();
        conn->shutdown();
    }
private:
    muduo::CountDownLatch _latch;            // 实现同步
    muduo::net::EventLoopThread _loopthread; // 异步IO处理循环处理线程
    muduo::net::TcpConnectionPtr _conn;      // 客户端对应的连接
    muduo::net::TcpClient _client;           // 客户端
    ProtobufDispatcher _dispatcher;          // 请求分发器对象
    ProtobufCodec _codec;                    // protobuf协议处理器-针对收到的请求对请求数据进行protobuf协议处理
};

int main()
{
    Client client("127.0.0.1", 8080);
    client.connect();
    client.Translate("database");
    client.Add(2,4);
    sleep(1);
    return 0;
}