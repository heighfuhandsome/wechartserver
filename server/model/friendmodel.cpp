#include "friendmodel.hpp"
#include <cstdio>
#include <muduo/base/Logging.h>
bool Friendmodel::isMyfriend(const Friend &my)
{
    char buff[128]{0};
    sprintf(buff,"select count(*) from friend where myid=%u and friendid=%u;",my.myid_,my.friendid_);
    LOG_INFO << buff;
    auto conn = connPoll_.getConn();
    int line;
    if(!conn->query(buff,&line)) LOG_INFO << conn->getError();
    return std::stoi(conn->next()[0]) > 0;
}

bool Friendmodel::insert(const Friend &obj)
{
    char buff[128]{0};
    sprintf(buff,"insert friend (myid,friendid,remarks) values(%u,%u,'%s');",obj.myid_,obj.friendid_,obj.remarks_.c_str());
    int line;
    auto conn = connPoll_.getConn();
    if(!conn->insert(buff,&line)) LOG_INFO <<"\n" <<conn->getError();
    return line>0;
}
