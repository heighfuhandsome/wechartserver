#pragma once
#include "orm/offlinemsg.hpp"
#include "poll.hpp"

class Offlinemsgmodel{
public:
     Offlinemsgmodel() : connPoll_(mysql::Poll::getInstance()){}
    ~Offlinemsgmodel() = default;

    bool insert(const Offlinemsg &msg);
private:
    mysql::Poll &connPoll_;
};

