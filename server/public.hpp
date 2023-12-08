#pragma once
#include <string>
enum REQ_CODE:unsigned short{
    REG_CODE_REG, //注册请求
    REG_CODE_LOGIN, //登录请求
    REG_CODE_ADDFRIEDN ,//添加好友请求
    REG_CODE_GETOFFLINEMSG, //添加好友请求
    REG_CODE_ACCEPTFRIEND, //接受/拒绝 好友请求
    REG_CODE_GETFRIENDLIST, //获取好友列表

};

enum RES_CODE:unsigned short{
    RES_CODE_OK, 
    RES_CODE_FAILE 
};


enum MSG_TYPE:unsigned int{
    MSG_TYPE_NORMAL,  
    MSG_TYPE_ADDFRIEND
};
