#include <cstring>
#include <iostream>
#include <memory>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include "server.h"
#include "message.h"
#include "telecom.h"

struct ConnectMessage {
	sockaddr_in requestSocket;
	int sockfd;
	MYSQL* mysql;
	void connectionClose() {
		close(sockfd);
		mysql_close(mysql);
	}
};

Server::Server() {
	MAX_SIZE = 100;
	HOST = new char(MAX_SIZE);
	USER = new char(MAX_SIZE);
	PASSWORD = new char(MAX_SIZE);
	DATABASE = new char(MAX_SIZE);
	listenSocket = new sockaddr_in();
}

Server::Server(std::string addr, in_port_t listenPort) {
	MAX_SIZE = 100;
	HOST = new char(MAX_SIZE);
	USER = new char(MAX_SIZE);
	PASSWORD = new char(MAX_SIZE);
	DATABASE = new char(MAX_SIZE);
	listenSocket = new sockaddr_in();
	inet_pton(AF_INET, addr.c_str(), &this->addr);
	listenSocket->sin_addr.s_addr = this->addr;
	listenSocket->sin_port = htons(listenPort);
	listenSocket->sin_family = AF_INET;
}

Server::~Server() {
	delete[]HOST;
	delete[]USER;
	delete[]PASSWORD;
	delete[]DATABASE;
	delete listenSocket;
}

void Server::setServerAddr(std::string addr) {
	inet_pton(AF_INET, addr.c_str(), &this->addr);
	listenSocket->sin_addr.s_addr = this->addr;
	listenSocket->sin_family = AF_INET;
}

void Server::setListenPort(in_port_t listenPort) {
	listenSocket->sin_port = htons(listenPort);
}

sockaddr_in* Server::getListenSocket() {
	return listenSocket;
}

void Server::configMysql(const char host[], const char user[], const char password[], const char database[], int port) {
	memcpy(HOST, host, strlen(host) + 1);
	memcpy(USER, user, strlen(user) + 1);
	memcpy(PASSWORD, password, strlen(password) + 1);
	memcpy(DATABASE, database, strlen(database) + 1);
	PORT = port;
}

