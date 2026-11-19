#include "../include/muduo/net/TcpClient.h"
#include "../include/muduo/net/EventLoopThread.h"
#include "../include/muduo/net/TcpConnection.h"
#include "../include/muduo/base/CountDownLatch.h"
#include <iostream>
#include <functional>
#include <string>

class TranslateClient
{
public:
    TranslateClient(const std::string& sip, int port)
        : _latch(1)
        , _client(_loopthread.startLoop(),muduo::net::InetAddress(sip, port),"TranslateClient")
    {
        auto func1 = std::bind(&TranslateClient::onConnection, this, std::placeholders::_1);
        _client.setConnectionCallback(func1);
        auto func2 = std::bind(&TranslateClient::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        _client.setMessageCallback(func2);
    }
    // 连接服务器成功时进行回复
    void connect()
    {
        _client.connect();
        // 阻塞等待直到连接成功
        _latch.wait();
    }
    bool send(std::string& words)
    {
        // 因为都是异步IO 连接正常的时候进行发送  否则不发送
        if(_conn->connected())
        {
            _conn->send(words);
            return true;
        }
        return false;
    }
private:
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
    // 收到消息时候的回调函数
    void onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp)
    {
        std::cout << "res-> " << buf->retrieveAllAsString() << std::endl;
    }
private:
    muduo::CountDownLatch _latch;
    muduo::net::EventLoopThread _loopthread;
    muduo::net::TcpClient _client;
    muduo::net::TcpConnectionPtr _conn;
};

int main()
{
    TranslateClient client("127.0.0.1",8085);
    client.connect();
    while(1)
    {
        std::string buf;
        std::cin >> buf;
        client.send(buf);
    }
    return 0;
}