#include "usermodel.hpp"
#include "poll.hpp"
#include <sstream>
#include <muduo/base/Logging.h>
#include <algorithm>
#include <string>

thread_local std::stringstream ss;
bool Usermodel::insert(const User &user)
{
    auto conn = connPoll_.getConn();
    ss.clear();
    ss << "insert user (account,password,online) values('" << user.getAccount() << "',"
       << "'" << user.getPassword() << "',0);";
    int line;
    return conn->insert(ss.str(), &line);
}

bool Usermodel::selectUserByAccount(User &user, const std::string &account)
{
    char buff[128]{0};
    sprintf(buff, "select * from user where account = %s;", account.c_str());
    int line;
    auto conn = connPoll_.getConn();
    bool exists = conn->query(buff, &line);
    if (line == 0)
        return false;
    auto result = conn->next();
    fillUser(user,result);
    return true;
}

bool Usermodel::update(const User &user)
{
    char buff[128]{0};
    sprintf(buff, "update set password='%s',nickname='%s',online=%d;", user.getPassword().c_str(), user.getNickname().c_str(), user.isOnline());
    auto conn = connPoll_.getConn();
    int line;
    conn->update(buff, &line);
    return line > 0 ? true : false;
}

bool Usermodel::selectUserById(User &user, unsigned int id)
{
    char buff[128]{0};
    sprintf(buff, "select * from user where id=%u",id);
    int line;
    auto conn = connPoll_.getConn();
    conn->query(buff,&line);
    if(line == 0) return false;
    fillUser(user,conn->next());
    return true;
}

void Usermodel::fillUser(User &user, const std::vector<const char *> &result)
{
    if (result[0])
        user.setId(std::stoul(result[0]));
    if (result[1])
        user.setAccount(result[1]);
    if (result[2])
        user.setPassword(result[2]);
    if (result[3])
        user.setNickname(result[3]);
    if (result[4])
    {
        int i = std::stoi(result[4]);
        i ? user.setOnline(true) : user.setOnline(false);
    }
}
