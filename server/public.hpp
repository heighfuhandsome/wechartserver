#pragma once
#include <string>
enum class REQ_CODE:unsigned short{
    REG, //注册请求
    LOGIN, //登录请求
};

enum class RES_CODE:unsigned short{
    OK, 
    FAILE 
};