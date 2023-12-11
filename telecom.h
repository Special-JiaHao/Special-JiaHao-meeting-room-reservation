#ifndef CLASS_TELECOM
#define CLASS_TELECOM

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
int teleSend(int sockfd, void* buff, size_t nbytes, int flags = MSG_WAITALL);
int teleReceive(int sockfd, void* buff, size_t nbytes, int flags = MSG_WAITALL);


#endif