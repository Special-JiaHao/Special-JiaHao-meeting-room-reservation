#ifndef CLASS_SERVER
#define CLASS_SERVER
#include <mysql/mysql.h>
#include <netinet/in.h>
#include <cstring>
#include <vector>
#include "message.h"
struct Account {
	std::string jID;
	std::string name;
	std::string user;
	std::string password;
};
bool insertAccount(MYSQL* mysql, std::string& table, Account& account);
bool insertAccount(MYSQL* mysql, std::string& table, char jID[], char name[], char user[], char password[]);
bool insertAccount(MYSQL* mysql, std::string& table, Member member);
bool insertRecord(MYSQL* mysql, char jID[], char name[], char user[], char data[], char period[], int roomID, int status);
bool insertRecord(MYSQL* mysql, Record record);
bool deleteRecord(MYSQL* mysql, int recordID);
bool deleteAllRecord(MYSQL* mysql);
bool updateRecordStatus(MYSQL* mysql, int recordID, int status);

std::vector<Account> queryAccount(MYSQL* mysql, std::string& table, std::string condition);
std::vector<Record> queryRecord(MYSQL* mysql, std::string condition);
class Server {
public:
	Server();
	Server(std::string addr, in_port_t listenPort);
	~Server();
	void setServerAddr(std::string addr);
	void setListenPort(in_port_t listenPort);
	sockaddr_in* getListenSocket();
	bool testMysqlConfigure();
	void configMysql(const char host[], const char user[], const char password[], const char database[], int port=3306);
	MYSQL* connectMysql();
	void run();
	static void* listenForRequests(void* arg);
	static void* dealRequest(void* arg);
private:
	char *HOST;
	char *USER;
	char *PASSWORD;
	char *DATABASE;
	int PORT;
	int MAX_SIZE;
	in_addr_t addr;
	sockaddr_in* listenSocket;
};
#endif
