### 注册

*请求格式*

```json
/*
account : 账号 (字符串)
password: 密码 (字符串)
nickname: 昵称 (字符串)
*/
{"REQ_CODE":0,"account":"1201","password":"123456","nickname":"whm"}
```

*响应格式*

```json
//REQ_CODE: 0 成功 1 失败
//content : 失败原因
{"REQ_CODE":,"content":""}
```



### 登录

*请求格式*

```json
/*
account : 账号 (字符串)
password: 密码 (字符串)
*/
{"REQ_CODE":1,"account":"1201","password":"123456"}
```

*响应格式*

```json
//成功
/*
nickname: 昵称 (字符串)
id		: 该用户的id (整形)
*/
{"id":,"nickname":,"RES_CODE":0}

//失败
/*
REQ_CODE: 0 成功 1 失败
content : 失败原因
*/
{"REQ_CODE":,"content":""}
```



### 添加好友 

*请求格式*

```json
/*
id 		->  添加好友发起者id	(整形)
content -> 添加好友时验证消息   (字符串)
account -> 被添加用户的账号	(字符串)
*/

{"REQ_CODE":2,"id":70,"account":"1728","content":"wswhm"}
```

*响应格式*

```json
/*
REQ_CODE: 0 成功 1 失败
content : 失败原因
*/
{"RES_CODE":,"content":}
```

**被添添加者如果在线则会收到请求添加好友消息**  (否则会写入离线消息列表)

```json
/*
fromid: 消息发送着id (整形)
msg_type: 消息类型 0: 普通消息 1: 好友添加消息 (整形)
content: 验证消息 -> 例如 我是XX (字符串)
*/
{"fromid":,"msg_type":1,"content":""}
```



### 接收离线消息

*请求格式*

```json	
{"REQ_CODE":3,"id":69}
```

*响应格式*

```json
//失败
{"RES_CODE":1,"content":""}
//成功
/*
fromid -> 发送者id (整形)
msg_type -> 消息类型 (整形)
content -> 消息内容 (字符串)
datetime -> 时间 (字符串)
*/
{
    "RES_CODE":0,
    "msg":[
        {"fromid":,"msg_type":,"content":"","datetime":""},
        {"fromid":,"msg_type":,"content":"","datetime":""},
        .......
    ]
}
```



### 接受/拒绝好友申请

*请求格式*

```json	
/*
fromid: 发起好友添加者的id (整形)
toid: 接受好友申请者的id (整形)
remarks: 给好友的备注 (字符串)
agree: 1:同意 0:拒绝 (整形)
*/
{"REQ_CODE":4,"fromid":69,"toid":70,"remarks":"hs","agree":}
```

*响应格式*

```json	
/*
RES_CODE: 0->成功 1->失败
*/
{"RES_CODE":,"content":""}
```



### 获取好友列表

*请求格式*

```json
{"REQ_CODE":5,"id":}
```

*响应格式*

```json	
//成功
/*
id: 好友id
account: 好友账号
remarks: 给好友的备注
*/
{
    "res_code":0,
    "friends":[
        {"id":,"account":,"remarks":},
    	{"id":,"account":,"remarks":},
        ........
    ]
}
//失败
//content 失败原因
{"RES_CODE":1,"content":""}

```



### 向指定好友发送消息

*请求格式*

```json
/*
fromid-> 发送者id
toid-> 接收者id
content-> 消息内容
*/
{"fromid","toid",content:""}
```

*响应格式*

```json
{"RES_CODE":,"content":}
```

*接收方获取消息格式*

```json
/*
fromid: 发送者id (整形)
msg_type: 消息类型 (整形)
content: 消息内容 (字符串)
datetime: 时间 (字符串)
*/
{"fromid":,"msg_type":0,"content":"","datetime":""}
```


