#pragma once
#include <string>
enum REQ_CODE:unsigned short{
    REG_CODE_REG, //注册请求
    REG_CODE_LOGIN, //登录请求
    REG_CODE_ADDFRIEDN //添加好友请求
};

enum RES_CODE:unsigned short{
    RES_CODE_OK, 
    RES_CODE_FAILE 
};