void* Server::dealRequest(void* arg) {
	ConnectMessage* conMsg = (ConnectMessage*)arg;
	Message* message = new Message();
	while (true) {
		int recvlen = recv(conMsg->sockfd, (void *)message, sizeof(class Message), MSG_WAITALL);
		if (recvlen <= 0) {
			if (recvlen < 0)	perror("recv error ");
			else	printf("connect interrupt.\n");
			delete message;
			conMsg->connectionClose();
			delete conMsg;
			return nullptr;
		}
		message->setstatusCode(-1);
		switch (message->getCurStatus()) {
			case 1:
			case 2:
			case 3: {
				int id = message->getID();
				std::string table;
				if (id == 1)	table = "business_user";
				else if (id == 2)	table = "staff";
				else table = "administrator";
				std::string condition = "user='" + message->getUser() + "' and password='" + message->getPassword() + "'";
				std::vector<Account> res = queryAccount(conMsg->mysql, table, condition);
				if (res.size() == 0) {
					message->setstatusCode(-1);
					message->setFeedback("未找到账户!");
				}
				else {
					message->setstatusCode(0);
					message->resetPipeline();
					for (auto& item : res)	message->pushMember(item.user, item.password);
					message->setJID(res[0].jID);
					message->setName(res[0].name);
				}
				if (teleSend(conMsg->sockfd, (void *)message, sizeof(class Message)) <= 0) {
					delete message;
					conMsg->connectionClose();
					delete conMsg;
					return nullptr;
				}
				break;
			}
			case 311: {
				/* 添加业务人员 */
				std::string table = "business_user";
				std::string condition = "user='" + message->getUser() + "'";
				std::vector<Account> res = queryAccount(conMsg->mysql, table, condition);
				if (res.size() == 0) {
					/* 未冲突, 可添加 */
					insertAccount(conMsg->mysql, table, message->getMember(1));
					message->setstatusCode(0);
				}
				else {
					/* 有冲突 */
					message->setstatusCode(-1);
					message->setFeedback("用户名冲突!");
				}
				if (teleSend(conMsg->sockfd, (void*)message, sizeof(class Message)) <= 0) {
					delete message;
					conMsg->connectionClose();
					delete conMsg;
					return nullptr;
				}
				break;
			}
			case 312: {
				/* 添加行政人员 */
				std::string table = "staff";
				std::string condition = "user='" + message->getUser() + "'";
				std::vector<Account> res = queryAccount(conMsg->mysql, table, condition);
				if (res.size() == 0) {
					/* 未冲突, 可添加 */
					insertAccount(conMsg->mysql, table, message->getMember(1));
					message->setstatusCode(0);
				}
				else {
					/* 有冲突 */
					message->setstatusCode(-1);
					message->setFeedback("用户名冲突!");
				}
				if (teleSend(conMsg->sockfd, (void*)message, sizeof(class Message)) <= 0) {
					delete message;
					conMsg->connectionClose();
					delete conMsg;
					return nullptr;
				}
				break;
			}
			case 321: {
				/* 业务人员账户信息 */
				std::string table = "business_user";
				std::vector<Account> res = queryAccount(conMsg->mysql, table, "");
				message->resetPipeline();
				for(auto &item : res)	message->pushMember(item.jID, item.name, item.user, item.password);
				message->setstatusCode(0);
				if (teleSend(conMsg->sockfd, (void*)message, sizeof(class Message)) <= 0) {
					delete message;
					conMsg->connectionClose();
					delete conMsg;
					return nullptr;
				}
				break;
			}
			case 322: {
				/* 行政人员账户 */
				std::string table = "staff";
				std::vector<Account> res = queryAccount(conMsg->mysql, table, "");
				message->resetPipeline();
				for (auto& item : res)	message->pushMember(item.jID, item.name, item.user, item.password);
				message->setstatusCode(0);
				if (teleSend(conMsg->sockfd, (void*)message, sizeof(class Message)) <= 0) {
					delete message;
					conMsg->connectionClose();
					delete conMsg;
					return nullptr;
				}
				break;
			}
			case 11: {
				/* 添加预约记录 */
				insertRecord(conMsg->mysql, message->getRecord(1));
				message->setstatusCode(0);
				if (teleSend(conMsg->sockfd, (void*)message, sizeof(class Message)) <= 0) {
					delete message;
					conMsg->connectionClose();
					delete conMsg;
					return nullptr;
				}
				break;
			}
			case 12: 
			case 13:
			case 14:
			case 21:
			case 22:{
				std::vector<Record> res;
				if(message->getCurStatus() == 12 || message->getCurStatus() == 21 || message->getCurStatus() == 22)	res = queryRecord(conMsg->mysql, "");
				else {
					std::string condition = "user='" + message->getUser() + "'";
					res = queryRecord(conMsg->mysql, condition);
				}
				message->setstatusCode(0);
				message->resetPipeline();
				for (auto& item : res)	message->pushRecord(item);
				if (teleSend(conMsg->sockfd, (void*)message, sizeof(class Message)) <= 0) {
					delete message;
					conMsg->connectionClose();
					delete conMsg;
					return nullptr;
				}
				break;
			}
			case 141: {
				if (deleteRecord(conMsg->mysql, message->getRecord(1).recordID))	message->setstatusCode(0);
				else {
					message->setstatusCode(-1);
					message->setFeedback("删除预约失败!");
				}
				if (teleSend(conMsg->sockfd, (void*)message, sizeof(class Message)) <= 0) {
					delete message;
					conMsg->connectionClose();
					delete conMsg;
					return nullptr;
				}
				break;
			}
			case 221: {
				if (updateRecordStatus(conMsg->mysql, message->getRecord(1).recordID, message->getRecord(1).status))	message->setCurStatus(0);
				else {
					message->setstatusCode(-1);
					message->setFeedback("审批失败!");
				}
				if (teleSend(conMsg->sockfd, (void*)message, sizeof(class Message)) <= 0) {
					delete message;
					conMsg->connectionClose();
					delete conMsg;
					return nullptr;
				}
				break;
			}
		}
	}
	delete message;
	conMsg->connectionClose();
	delete conMsg;
}

