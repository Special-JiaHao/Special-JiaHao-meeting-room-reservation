#define _CRT_SECURE_NO_WARNINGS 1
#include <iostream>
#include <mysql/mysql.h>
#include <cstring>
#include <cstdio>
#include <sys/socket.h>
#include <memory>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include "server.h"
#include "client.h"
/* 数据库配置参数 */
char HOST[50] = "127.0.0.1";  /* IP 127.0.0.1 */
char USER[50] = "root";  /* User root */
char PASSWORD[50] = "123456"; /* password root */
char DATABASE[50] = "reservation_system"; /* database reservation_system */
const int PORT = 3306; /* port 3306 */
int main()
{
    Server server("127.0.0.1", 8899);
    server.configMysql(HOST, USER, PASSWORD, DATABASE);
    server.run();
    return 0;
}
