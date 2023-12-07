#pragma once
#include "orm/offlinemsg.hpp"
#include "../../dbinc/poll.hpp"

class Offlinemsgmodel{
public:
     Offlinemsgmodel() : connPoll_(mysql::Poll::getInstance()){}
    ~Offlinemsgmodel() = default;

    bool insert(const Offlinemsg &msg);
    // 判断离线好友添加请求是否发送过
    bool isFriendAdded(const Offlinemsg &msg);
private:
    mysql::Poll &connPoll_;
};

