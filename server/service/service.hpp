#pragma once

#include <functional>
#include <muduo/net/TcpConnection.h>
#include <muduo/base/Timestamp.h>
#include <json/json.h>
#include <unordered_map>
#include <unordered_map>
#include <mutex>
#include "../model/usermodel.hpp"
#include "../model/offlinemsgmodel.hpp"
#include "../model/friendmodel.hpp"



using namespace muduo::net;

//接口层函数
using serviceFunc = std::function<void(const TcpConnectionPtr &ptr,Json::Value &json,muduo::Timestamp)>;
//设计为单例类
class Service{
public:
    static Service& getInstance();
    //拿到接口处理函数
    serviceFunc getHandler(int id);
    //用户下线
    void removeConn(const TcpConnectionPtr &ptr);
    ~Service() = default;
private:
    Service();
    void reg(const TcpConnectionPtr &ptr,Json::Value &json,muduo::Timestamp);
    void login(const TcpConnectionPtr &ptr,Json::Value &json,muduo::Timestamp);
    void addFriend(const TcpConnectionPtr &ptr,Json::Value &json,muduo::Timestamp);

    //向客户端发送响应
    void sendResponse(const TcpConnectionPtr &ptr,int rescode,const char *content);
    void sendResponse(const TcpConnectionPtr &ptr,int rescode,Json::Value &content);
    
    Usermodel userModel_;
    Friendmodel friendmodel_;
    Offlinemsgmodel offlinemsgmodel_;

    std::mutex mutex_;
    std::unordered_map<int,TcpConnectionPtr> conns_;
    std::unordered_map<int,serviceFunc> serviceMapping_;


};
