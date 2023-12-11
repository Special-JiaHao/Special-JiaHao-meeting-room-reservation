# Special-JiaHao-meeting-room-reservation
会议室预约系统
## 目录
- [需求说明](#需求说明)
- [Mysql配置](#Mysql配置)
- [系统运行](#系统运行)
- [系统演示](#系统演示)
- [设计逻辑(状态机模型)](#设计逻辑(状态机模型))

## 需求说明
- 首先进入登录界面，可选登录身份有：
  - 业务人员
  - 行政人员
  - 系统管理员
  - 退出
- 选择上述自己身份后，进入子菜单，每个身份都需要进行验证后，输入下列信息：
  - 账号
  - 密码
- 验证通过后，根据自己不同的身份，进入对应二级子菜单
  - 业务人员
    - 申请预约——预约会议室
    - 查看所有预约——查看全部预约信息以及预约状态
    - 查看自己预约——查看自己的预约状态
    - 取消预约 —— 取消会议室的预约
    - 退出 ——退出登录
  - 行政人员
    - 查看所有预约——查看全部预约信息以及预约状态
    - 审批预约——对职员的预约进行审批
    - 退出 ——退出登录
  - 管理员具体功能
    - 添加账户——添加业务人员或行政的账户, 账户名具有唯一性。
      - 添加信息—— 工号、姓名、用户名、密码
    - 查看账户
      - 查看业务人员账户
      - 查看行政人员账户
     - 查看会议室信息——可以查看所有会议室的名称、容量、预约信息
    - 清空预约——清空所有预约记录
    - 退出——退出登录
      
![需求说明](https://github.com/Special-JiaHao/images/blob/main/%E9%9C%80%E6%B1%82%E8%AF%B4%E6%98%8E.png?raw=true)
## Mysql配置
mysql数据库需要预先生成`administrator`(管理员身份表), `business_user`(业务人员身份表), `staff`(行政人员身份表)以及`records`(预约记录表), sql命令如下:
```sql
> create table administrator(user varchar(255) not null, password varchar(255) not null);
> create table business_user(jid varchar(255) not null, name varchar(255) not null, user varchar(255) not null, password varchar(255) not null);
> create table staff(jid varchar(255) not null, name varchar(255) not null, user varchar(255) not null, password varchar(255) not null);
> create table records(rid int not null auto_increment primary key, jid varchar(255) not null, name varchar(255) not null, user varchar(255) not null, data varchar(255) not null, period varchar(255) not null, status int not null, room_id int not null);
```
数据库配置文件在`s1.cpp`中进行相应的修改,修改内容如下:
```cpp
/* 数据库配置参数 */
char HOST[50] = "127.0.0.1";  /* IP 127.0.0.1 */
char USER[50] = "root";  /* User root */
char PASSWORD[50] = "123456"; /* password root */
char DATABASE[50] = "reservation_system"; /* database reservation_system */
const int PORT = 3306; /* port 3306 */
```


## 系统运行
- 编译/运行服务端


```shell
> g++ message.cpp telecom.cpp server.cpp client.cpp s1.cpp database.cpp -o s1 -lpthread -lmysqlclient 
> ./s1
```
- 编译/运行服务端

```shell
> g++ message.cpp telecom.cpp server.cpp client.cpp database.cpp  c1.cpp -o c1 -lpthread -lmysqlclient
> ./c1
```
## 系统演示
[【Bilibili 金山预约系统演示】](https://raw.githubusercontent.com/Special-JiaHao/images/main/金山预约系统架构.png)
## 系统架构C/S
系统采用C/S架构，服务端开启监听端口，用于接收客户端的访问请求，使用多线程去分别处理用户请求，同时服务端与Mysql数据库进行数据传输；客户端通过服务端开启的端口与服务端建立连接，同时接收用户输入发送响应的请求.
![系统架构](https://github.com/Special-JiaHao/images/blob/main/%E9%87%91%E5%B1%B1%E9%A2%84%E7%BA%A6%E7%B3%BB%E7%BB%9F%E6%9E%B6%E6%9E%84.png?raw=true)

## 设计逻辑(状态机模型)
![状态机模型](https://github.com/Special-JiaHao/images/blob/main/%E9%87%91%E5%B1%B1%E7%B3%BB%E7%BB%9F%E9%80%BB%E8%BE%91.png?raw=true)
