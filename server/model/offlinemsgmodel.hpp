#pragma once
#include <vector>
#include "orm/offlinemsg.hpp"
#include "../../dbinc/poll.hpp"

class Offlinemsgmodel{
public:
     Offlinemsgmodel() : connPoll_(mysql::Poll::getInstance()){}
    ~Offlinemsgmodel() = default;
    bool update(const Offlinemsg &msg);
    bool insert(const Offlinemsg &msg);
    std::vector<Offlinemsg> selectOfflinemsgById(unsigned int id);
    
private:
    mysql::Poll &connPoll_;
};

