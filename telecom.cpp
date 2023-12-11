#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include "telecom.h"
int teleSend(int sockfd, void* buff, size_t nbytes, int flags)
{
	int sendlen = send(sockfd, buff, nbytes, flags);
	if (sendlen < 0)
	{
		perror("send error ");
		return -1;
	}
	else if (sendlen == 0)
	{
		printf("connect interrupt.\n");
		return 0;
	}
	return sendlen;
}

int teleReceive(int sockfd, void* buff, size_t nbytes, int flags)
{
	int recvlen = recv(sockfd, buff, nbytes, flags);
	if (recvlen < 0)
	{
		perror("recv error ");
		return -1;
	}
	else if (recvlen == 0)
	{
		printf("connect interrupt.\n");
		return 0;
	}
	return recvlen;
}
