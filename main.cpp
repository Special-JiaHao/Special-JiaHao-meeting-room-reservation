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
char HOST[50] = "127.0.0.1";  /* IP 127.0.0.1 */
char USER[50] = "root";  /* User root */
char PASSWORD[50] = "123456"; /* password root */
char DATABASE[50] = "reservation_system"; /* database reservation_system */
const int PORT = 3306; /* port 3306 */
void init() {
    do {
        printf("MYSQL数据库IP：");  scanf("%s", HOST);
        printf("User用户名："); scanf("%s", USER);
        printf("Password密码：");  scanf("%s", PASSWORD);
        printf("Database name数据库名称：");  scanf("%s", DATABASE);
        MYSQL* mysql = mysql_init(nullptr);
        mysql_options(mysql, MYSQL_SET_CHARSET_NAME, "GBK");
        if (!mysql_real_connect(mysql, HOST, USER, PASSWORD, DATABASE, PORT, nullptr, 0)) {
            fprintf(stderr, "Failed to connect to database : %s\n", mysql_error(mysql));
            std::string flag;
            printf("是否重新输入(Y/N)：");
            std::cin >> flag;
            if (flag == "Y" || flag == "y") continue;
            else    return;
        }
        break;
    } while (true);
    printf("connect successfully!\n");
    printf("==============================  欢迎来到金山办公会议预约系统  ==============================\n");
    printf("请输入你的身份：\n");
    printf("                            +-------------------------------+                            \n");
    printf("                            |           1. 业务人员          |                           \n");
    printf("                            |           2. 行政人员          |                           \n");
    printf("                            |           3. 管理人员          |                           \n");
    printf("                            |           4.  退 出           |                           \n");
    printf("                            +-------------------------------+                            \n");
}
void* createServer(void* arg) {
    Server server("127.0.0.1", 8899);
    server.configMysql(HOST, USER, PASSWORD, DATABASE);
    server.run();
    sleep(100);
}

void* createClient(void* arg) {
    Client client("127.0.1.0", 8850);
    client.connectSystem("127.0.0.1", 8899);
    sleep(100);
}

int main()
{
    pthread_t serverTid;
    pthread_create(&serverTid, nullptr, createServer, nullptr);
    pthread_detach(serverTid);
    
    sleep(3);
    pthread_t clientTid;
    pthread_create(&clientTid, nullptr, createClient, nullptr);
    pthread_detach(clientTid);
    
    sleep(200);
    return 0;
}