void* Server::listenForRequests(void* arg) {
	Server* server = (Server*)arg;
	sockaddr_in* listenSocket = server->getListenSocket();
	int listenFd = socket(AF_INET, SOCK_STREAM, 0);
	if (bind(listenFd, (struct sockaddr*)listenSocket, sizeof(struct sockaddr_in)) == -1) {
		perror("bind error ");
		return nullptr;
	}
	if (listen(listenFd, 10) == -1) {
		perror("listen error ");
		close(listenFd);
		return nullptr;
	}
	printf("监听已端口开启,监听中...\n");
	while (true) {
		ConnectMessage* conMsg = new ConnectMessage();
		socklen_t accpetLength;
		conMsg->sockfd = accept(listenFd, (struct sockaddr*)&conMsg->requestSocket, &accpetLength);
		if (conMsg->sockfd == -1) {
			perror("accept error ");
			delete conMsg;
			continue;
		}
		conMsg->mysql = server->connectMysql();
		if (conMsg->mysql == nullptr) {
			delete conMsg;
			continue;
		}
		pthread_t dealRequestTid;
		pthread_create(&dealRequestTid, nullptr, server->dealRequest, conMsg);
		pthread_detach(dealRequestTid);
	}
	close(listenFd);
}

MYSQL* Server::connectMysql() {
	MYSQL* mysql = mysql_init(nullptr);
	mysql_options(mysql, MYSQL_SET_CHARSET_NAME, "GBK");
	if (!mysql_real_connect(mysql, HOST, USER, PASSWORD, DATABASE, PORT, nullptr, 0)) {
		fprintf(stderr, "Failed to connect to database : %s\n", mysql_error(mysql));
		return nullptr;
	}
	return mysql;
}

bool Server::testMysqlConfigure() {
	MYSQL* mysql = connectMysql();
	if (mysql == nullptr) {
		printf("请重新配置Mysql参数!");
		return false;
	}
	mysql_close(mysql);
	return true;
}


void Server::run() {
	if (!testMysqlConfigure())	return;
	pthread_t listenForRequestsTid;
	pthread_create(&listenForRequestsTid, nullptr, this->listenForRequests, this);
	pthread_join(listenForRequestsTid, nullptr);

}

bool insertAccount(MYSQL* mysql, std::string& table, Account& account) {
	char sql[512];
	sprintf(sql, "insert into %s (jid, name, user, password) values('%s', '%s', '%s', '%s')", table.c_str(), account.jID.c_str(), account.name.c_str(), account.user.c_str(), account.password.c_str());
	if (mysql_query(mysql, sql)) {
		fprintf(stderr, "Failed to insert --- Error:%s\n", mysql_error(mysql));
		return false;
	}
	return true;
}

bool insertAccount(MYSQL* mysql, std::string& table, char jID[], char name[], char user[], char password[]) {
	char sql[512];
	sprintf(sql, "insert into %s (jid, name, user, password) values('%s', '%s', '%s', '%s')", table.c_str(), jID, name, user, password);
	if (mysql_query(mysql, sql)) {
		fprintf(stderr, "Failed to insert --- Error:%s\n", mysql_error(mysql));
		return false;
	}
	return true;
}

bool insertAccount(MYSQL* mysql, std::string& table, Member member) {
	char sql[512];
	sprintf(sql, "insert into %s (jid, name, user, password) values('%s', '%s', '%s', '%s')", table.c_str(), member.jid, member.name, member.user, member.password);
	if (mysql_query(mysql, sql)) {
		fprintf(stderr, "Failed to insert --- Error:%s\n", mysql_error(mysql));
		return false;
	}
	return true;
}

bool insertRecord(MYSQL* mysql, char jID[], char name[], char user[], char data[], char period[], int roomID, int status) {
	char sql[512];
	sprintf(sql, "insert into records (jid, name, user, data, period, status, room_id) values('%s', '%s', '%s', '%s', '%s', %d, %d)", jID, name, user, data, period, status, roomID);
	if (mysql_query(mysql, sql)) {
		fprintf(stderr, "Failed to insert --- Error:%s\n", mysql_error(mysql));
		return false;
	}
	return true;
}

