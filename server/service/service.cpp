#include "service.hpp"
#include "public.hpp"

#include <functional>
#include <muduo/base/Logging.h>
#include <json/json.h>
#include <muduo/base/Logging.h>

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
    serviceMapping_.insert({static_cast<int>(REQ_CODE::REG_CODE_REG), std::bind(&Service::reg, this, _1, _2, _3)});
    serviceMapping_.insert({static_cast<int>(REQ_CODE::REG_CODE_LOGIN), std::bind(&Service::login, this, _1, _2, _3)});
}

void Service::reg(const TcpConnectionPtr &ptr, Json::Value &json, muduo::Timestamp)
{
    LOG_INFO <<"\nregister";
    User user;
    LOG_INFO << jswriter.write(json);

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
    LOG_INFO <<"login";
    User user;
    Json::Value res;
    // 判断参数是否正确
    if (json["account"].isNull() || json["password"].isNull() || json["account"].asString()=="" || json["password"].asString()=="")
    {
        sendResponse(ptr,RES_CODE::RES_CODE_FAILE,"incorrect parameter");
        return;
    }
    

    // 判断用户是否已注册过
    if (userModel_.selectUserByAccount(user, json["account"].asString()))
    {
        //判断该用户是否在线
        if(user.isOnline())
        {
            sendResponse(ptr,RES_CODE::RES_CODE_FAILE,"no repeat logins");
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
                LOG_INFO <<"\n用户上线，id is " << user.getId();
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
    for(it = conns_.begin();it!=conns_.end();it++)
    {
        if(it->second.get() == ptr.get())
        {
            userModel_.selectUserById(user,it->first);
            id = user.getId();
            conns_.erase(it);
            break;
        }
    }
    user.setOnline(false);
    userModel_.update(user);
    if(it!=conns_.end())
    {
        LOG_INFO <<"\n用户下线，id is " << id;
    }
}
