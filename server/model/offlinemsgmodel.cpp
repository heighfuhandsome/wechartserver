#include "offlinemsgmodel.hpp"
#include <muduo/base/Logging.h>
#include <cstdio>



bool Offlinemsgmodel::insert(const Offlinemsg &msg)
{
    char buff[128]{0};
    sprintf(buff,"insert offlineMessage(fromid,toid,msg_type,accepted,content) values(%u,%u,%u,%d,'%s');",msg.fromid_,msg.toid_,msg.msg_type_,(int)msg.accepted_,msg.content_.c_str());
    auto conn = connPoll_.getConn();
    int line;
    if(!conn->insert(buff,&line)) LOG_INFO << conn->getError();
    return line>0;
}