bool insertRecord(MYSQL* mysql, Record record) {
	char sql[512];
	sprintf(sql, "insert into records (jid, name, user, data, period, status, room_id) values('%s', '%s', '%s', '%s', '%s', %d, %d)", record.jid, record.name, record.user, record.data, record.period, record.status, record.roomID);
	if (mysql_query(mysql, sql)) {
		fprintf(stderr, "Failed to insert --- Error:%s\n", mysql_error(mysql));
		return false;
	}
	return true;
}

bool deleteRecord(MYSQL* mysql, int recordID) {
	char sql[512];
	sprintf(sql, "delete from records where rid=%d", recordID);
	if (mysql_query(mysql, sql)) {
		fprintf(stderr, "Failed to delete --- Error:%s\n", mysql_error(mysql));
		return false;
	}
	return true;
}

bool deleteAllRecord(MYSQL* mysql) {
	char sql[512];
	sprintf(sql, "delete from records");
	if (mysql_query(mysql, sql)) {
		fprintf(stderr, "Failed to delete --- Error:%s\n", mysql_error(mysql));
		return false;
	}
	return true;
}

bool updateRecordStatus(MYSQL* mysql, int recordID, int status) {
	char sql[512];
	sprintf(sql, "update records set status=%d where rid=%d", status, recordID);
	if (mysql_query(mysql, sql)) {
		fprintf(stderr, "Failed to update --- Error:%s\n", mysql_error(mysql));
		return false;
	}
	return true;
}

std::vector<Account> queryAccount(MYSQL* mysql, std::string& table, std::string condition) {
	char sql[512];
	if (condition == "")	sprintf(sql, "SELECT * FROM %s", table.c_str());
	else	sprintf(sql, "SELECT * FROM %s WHERE %s", table.c_str(), condition.c_str());
	std::vector<Account> account;
	if (mysql_query(mysql, sql)) {
		fprintf(stderr, "Failed to query --- Error:%s\n", mysql_error(mysql));
		return account;
	}
	MYSQL_RES* res = mysql_store_result(mysql);
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(res))) {
		Account cur;
		if (table == "administrator") {
			cur.user = row[0];
			cur.password = row[1];
		}
		else {
			cur.jID = row[0];
			cur.name = row[1];
			cur.user = row[2];
			cur.password = row[3];
		}
		account.push_back(cur);
	}
	return account;
}


std::vector<Record> queryRecord(MYSQL* mysql, std::string condition) {
	char sql[512];
	if (condition == "")	sprintf(sql, "SELECT * FROM records");
	else	sprintf(sql, "SELECT * FROM records WHERE %s", condition.c_str());
	std::vector<Record> record;
	if (mysql_query(mysql, sql)) {
		fprintf(stderr, "Failed to query --- Error:%s\n", mysql_error(mysql));
		return record;
	}
	MYSQL_RES* res = mysql_store_result(mysql);
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(res))) {
		Record cur;
		int i = 0;
		cur.recordID = 0;
		while (row[0][i] != '\0') {
			cur.recordID = cur.recordID * 10 + (row[0][i] - '0');
			i++;
		}
		i = 0;
		while (row[1][i] != '\0') {
			cur.jid[i] = row[1][i];
			i++;
		}
		cur.jid[i] = '\0';

		i = 0;
		while (row[2][i] != '\0') {
			cur.name[i] = row[2][i];
			i++;
		}
		cur.name[i] = '\0';

		i = 0;
		while (row[3][i] != '\0') {
			cur.user[i] = row[3][i];
			i++;
		}
		cur.user[i] = '\0';

		i = 0;
		while (row[4][i] != '\0') {
			cur.data[i] = row[4][i];
			i++;
		}
		cur.data[i] = '\0';

		i = 0;
		while (row[5][i] != '\0') {
			cur.period[i] = row[5][i];
			i++;
		}
		cur.period[i] = '\0';

		cur.status = 0;
		i = 0;
		while (row[6][i] != '\0') {
			cur.status = cur.status * 10 + (row[6][i] - '0');
			i++;
		}

		cur.roomID = 0;
		i = 0;
		while (row[7][i] != '\0') {
			cur.roomID = cur.roomID * 10 + (row[7][i] - '0');
			i++;
		}
		record.push_back(cur);
	}
	return record;
}