#pragma once

#include "orm/user.hpp"
#include "poll.hpp"


class Usermodel{
public:
    Usermodel() : connPoll_(mysql::Poll::getInstance()){}
    bool insert(const User &user);
    bool selectUserByAccount(User &,const std::string &); //通过账号查询user
    bool update(const User &user);
    bool selectUserById(User &user,unsigned int id);
private:
    mysql::Poll &connPoll_;

    //
    void fillUser(User &user,const std::vector<const char*> &result);
};

