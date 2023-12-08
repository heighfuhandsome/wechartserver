#pragma once

#include "orm/friend.hpp"
#include "../../dbinc/poll.hpp"
#include <vector>


class Friendmodel{
public:
     Friendmodel() : connPoll_(mysql::Poll::getInstance()){}
    ~Friendmodel() = default;
    //判断是否已经是自己的好友
    bool isMyfriend(const Friend &my);
    bool insert(const Friend &obj);
    std::vector<Friend> selectFriendsById(unsigned int id);
private:
    mysql::Poll &connPoll_;

};

