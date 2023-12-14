#ifndef CLASS_SERVER
#define CLASS_SERVER
#include <mysql/mysql.h>
#include <netinet/in.h>
#include <cstring>
#include <vector>
#include <memory>
#include "message.h"
#include "database.h"
class Server {
public:
	Server(int maxSize=100);
	Server(std::string addr, in_port_t listenPort, int maxSize = 100);
	~Server();
	void setServerAddr(std::string addr);
	void setListenPort(in_port_t listenPort);
	std::shared_ptr<sockaddr_in> getListenSocket();
	bool testMysqlConfigure();
	void configMysql(const char host[], const char user[], const char password[], const char database[], int port=3306);
	void run();
	static void* listenForRequests(void* arg);
	static void* dealRequest(void* arg);
	std::shared_ptr<char> HOST;
	std::shared_ptr<char> USER;
	std::shared_ptr<char> PASSWORD;
	std::shared_ptr<char> DATABASE;
	int PORT;
private:
	int m_nMaxSize;
	in_addr_t addr;
	std::shared_ptr<sockaddr_in> listenSocket;
};
#endif
