#ifndef CLASS_CLIENT
#define CLASS_CLIENT
#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include "message.h"
void showTableData(Message& message, std::string title, std::string type="person");
class Client {
public:
	Client();
	Client(std::string addr, in_port_t correspondPort);
	~Client();
	void setClientAddr(std::string addr);
	void setCorrespondPort(in_port_t correspondPort);
	sockaddr_in* getCorrespondSocket();
	void connectSystem(std::string serverIP, in_port_t serverPort);
private:
	in_addr_t addr;
	sockaddr_in* correspondSocket;
};
#endif