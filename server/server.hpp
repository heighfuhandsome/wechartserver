#pragma once
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/base/Timestamp.h>
#include <string>
#include <json/json.h>


using namespace muduo::net;

class Server{
public:
    explicit Server(int port,const std::string &name);
    void start();
    ~Server() = default;
private:
    //当有数据来时 读事件回调
    void onMessage(const TcpConnectionPtr &conn,Buffer *buff,muduo::Timestamp timeStamp);

    //当与客户端链接成功或断开链接时的回调
    void onConnect(const TcpConnectionPtr &conn);
    EventLoop mainLoop_;
    TcpServer server_;
};

