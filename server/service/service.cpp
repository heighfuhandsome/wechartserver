#include "service.hpp"
#include "public.hpp"

#include <functional>
#include <muduo/base/Logging.h>
#include <json/json.h>
#include <muduo/base/Logging.h>

// 使用枚举来对应具体所要执行的function

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
    serviceMapping_.insert({static_cast<int>(REQ_CODE::REG), std::bind(&Service::reg, this, _1, _2, _3)});
    serviceMapping_.insert({static_cast<int>(REQ_CODE::LOGIN), std::bind(&Service::login, this, _1, _2, _3)});
}

void Service::reg(const TcpConnectionPtr &ptr, Json::Value &json, muduo::Timestamp)
{
    User user;
    Json::Value res;
    Json::FastWriter jsonWriter;
    


    // 判断该用户是否已经注册过
    if (userModel_.selectUserByAccount(user, json["account"].asString()))
    {
        res["RES_CODE"] = static_cast<int>(RES_CODE::FAILE);
        res["RES_STR"] = "该用户已经注册过";
        ptr->send(jsonWriter.write(res));
        return;
    }

    //该用户还未注册
    user.setAccount(json["account"].asString());
    user.setPassword(json["password"].asString());
    if (userModel_.insert(user))
    {
        res["RES_CODE"] = static_cast<int>(RES_CODE::OK);
        res["RES_STR"] = "注册成功";
    }
    else
    {
        res["RES_CODE"] = static_cast<int>(RES_CODE::FAILE);
        res["RES_STR"] = "注册失败";
    }
    res["REQ_CODE"] = json["REQ_CODE"];
    ptr->send(jsonWriter.write(res));
}

void Service::login(const TcpConnectionPtr &ptr, Json::Value &json, muduo::Timestamp)
{
    User user;
    Json::Value res;
    Json::FastWriter jsonWriter;

    //判断用户是否已注册过
    if(userModel_.selectUserByAccount(user,json["account"].asString())){
        //如果该账户存在则验证密码
        if (user.getPassword() == json["password"].asString())
        {
            //密码正确
            res["RES_CODE"] = static_cast<int>(RES_CODE::OK);
            user.setOnline(true);
            userModel_.update(user);
            res["id"] = user.getId();
            res["nickname"] = user.getNickname();
            {
                std::lock_guard guard(mutex_);
                conns_.insert({user.getId(),ptr});
            }
        }else{
            res["RES_CODE"] = static_cast<int>(RES_CODE::FAILE);
            res["RES_STR"] = "密码不正确";
        }
    }else{
        //该用户还未注册过
        res["RES_CODE"] = static_cast<int>(RES_CODE::FAILE);
        res["RES_STR"] = "该用户未注册";
    }
    res["REQ_CODE"] = json["REQ_CODE"];
    ptr->send(jsonWriter.write(res));
}
