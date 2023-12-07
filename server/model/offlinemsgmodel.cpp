#include "offlinemsgmodel.hpp"
#include <algorithm>
#include <muduo/base/Logging.h>
#include <cstdio>



bool Offlinemsgmodel::insert(const Offlinemsg &msg)
{
    char buff[128]{0};
    sprintf(buff,"insert offlineMessage(fromid,toid,msg_type,accepted,content,datetime) values(%u,%u,%u,%d,'%s',NOW());",msg.fromid_,msg.toid_,msg.msg_type_,(int)msg.accepted_,msg.content_.c_str());
    auto conn = connPoll_.getConn();
    int line;
    if(!conn->insert(buff,&line)) LOG_INFO << conn->getError();
    return line>0;
}

std::vector<Offlinemsg> Offlinemsgmodel::selectOfflinemsgById(unsigned int id)
{
    char buff[128]{0};
    sprintf(buff,"select * from offlineMessage where toid = %u and accepted=0;",id);
    auto conn = connPoll_.getConn();
    int line;
    if(!conn->query(buff,&line)) LOG_INFO << "\n" << conn->getError();
    std::vector<Offlinemsg> msgs;
    for(int i=0;i<line;i++)
    {
        auto ret = conn->next();
        Offlinemsg msg;
        msg.fromid_ = std::stoul(ret[0]);
        msg.toid_ = std::stoul(ret[1]);
        msg.msg_type_ = std::stoul(ret[2]);
        msg.id_ = std::stoul(ret[3]);
        msg.content_ = ret[5];
        msg.datetime_ = ret[6];
        msg.accepted_ = true;
        update(msg);
        msgs.push_back(std::move(msg));
    }
    return msgs;
}
bool Offlinemsgmodel::update(const Offlinemsg &msg)
{
    char buff[128]{0};
    //add code
    sprintf(buff,"update offlineMessage set accepted=%d where id = %u;",(int)msg.accepted_,msg.id_);
    auto conn = connPoll_.getConn();
    int line;
    if(!conn->update(buff,&line)) LOG_INFO << "\n" << conn->getError();
    return line>0;
}

