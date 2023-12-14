#ifndef CLASS_DATABASE
#define CLASS_DATABASE
#include <mysql/mysql.h>
#include <iostream>
#include <vector>
#include "message.h"
class Database {
public:
	Database();
	Database(char* HOST, char* USER, char* PASSWORD, char* DATABASE, int PORT);
	~Database();
	void connectMysql(const char host[], const char user[], const char password[], const char database[], int port);
	bool testMysqlConfigure(const char host[], const char user[], const char password[], const char database[], int port);

	bool insertAccount(std::string& table, Account& account);
	bool insertAccount(std::string& table, char jID[], char name[], char user[], char password[]);
	bool insertAccount(std::string& table, Member member);
	bool insertRecord(char jID[], char name[], char user[], char data[], char period[], int roomID, int status);
	bool insertRecord(Record record);
	bool deleteRecord( int recordID);
	bool deleteAllRecord();
	bool updateRecordStatus(int recordID, int status);
	std::vector<Account> queryAccount(std::string& table, std::string condition);
	std::vector<Record> queryRecord(std::string condition);
private:
	MYSQL* m_pMysql;
};
#endif