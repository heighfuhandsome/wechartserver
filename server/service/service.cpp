#include "service.hpp"
#include "../public.hpp"

#include <functional>
#include <json/value.h>
#include <json/writer.h>
#include <muduo/base/Logging.h>
#include <json/json.h>
#include <muduo/net/Callbacks.h>
#include <vector>

thread_local Json::FastWriter jswriter;
Service &Service::getInstance()
{
    static Service service;
    return service;
}

serviceFunc Service::getHandler(int id)
{
    auto it = serviceMapping_.find(id);
    if (it == serviceMapping_.end())
    {
        return [=](const TcpConnectionPtr &ptr, Json::Value &json, muduo::Timestamp)
        {
            LOG_ERROR << "对应接口不存在，req_id ：" << id;
        };
    }
    return serviceMapping_[id];
}

Service::Service()
{
    // 填充 serviceMapping_
    using namespace std::placeholders;
    serviceMapping_.insert({REQ_CODE::REG_CODE_REG, std::bind(&Service::reg, this, _1, _2, _3)});
    serviceMapping_.insert({REQ_CODE::REG_CODE_LOGIN, std::bind(&Service::login, this, _1, _2, _3)});
    serviceMapping_.insert({REQ_CODE::REG_CODE_ADDFRIEDN, std::bind(&Service::addFriend, this, _1, _2, _3)});
    serviceMapping_.insert({REQ_CODE::REG_CODE_GETOFFLINEMSG, std::bind(&Service::getOfflienMsg, this, _1, _2, _3)});
    serviceMapping_.insert({REQ_CODE::REG_CODE_ACCEPTFRIEND, std::bind(&Service::acceptFriend, this, _1, _2, _3)});
    serviceMapping_.insert({REQ_CODE::REG_CODE_GETFRIENDLIST, std::bind(&Service::getFriendList, this, _1, _2, _3)});
}

void Service::reg(const TcpConnectionPtr &ptr, Json::Value &json, muduo::Timestamp)
{
    LOG_INFO << "\nregister";
    User user;

    //参数校验
    if(json["account"].isNull() || json["password"].isNull() || json["account"].asString()=="" || json["password"].asString() == "")
        sendResponse(ptr,RES_CODE::RES_CODE_FAILE,"incorrect parameter");

    // 判断该用户是否已经注册过
    if (userModel_.selectUserByAccount(user, json["account"].asString()))
    {
        sendResponse(ptr, RES_CODE::RES_CODE_FAILE, "account is exists");
        return;
    }

    // 该用户还未注册
    user.setAccount(json["account"].asString());
    user.setPassword(json["password"].asString());
    user.setNickname(json["nickname"].asString());
    if (userModel_.insert(user))
    {
        sendResponse(ptr, RES_CODE::RES_CODE_OK, "register success");
    }
    else
    {
        sendResponse(ptr, RES_CODE::RES_CODE_FAILE, "register fail");
    }
}

void Service::login(const TcpConnectionPtr &ptr, Json::Value &json, muduo::Timestamp)
{
    LOG_INFO << "\nlogin";
    User user;
    Json::Value res;
    // 判断参数是否正确
    if (json["account"].isNull() || json["password"].isNull() || json["account"].asString() == "" || json["password"].asString() == "")
    {
        sendResponse(ptr, RES_CODE::RES_CODE_FAILE, "incorrect parameter");
        return;
    }

    // 判断用户是否已注册过
    if (userModel_.selectUserByAccount(user, json["account"].asString()))
    {
        // 判断该用户是否在线
        if (user.isOnline())
        {
            sendResponse(ptr, RES_CODE::RES_CODE_FAILE, "no repeat logins");
            return;
        }

        // 如果该账户存在则验证密码
        if (user.getPassword() == json["password"].asString())
        {
            // 密码正确
            user.setOnline(true);
            userModel_.update(user);
            res["id"] = user.getId();
            res["nickname"] = user.getNickname();
            {
                std::lock_guard guard(mutex_);
                conns_.insert({user.getId(), ptr});
                LOG_INFO << "\n用户上线，id is " << user.getId();
            }
            sendResponse(ptr, RES_CODE::RES_CODE_OK, res);
        }
        else
        {
            sendResponse(ptr, RES_CODE::RES_CODE_FAILE, "incorrect pawword");
        }
    }
    else
    {
        // 该用户还未注册过
        sendResponse(ptr, RES_CODE::RES_CODE_FAILE, "please register first");
    }
}

