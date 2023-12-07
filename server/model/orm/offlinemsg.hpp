#pragma once
#include <string>
class Offlinemsg
{
public:
    unsigned int fromid_;
    unsigned int toid_;
    unsigned int msg_type_;
    std::string content_;
    bool accepted_;
    unsigned int id_;
};