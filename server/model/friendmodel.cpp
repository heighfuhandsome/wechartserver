#include "friendmodel.hpp"
#include <muduo/base/Logging.h>
bool Friendmodel::isMyfriend(const Friend &my)
{
    char buff[128]{0};
    sprintf(buff,"select count(*) from friend where myid=%u and friendid=%u;",my.myid_,my.friendid_);
    auto conn = connPoll_.getConn();
    int line;
    if(!conn->query(buff,&line)) LOG_INFO << conn->getError();
    return line>0;
}