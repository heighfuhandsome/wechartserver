#pragma once

#include "../model/friendmodel.hpp"
#include "../model/offlinemsgmodel.hpp"
#include "../model/usermodel.hpp"
#include <functional>
#include <json/json.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/TcpConnection.h>
#include <mutex>
#include <unordered_map>

using namespace muduo::net;

// 接口层函数
using serviceFunc = std::function<void(const TcpConnectionPtr &ptr,
                                       Json::Value &json, muduo::Timestamp)>;
// 设计为单例类
class Service {
public:
  static Service &getInstance();
  // 拿到接口处理函数
  serviceFunc getHandler(int id);
  // 用户下线
  void removeConn(const TcpConnectionPtr &ptr);
  ~Service() = default;

private:
  Service();
  void reg(const TcpConnectionPtr &ptr, Json::Value &json, muduo::Timestamp);
  void login(const TcpConnectionPtr &ptr, Json::Value &json, muduo::Timestamp);
  void addFriend(const TcpConnectionPtr &ptr, Json::Value &json,
                 muduo::Timestamp);
  void getOfflienMsg(const TcpConnectionPtr &ptr, Json::Value &json,
                     muduo::Timestamp);
  void acceptFriend(const TcpConnectionPtr &ptr, Json::Value &json,
                    muduo::Timestamp);
  void getFriendList(const TcpConnectionPtr &ptr, Json::Value &json,
                     muduo::Timestamp);
  void sendMsgToFriend(const TcpConnectionPtr &ptr, Json::Value &json,
                       muduo::Timestamp);

  // 向客户端发送响应
  void sendResponse(const TcpConnectionPtr &ptr, int rescode,
                    const char *content);
  void sendResponse(const TcpConnectionPtr &ptr, int rescode,
                    Json::Value &content);
  // 判断用户是否登录
  bool userIsOnline(unsigned int id, const TcpConnectionPtr &ptr);
  // 获取当前时间
  std::string getDatetime();

  Usermodel userModel_;
  Friendmodel friendmodel_;
  Offlinemsgmodel offlinemsgmodel_;

  std::mutex mutex_;
  std::unordered_map<int, TcpConnectionPtr> conns_;
  std::unordered_map<int, serviceFunc> serviceMapping_;
};