void Service::addFriend(const TcpConnectionPtr &ptr, Json::Value &json, muduo::Timestamp)
{
    LOG_INFO << "\naddFriend";
    User user;
    Friend fd;
    // 参数判断
    if (json["id"].isNull() || !json["id"].isInt() || json["account"].isNull() || json["account"].asString() == "")
    {
        sendResponse(ptr, RES_CODE::RES_CODE_FAILE, "incorrect parameter");
        return;
    }

    if( !userIsOnline(json["id"].asUInt(),ptr)) return;

    // 判断需要添加的好友是否存在
    if (!userModel_.selectUserByAccount(user,json["account"].asString()))
    {
        //不存在
        sendResponse(ptr, RES_CODE::RES_CODE_FAILE, "account not find");
        return;
    }

    //判断是否是自己添加自己为好友
    if(user.getId() == json["id"].asUInt())
    {
        sendResponse(ptr, RES_CODE::RES_CODE_FAILE, "you cant add youself");
        return;
    }


    //判断是否已经是好友
    fd.myid_ = json["id"].asUInt();
    fd.friendid_ = user.getId();
    if(friendmodel_.isMyfriend(fd))
    {
        sendResponse(ptr, RES_CODE::RES_CODE_FAILE, "The other person is already your friend");
        return;
    }


    //判断对方是否在线
    if(user.isOnline())
    {
        Json::Value msg;
        msg["fromid"] = json["id"].asUInt();
        msg["msg_type"] = MSG_TYPE::MSG_TYPE_ADDFRIEND;
        if(!json["content"].isNull() && json["content"].asString()!="") msg["content"] = json["content"];
        {
            auto it = conns_.find(user.getId());
            if(it!=conns_.end()) it->second->send(jswriter.write(msg));
        }
    }
    else
    {
        Offlinemsg offline;
        offline.fromid_ = json["id"].asUInt();
        offline.toid_ = user.getId();
        offline.msg_type_ = MSG_TYPE::MSG_TYPE_ADDFRIEND;
        offline.accepted_ = false;
        offline.content_ = json["content"].asString();
        offlinemsgmodel_.insert(offline);
    }
    sendResponse(ptr,RES_CODE::RES_CODE_OK,"");
}

void Service::sendResponse(const TcpConnectionPtr &ptr, int rescode, const char *content)
{
    Json::Value res;
    res["RES_CODE"] = rescode;
    res["content"] = content;
    ptr->send(jswriter.write(res));
}

void Service::sendResponse(const TcpConnectionPtr &ptr, int rescode, Json::Value &content)
{
    content["RES_CODE"] = rescode;
    ptr->send(jswriter.write(content));
}

void Service::removeConn(const TcpConnectionPtr &ptr)
{
    unsigned id;
    User user;
    auto it = conns_.begin();
    for (it = conns_.begin(); it != conns_.end(); it++)
    {
        if (it->second.get() == ptr.get())
        {
            userModel_.selectUserById(user, it->first);
            id = user.getId();
            conns_.erase(it);
            break;
        }
    }
    if (it != conns_.end())
    {
        user.setOnline(false);
        userModel_.update(user);
        LOG_INFO << "\n用户下线，id is " << id;
    }
}





void Service::getOfflienMsg(const TcpConnectionPtr &ptr,Json::Value &json,muduo::Timestamp)
{
    LOG_INFO <<"\n" << "getOfflineMsg";
    //参数验证
    if(json["id"].isNull() || !json["id"].isInt())
    {
        sendResponse(ptr,RES_CODE::RES_CODE_FAILE,"incorrect parameter");
        return;
    }
    if(!userIsOnline(json["id"].asUInt(),ptr)) return;

    auto msgs = offlinemsgmodel_.selectOfflinemsgById(json["id"].asUInt());
    Json::Value res;
    Json::Value arr;
    for(const Offlinemsg &e:msgs)
    {
        Json::Value obj;
        obj["fromid"] = e.fromid_;
        obj["msg_type"] = e.msg_type_;
        obj["content"] = e.content_;
        obj["datetime"] = e.datetime_;
        arr.append(obj);
    }
    res["msg"] = arr;
    sendResponse(ptr,RES_CODE::RES_CODE_OK,res);
}



void Service::acceptFriend(const TcpConnectionPtr &ptr,Json::Value &json,muduo::Timestamp)
{
    LOG_INFO <<"\n" << "acceptFriend";
    Friend f;
    //参数验证
    if(json["fromid"].isNull() || json["toid"].isNull() || json["agree"].isNull() ||  !json["fromid"].isInt() || !json["toid"].isInt() || !json["agree"].isBool())
    {
        sendResponse(ptr,RES_CODE::RES_CODE_FAILE,"incorrect parameter");
        return;
    }

    if( !userIsOnline(json["toid"].asUInt(),ptr)) return;
    //添加好友
    if(json["agree"].asBool()){
        f.myid_ = json["toid"].asUInt();
        f.friendid_ = json["fromid"].asUInt();
        f.remarks_ = json["remarks"].asString();
        if(friendmodel_.insert(f)) sendResponse(ptr,RES_CODE::RES_CODE_OK,""); else sendResponse(ptr,RES_CODE::RES_CODE_FAILE,"fail");
        return;
    }
    sendResponse(ptr,RES_CODE::RES_CODE_OK,"");
}



bool Service::userIsOnline(unsigned int id,const TcpConnectionPtr &ptr)
{
    User user;
    if(userModel_.selectUserById(user,id) && user.isOnline())
    {
        return true;
    }
    sendResponse(ptr,RES_CODE::RES_CODE_FAILE,"please login first");
    return false;
}


void Service::getFriendList(const TcpConnectionPtr &ptr,Json::Value &json,muduo::Timestamp)
{
    LOG_INFO << "\ngetFriendList";
    unsigned int id;
    User user;
    //参数验证
    if(json["id"].isNull() || !json["id"].isInt())
    {
        sendResponse(ptr,RES_CODE::RES_CODE_FAILE,"incorrect parameter");
        return;
    }
    id = json["id"].asUInt();
    if(!userIsOnline(id,ptr)) return;
    std::vector<Friend> friends = friendmodel_.selectFriendsById(id);
    Json::Value res;
    Json::Value array;
    for(const Friend &f: friends) {
        Json::Value obj;
        obj["id"] = f.friendid_;
        userModel_.selectUserById(user,f.friendid_);
        obj["account"] = user.getAccount();
        obj["remarks"] = f.remarks_;
        array.append(obj);
    }
    res["friends"] = array;
    sendResponse(ptr,RES_CODE::RES_CODE_OK,res);
}
