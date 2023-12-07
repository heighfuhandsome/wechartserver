#include "server.hpp"
#include "service/service.hpp"
#include "public.hpp"

#include <functional>
#include <json/json.h>
#include <thread>
#include <muduo/base/Logging.h>


thread_local Json::Reader jsreader;

InetAddress createListenAddr(int port)
{
    return InetAddress(port);
}

Server::Server(int port, const std::string &name) : server_(&mainLoop_, createListenAddr(port), name)
{
    using namespace std::placeholders;
    server_.setMessageCallback(std::bind(&Server::onMessage, this, _1, _2, _3));
    server_.setConnectionCallback(std::bind(&Server::onConnect, this, _1));
    // 设置ioloop 的个数为当前主机核心数-1
    server_.setThreadNum(std::thread::hardware_concurrency() - 1);
}

void Server::start()
{
    server_.start();
    mainLoop_.loop();
}

void Server::onMessage(const TcpConnectionPtr &conn, Buffer *buff, muduo::Timestamp timeStamp)
{
    std::string msg = buff->retrieveAllAsString();
    Json::Value json;
    jsreader.parse(msg, json, false);
    if(json["REQ_CODE"].isNull() || !json["REQ_CODE"].isInt()) return;
    Service &service = Service::getInstance();
    service.getHandler(json["REQ_CODE"].asInt())(conn, json, timeStamp);
}

void Server::onConnect(const TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        Service::getInstance().removeConn(conn);
    }
}
