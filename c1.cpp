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
int main()
{
    Client client("127.0.1.0", 8850);
    client.connectSystem("127.0.0.1", 8899);
    return 0;
}
