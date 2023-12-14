#include <cstring>
#include <iostream>
#include <memory>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <memory>
#include "server.h"
#include "message.h"
#include "telecom.h"
#include "database.h"

struct ConnectMessage {
	sockaddr_in requestSocket;
	int sockfd;
	Database database;
};

Server::Server(int maxSize) {
	m_nMaxSize = maxSize;
	HOST = std::make_shared<char>(m_nMaxSize);
	USER = std::make_shared<char>(m_nMaxSize);
	PASSWORD = std::make_shared<char>(m_nMaxSize);
	DATABASE = std::make_shared<char>(m_nMaxSize);
	listenSocket = std::make_shared<sockaddr_in>();
}

Server::Server(std::string addr, in_port_t listenPort, int maxSize) {
	m_nMaxSize = maxSize;
	HOST = std::make_shared<char>(m_nMaxSize);
	USER = std::make_shared<char>(m_nMaxSize);
	PASSWORD = std::make_shared<char>(m_nMaxSize);
	DATABASE = std::make_shared<char>(m_nMaxSize);
	listenSocket = std::make_shared<sockaddr_in>();
	inet_pton(AF_INET, addr.c_str(), &this->addr);
	listenSocket->sin_addr.s_addr = this->addr;
	listenSocket->sin_port = htons(listenPort);
	listenSocket->sin_family = AF_INET;
}

Server::~Server() {

}

void Server::setServerAddr(std::string addr) {
	inet_pton(AF_INET, addr.c_str(), &this->addr);
	listenSocket->sin_addr.s_addr = this->addr;
	listenSocket->sin_family = AF_INET;
}

void Server::setListenPort(in_port_t listenPort) {
	listenSocket->sin_port = htons(listenPort);
}

std::shared_ptr<sockaddr_in> Server::getListenSocket() {
	return listenSocket;
}

void Server::configMysql(const char host[], const char user[], const char password[], const char database[], int port) {
	memcpy(HOST.get(), host, strlen(host) + 1);
	memcpy(USER.get(), user, strlen(user) + 1);
	memcpy(PASSWORD.get(), password, strlen(password) + 1);
	memcpy(DATABASE.get(), database, strlen(database) + 1);
	PORT = port;
}

void* Server::dealRequest(void* arg) {
	ConnectMessage* conMsg = (ConnectMessage*)arg;
	std::shared_ptr<Message> message = std::make_shared<Message>();
	while (true) {
		if (teleReceive(conMsg->sockfd, (void*)message.get(), sizeof(class Message)) <= 0) {
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
				std::vector<Account> res = conMsg->database.queryAccount(table, condition);
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
				if (teleSend(conMsg->sockfd, (void *)message.get(), sizeof(class Message)) <= 0) {
					delete conMsg;
					return nullptr;
				}
				break;
			}
			case 311:
			case 312: {
				/* 添加业务人员/添加行政人员 */
				std::string table, condition;
				if (message->getCurStatus() == 311)	table = "business_user";
				else table = "staff";
				condition = "user='" + message->getUser() + "'";
				std::vector<Account> res = conMsg->database.queryAccount(table, condition);
				if (res.size() == 0) {
					/* 未冲突, 可添加 */
					conMsg->database.insertAccount(table, message->getMember(1));
					message->setstatusCode(0);
				}
				else {
					/* 有冲突 */
					message->setstatusCode(-1);
					message->setFeedback("用户名冲突!");
				}
				if (teleSend(conMsg->sockfd, (void*)message.get(), sizeof(class Message)) <= 0) {
					delete conMsg;
					return nullptr;
				}
				break;
			}
			case 321: {
				/* 业务人员账户信息 */
				std::string table = "business_user";
				std::vector<Account> res = conMsg->database.queryAccount(table, "");
				message->resetPipeline();
				for(auto &item : res)	message->pushMember(item.jID, item.name, item.user, item.password);
				message->setstatusCode(0);
				if (teleSend(conMsg->sockfd, (void*)message.get(), sizeof(class Message)) <= 0) {
					delete conMsg;
					return nullptr;
				}
				break;
			}
			case 322: {
				/* 行政人员账户 */
				std::string table = "staff";
				std::vector<Account> res = conMsg->database.queryAccount(table, "");
				message->resetPipeline();
				for (auto& item : res)	message->pushMember(item.jID, item.name, item.user, item.password);
				message->setstatusCode(0);
				if (teleSend(conMsg->sockfd, (void*)message.get(), sizeof(class Message)) <= 0) {
					delete conMsg;
					return nullptr;
				}
				break;
			}
			case 11: {
				/* 添加预约记录 */
				conMsg->database.insertRecord(message->getRecord(1));
				message->setstatusCode(0);
				if (teleSend(conMsg->sockfd, (void*)message.get(), sizeof(class Message)) <= 0) {
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
				if(message->getCurStatus() == 12 || message->getCurStatus() == 21 || message->getCurStatus() == 22)	res = conMsg->database.queryRecord("");
				else {
					std::string condition = "user='" + message->getUser() + "'";
					res = conMsg->database.queryRecord(condition);
				}
				message->setstatusCode(0);
				message->resetPipeline();
				for (auto& item : res)	message->pushRecord(item);
				if (teleSend(conMsg->sockfd, (void*)message.get(), sizeof(class Message)) <= 0) {
					delete conMsg;
					return nullptr;
				}
				break;
			}
			case 141: {
				if (conMsg->database.deleteRecord(message->getRecord(1).recordID))	message->setstatusCode(0);
				else {
					message->setstatusCode(-1);
					message->setFeedback("删除预约失败!");
				}
				if (teleSend(conMsg->sockfd, (void*)message.get(), sizeof(class Message)) <= 0) {
					delete conMsg;
					return nullptr;
				}
				break;
			}
			case 221: {
				if (conMsg->database.updateRecordStatus(message->getRecord(1).recordID, message->getRecord(1).status))	message->setCurStatus(0);
				else {
					message->setstatusCode(-1);
					message->setFeedback("审批失败!");
				}
				if (teleSend(conMsg->sockfd, (void*)message.get(), sizeof(class Message)) <= 0) {
					delete conMsg;
					return nullptr;
				}
				break;
			}
		}
	}
	delete conMsg;
	return nullptr;
}

void* Server::listenForRequests(void* arg) {
	Server* server = (Server*)arg;
	std::shared_ptr<sockaddr_in> listenSocket = server->getListenSocket();
	int listenFd = socket(AF_INET, SOCK_STREAM, 0);
	if (bind(listenFd, (struct sockaddr*)listenSocket.get(), sizeof(struct sockaddr_in)) == -1) {
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
		conMsg->database.connectMysql(server->HOST.get(), server->USER.get(), server->PASSWORD.get(), server->DATABASE.get(), server->PORT);
		pthread_t dealRequestTid;
		pthread_create(&dealRequestTid, nullptr, server->dealRequest, conMsg);
		pthread_detach(dealRequestTid);
	}
	close(listenFd);
	return nullptr;
}


bool Server::testMysqlConfigure() {
	Database database;
	if (!database.testMysqlConfigure(HOST.get(), USER.get(), PASSWORD.get(), DATABASE.get(), PORT)) {
		printf("请重新配置Mysql参数!");
		return false;
	}
	return true;
}


void Server::run() {
	if (!testMysqlConfigure()) {
		printf("请重新配置Mysql参数!");
		return;
	};
	pthread_t listenForRequestsTid;
	pthread_create(&listenForRequestsTid, nullptr, this->listenForRequests, this);
	pthread_join(listenForRequestsTid, nullptr);
}
