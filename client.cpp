#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include "client.h"
#include "message.h"
#include "telecom.h"
Client::Client() {
	correspondSocket = new sockaddr_in();
}

Client::Client(std::string addr, in_port_t correspondPort) {
	correspondSocket = new sockaddr_in();
	setClientAddr(addr);
	setCorrespondPort(correspondPort);
}

Client::~Client() {
	delete correspondSocket;
}

void Client::setClientAddr(std::string addr) {
	inet_pton(AF_INET, addr.c_str(), &this->addr);
	correspondSocket->sin_addr.s_addr = this->addr;
	correspondSocket->sin_family = AF_INET;
}

void Client::setCorrespondPort(in_port_t correspondPort) {
	correspondSocket->sin_port = htons(correspondPort);
}

sockaddr_in* Client::getCorrespondSocket() {
	return correspondSocket;
}

void Client::connectSystem(std::string serverIP, in_port_t serverPort) {
	int correspondFd = socket(AF_INET, SOCK_STREAM, 0);
	if (bind(correspondFd, (struct sockaddr*)getCorrespondSocket(), sizeof(struct sockaddr_in)) == -1) {
		perror("bind error ");
		close(correspondFd);
		return;
	}
	sockaddr_in serverAddr;
	inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr.s_addr);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverPort);
	if (connect(correspondFd, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr_in)) == -1) {
		perror("connect error ");
		close(correspondFd);
		return;
	}
	printf("connect successfully!\n");
	bool exit = false;
	Message message;
	message.setCurStatus(0);
	while (!exit) {
		switch (message.getCurStatus()) {
			case -1: {
				exit = true;
				break;
			}
			case 0: {
				system("clear");
				printf("==============================  欢迎来到金山办公会议预约系统  ==============================\n");
				printf("请输入你的身份：\n");
				printf("                            +-------------------------------+                            \n");
				printf("                            |           1. 业务人员         |                           \n");
				printf("                            |           2. 行政人员         |                           \n");
				printf("                            |           3. 管理人员         |                           \n");
				printf("                            |           4.  退 出          |                           \n");
				printf("                            +-------------------------------+                            \n");
				message.setCurStatus(5);
				break;
			}case 1: {
				std::string user, password;
				printf("(业务人员)请输入用户名：");	std::cin >> user;
				printf("(业务人员)请输入密码：");	std::cin >> password;
				message.setUser(user);
				message.setPassword(password);
				message.setMessageType(false);
				if (teleSend(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (teleReceive(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (message.getStatusCode() == -1) {
					printf("%s 是否重新输入(Y/N):", message.getFeedback().c_str());
					std::string reInput;	std::cin >> reInput;
					if (reInput[0] == 'Y' || reInput[0] == 'y')	message.setCurStatus(1);
					else	message.setCurStatus(5);
					break;
				}
				message.setCurStatus(10);
				break;
			}
			case 2: {
				std::string user, password;
				printf("(行政人员)请输入用户名：");	std::cin >> user;
				printf("(行政人员)请输入密码：");	std::cin >> password;
				message.setUser(user);
				message.setPassword(password);
				message.setMessageType(false);
				if (teleSend(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (teleReceive(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (message.getStatusCode() == -1) {
					printf("%s 是否重新输入(Y/N):", message.getFeedback().c_str());
					std::string reInput;	std::cin >> reInput;
					if (reInput[0] == 'Y' || reInput[0] == 'y')	message.setCurStatus(2);
					else	message.setCurStatus(5);
					break;
				}
				message.setCurStatus(20);
				break;
			}
			case 3: {
				std::string user, password;
				printf("(管理员)请输入用户名：");	std::cin >> user;
				printf("(管理员)请输入密码：");	std::cin >> password;
				message.setUser(user);
				message.setPassword(password);
				message.setMessageType(false);
				
				if (teleSend(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (teleReceive(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}

				if (message.getStatusCode() == -1) {
					printf("%s 是否重新输入(Y/N):", message.getFeedback().c_str());
					std::string reInput;	std::cin >> reInput;
					if (reInput[0] == 'Y' || reInput[0] == 'y')	message.setCurStatus(3);
					else	message.setCurStatus(5);
					break;
				}
				message.setCurStatus(30);
				break;
			}
			case 4: {
				message.setCurStatus(-1);
				break;
			}
			case 5: {
				printf("输入你的身份：");
				int id;
				while (std::cin >> id) {
					if (id > 0 && id <= 4) {
						message.setID(id);
						break;
					}
					printf("输入有误!请重新输入你的身份:");
				}
				message.setCurStatus(id);
				break;
			}
			case 6: {
				printf("程序异常!\n");
				exit = true;
				break;
			}

			case 10: {
				system("clear");
				printf("==============================  欢迎业务人员:%s  ==============================\n", message.getUser().c_str());
				printf("                            +---------------------------------+                            \n");
				printf("                            |            1. 申请预约          |                           \n");
				printf("                            |           2. 查看全部预约       |                           \n");
				printf("                            |           3. 查看我的预约       |                           \n");
				printf("                            |            4. 取消预约          |                           \n");
				printf("                            |            5. 退出登录          |                           \n");
				printf("                            +---------------------------------+                            \n");
				message.setCurStatus(16);
				break;
			}
			case 11: {
				printf("+------------会议室------------+\n");
				printf("| 会议室号 | 会议室 | 容量(人) |\n");
				printf("+----------|--------|----------+\n");
				printf("|   1号    | 紫阳湖 |     14   |\n");
				printf("+----------|--------|----------+\n");
				printf("|   2号    | 后官湖 |     8    |\n");
				printf("+----------|--------|----------+\n");
				printf("|   3号    | 清潭湖 |     10   |\n");
				printf("+----------|--------|----------+\n");
				printf("|   4号    | 黄家湖 |     6    |\n");
				printf("+----------|--------|----------+\n");
				printf("|   5号    | 杨春湖 |     18   |\n");
				printf("+----------|--------|----------+\n");
				printf("|   6号    | 中山湖 |    150   |\n");
				printf("+----------|--------|----------+\n");
				printf("请输入你需要预约的会议室编号(0退出):");
				int option1;
				while (std::cin >> option1) {
					if (option1 >= 0 && option1 <= 6)	break;
					printf("输入有误!请重新输入你需要预约的会议室编号(0退出):");
				}
				if (option1 == 0) {
					message.setCurStatus(16);
					break;
				}
				printf("+------|--------+\n");
				printf("| 编号 |  日期  |\n");
				printf("+------|--------+\n");
				printf("|  1   | 星期一 |\n");
				printf("+------|--------+\n");
				printf("|  2   | 星期二 |\n");
				printf("+------|--------+\n");
				printf("|  3   | 星期三 |\n");
				printf("+------|--------+\n");
				printf("|  4   | 星期四 |\n");
				printf("+------|--------+\n");
				printf("|  5   | 星期五 |\n");
				printf("+------|--------+\n");
				printf("|  6   | 星期六 |\n");
				printf("+------|--------+\n");
				printf("|  7   | 星期天 |\n");
				printf("+------|--------+\n");
				int option2;
				printf("请输入你需要预约的日期编号(0退出):");
				while (std::cin >> option2) {
					if (option2 >= 0 && option2 <= 7)	break;
					printf("输入有误!请重新输入你需要预约的日期编号(0退出):");
				}
				if (option2 == 0) {
					message.setCurStatus(16);
					break;
				}
				printf("+-----|------+\n");
				printf("| 编号 | 时段 |\n");
				printf("+-----|------+\n");
				printf("|  1  | 上午 |\n");
				printf("+-----|------+\n");
				printf("|  2  | 下午 |\n");
				printf("+-----|------+\n");
				int option3;
				printf("请输入你要预约的时段编号(0退出):");
				while (std::cin >> option3) {
					if (option3 >= 0 && option3 <= 2)	break;
					printf("输入有误!请重新输入你要预约的时段编号(0退出):");
				}
				if (option3 == 2) {
					message.setCurStatus(16);
					break;
				}
				std::string data, period;
				if (option2 == 1)	data = "Monday";
				else if (option2 == 2)	data = "Tuesday";
				else if (option2 == 3)	data = "Wednesday";
				else if (option2 == 4)	data = "Thursday";
				else if (option2 == 5)	data = "Friday";
				else if (option2 == 7)	data = "Saturday";
				else	data = "Sunday";
				if (option3 == 1)	period = "morning";
				else	period = "afternoon";
				message.resetPipeline();
				std::string jid = message.getJID();
				std::string name = message.getName();
				std::string user = message.getUser();
				message.pushRecord(jid, name, user, data, period, option1);
				if (teleSend(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (teleReceive(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (message.getStatusCode() == -1) {
					printf("%s 是否重新输入(Y/N):", message.getFeedback().c_str());
					std::string reInput;	std::cin >> reInput;
					if (reInput[0] == 'Y' || reInput[0] == 'y')	message.setCurStatus(11);
					else	message.setCurStatus(16);
					break;
				}
				printf("预约成功!等待审核...\n");
				message.setCurStatus(10);
				break;
			}
			case 12: 
			case 13: {
				if (teleSend(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (teleReceive(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (message.getStatusCode() == -1)	printf("%s", message.getFeedback().c_str());
				else {
					if(message.getCurStatus() == 12)	showTableData(message, "All Reservation Record", "record");
					else	showTableData(message, "My Reservation Record", "record");
				}
				message.setCurStatus(16);
				break;
			}
			case 14: {
				if (teleSend(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (teleReceive(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (message.getStatusCode() == -1) {
					printf("%s", message.getFeedback().c_str());
					message.setCurStatus(16);
					break;
				}
				showTableData(message, "My Reservation Record", "record");
				message.setCurStatus(141);
				break;
			}
			case 141: {
				printf("请输入你要取消的预约记录ID：");
				int id, recordID;
				while (std::cin >> id) {
					if (id < 0 || id > message.getRecordCount())	printf("输入有误!请输入你要取消的预约记录ID(0退出)：");
					recordID = message.getRecord(id).recordID;
					break;
				}
				if (id == 0) {
					message.setCurStatus(16);
					break;
				}
				message.resetPipeline();
				message.pushRecord(recordID);
				if (teleSend(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (teleReceive(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (message.getStatusCode() == -1) {
					printf("%s", message.getFeedback().c_str());
					message.setCurStatus(16);
					break;
				}
				printf("取消预约成功!\n");
				message.setCurStatus(16);
				break;
			}
			case 15: {
				message.setCurStatus(0);
				break;
			}
			case 16: {
				printf("输入你的选择：");
				int option;
				while (std::cin >> option) {
					if (option >= 1 && option <= 5)	break;
					printf("输入有误!请重新输入:");
				}
				message.setCurStatus(10 + option);
				break;
			}
			case 20: {
				system("clear");
				printf("==============================  欢迎行政人员:%s  ==============================\n", message.getUser().c_str());
				printf("                            +---------------------------------+                            \n");
				printf("                            |           1. 查看全部预约         |                           \n");
				printf("                            |            2. 审批预约           |                           \n");
				printf("                            |            3. 退出登录           |                           \n");
				printf("                            +---------------------------------+                            \n");
				message.setCurStatus(25);
				break;
			}
			case 21:
			case 22: {
				if (teleSend(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (teleReceive(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (message.getStatusCode() == -1)	printf("%s", message.getFeedback().c_str());
				else	showTableData(message, "All Reservation Record", "record");
				if (message.getCurStatus() == 21)	message.setCurStatus(25);
				else	message.setCurStatus(221);
				break;
			}
			case 221: {
				printf("请输入需要审批的记录ID(0退出)：");
				int id, recordID;
				while (std::cin >> id) {
					if (id < 0 || id > message.getRecordCount())	printf("输入有误!请重新输入需要审批的记录ID(0退出)：");
					if(id != 0)	recordID = message.getRecord(id).recordID;
					break;
				}
				if (id == 0) {
					message.setCurStatus(25);
					break;
				}
				printf("审批是否通过(Y/N):");
				std::string op;
				while (std::cin >> op) {
					if (op[0] == 'Y' || op[0] == 'N' || op[0] == 'y' || op[0] == 'n')	break;
					printf("输入有误!请重新输入审批是否通过(Y/N):");
				}
				message.resetPipeline();
				if (op[0] == 'Y' || op[0] == 'y')	message.pushRecord(recordID, 1);
				else	message.pushRecord(recordID, 2);
				if (teleSend(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (teleReceive(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (message.getStatusCode() == -1)	printf("%s", message.getFeedback().c_str());
				else	printf("审批成功!\n");
				message.setCurStatus(22);
				break;
			}
			case 23: {
				message.setCurStatus(0);
				break;
			}
			case 25: {
				printf("请输入你的选择:");
				int option;
				while (std::cin >> option) {
					if (option >= 1 && option <= 3)	break;
					printf("输入有误!请重新输入:");
				}
				message.setCurStatus(20 + option);
				break;
			}
			case 30: {
				system("clear");
				printf("==============================  欢迎管理员:%s  ==============================\n", message.getUser().c_str());
				printf("                            +-------------------------------+                            \n");
				printf("                            |           1. 添加账号          |                           \n");
				printf("                            |           2. 查看账号          |                           \n");
				printf("                            |           3. 产看会议室        |                           \n");
				printf("                            |           4. 注销登录          |                           \n");
				printf("                            |           5. 清空预约          |                           \n");
				printf("                            +-------------------------------+                            \n");
				message.setCurStatus(35);
				break;
			}
			case 31: {
				printf("------------------+--------  账户类别  --------+------------------\n");
				printf("                  |     1. 添加业务人员         |                  \n");
				printf("                  |     2. 添加行政人员         |                  \n");
				printf("                  |     3. 返       回         |                  \n");
				printf("------------------+---------------------------+------------------\n");
				message.setCurStatus(315);
				break;
			}
			case 311: {
				std::string jID, name, user, password;
				printf("(业务人员)请输入工号：");	std::cin >> jID;
				printf("(业务人员)请输入姓名：");	std::cin >> name;
				printf("(业务人员)请输入用户名：");	std::cin >> user;
				printf("(业务人员)请输入密码：");	std::cin >> password;
				message.resetPipeline();
				message.pushMember(jID, name, user, password);
				if (teleSend(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (teleReceive(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (message.getStatusCode() == -1) {
					printf("%s 是否重新添加(Y/N):", message.getFeedback().c_str());
					std::string reInput;	std::cin >> reInput;
					if (reInput[0] == 'Y' || reInput[0] == 'y')	message.setCurStatus(312);
					else	message.setCurStatus(31);
					break;
				}
				printf("添加成功!\n");
				message.setCurStatus(31);
				break;
			}
			case 312: {
				std::string jID, name, user, password;
				printf("(行政人员)请输入工号：");	std::cin >> jID;
				printf("(行政人员)请输入姓名：");	std::cin >> name;
				printf("(行政人员)请输入用户名：");	std::cin >> user;
				printf("(行政人员)请输入密码：");	std::cin >> password;
				message.resetPipeline();
				message.pushMember(jID, name, user, password);
				if (teleSend(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (teleReceive(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (message.getStatusCode() == -1) {
					printf("%s 是否重新添加(Y/N):", message.getFeedback().c_str());
					std::string reInput;	std::cin >> reInput;
					if (reInput[0] == 'Y' || reInput[0] == 'y')	message.setCurStatus(312);
					else	message.setCurStatus(31);
					break;
				}
				printf("添加成功!\n");
				message.setCurStatus(31);
				break;
			}
			case 313: {
				message.setCurStatus(30);
				break;
			}
			case 315: {
				printf("请输入需要添加的账户类别:");
				int option;
				while (std::cin >> option) {
					if (option >= 1 && option <= 3)	break;
					printf("输入有误！请重新输入需要添加账户类别:");
				}
				message.setCurStatus(310 + option);
				break;
			}
			case 32: {
				printf("------------------+--------  账户类别  --------+------------------\n");
				printf("                  |     1. 产看业务人员         |                  \n");
				printf("                  |     2. 查看行政人员         |                  \n");
				printf("                  |     3. 返       回         |                  \n");
				printf("------------------+---------------------------+------------------\n");
				message.setCurStatus(325);
				break;
			}
			case 321: {
				if (teleSend(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (teleReceive(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				showTableData(message, "business_user");
				message.setCurStatus(325);
				break;
			}
			case 322: {
				if (teleSend(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				if (teleReceive(correspondFd, (void*)&message, sizeof(class Message)) <= 0) {
					message.setCurStatus(6);
					break;
				}
				showTableData(message, "staff");
				message.setCurStatus(325);
				break;
			}
			case 323: {
				message.setCurStatus(30);
				break;
			}
			case 325: {
				printf("请输入需要查看的账户类别:");
				int option;
				while (std::cin >> option) {
					if (option >= 1 && option <= 3)	break;
					printf("输入有误！请重新输入需要查看账户类别:");
				}
				message.setCurStatus(320 + option);
				break;
			}
			case 33: {
				printf("+------------会议室------------+\n");
				printf("| 会议室号 | 会议室 | 容量(人) |\n");
				printf("+----------|--------|----------+\n");
				printf("|   1号    | 紫阳湖 |     14   |\n");
				printf("+----------|--------|----------+\n");
				printf("|   2号    | 后官湖 |     8    |\n");
				printf("+----------|--------|----------+\n");
				printf("|   3号    | 清潭湖 |     10   |\n");
				printf("+----------|--------|----------+\n");
				printf("|   4号    | 黄家湖 |     6    |\n");
				printf("+----------|--------|----------+\n");
				printf("|   5号    | 杨春湖 |     18   |\n");
				printf("+----------|--------|----------+\n");
				printf("|   6号    | 中山湖 |    150   |\n");
				printf("+----------|--------|----------+\n");
				message.setCurStatus(35);
				break;
			}
			case 34: {
				message.setCurStatus(0);
				break;
			}
			case 35: {
				printf("输入你的选择：");
				int option;
				while (std::cin >> option) {
					if (option >= 1 && option <= 4)	break;
					printf("输入有误!请重新输入:");
				}
				message.setCurStatus(30 + option);
				break;
			}
		}
	}
	close(correspondFd);
}

void printTitle(std::string title, int width) {
	for (int i = 0; i < (width - int(title.size())) / 2; i++) {
		if (i == 0)	std::cout << "+";
		else	std::cout << "-";
	}
	std::cout << title;
	for (int i = 0; i < width - int(title.size()) - (width - int(title.size())) / 2; i++) {
		if (i == width - int(title.size()) - (width - int(title.size())) / 2 - 1)	std::cout << "+";
		else	std::cout << "-";
	}
	std::cout << std::endl;
}

void printSegmentationLine(int len[], int n) {
	for (int i = 0; i < n; i++) {
		if(i == 0)	std::cout << "+-";
		else std::cout << "-+-";
		for (int j = 0; j < len[i]; j++)	std::cout << '-';
	}
	std::cout << "-+" << std::endl;
}

void printContent(std::string content[], int len[], int n) { 
	for (int k = 0; k < n; k++) {
		if(k == 0)	std::cout << "| ";
		else	std::cout << " | ";
		int t = content[k].size();
		for (int i = 0; i < (len[k] - t) / 2; i++)	std::cout << ' ';
		std::cout << content[k];
		for (int i = 0; i < len[k] - t - (len[k] - t) / 2; i++)	std::cout << ' ';
		if (k == n - 1)	std::cout << " |";
	}
	std::cout << std::endl;
}



void showTableData(Message& message, std::string title, std::string type) {
	if (type == "person") {
		std::string content[4];
		content[0] = "JID", content[1] = "Name", content[2] = "User", content[3] = "Password";
		int len[4];
		for (int i = 0; i < 4; i++)	len[i] = content[i].size();
		for (int i = 1; i <= message.getMemberCount(); i++) {
			auto item = message.getMember(i);
			len[0] = len[0] < strlen(item.jid) ? strlen(item.jid) : len[0];
			len[1] = len[1] < strlen(item.name) ? strlen(item.name) : len[1];
			len[2] = len[2] < strlen(item.user) ? strlen(item.user) : len[2];
			len[3] = len[3] < strlen(item.password) ? strlen(item.password) : len[3];
		}
		int width = 13;
		for (int i = 0; i < 4; i++)	width += len[i];
		printTitle(title, width);
		printContent(content, len, 4);
		for (int i = 1; i <= message.getMemberCount(); i++) {
			printSegmentationLine(len, 4);
			auto item = message.getMember(i);
			content[0] = item.jid, content[1] = item.name, content[2] = item.user, content[3] = item.password;
			printContent(content, len, 4);
		}
		for (int i = 0; i < width; i++) {
			if (i == 0 || i == width - 1)	std::cout << "+";
			else	std::cout << "-";
		}
		std::cout << std::endl;
	}
	else {
		std::string content[8];
		content[0] = "ID", content[1] = "JID", content[2] = "Name", content[3] = "User", content[4] = "Date", content[5] = "Period", content[6] = "Room", content[7] = "Status";
		int len[8];
		for (int i = 0; i < 8; i++)	len[i] = content[i].size();
		len[6] = len[7] = 6;
		for (int i = 1; i <= message.getRecordCount(); i++) {
			auto item = message.getRecord(i);
			len[1] = len[1] < strlen(item.jid) ? strlen(item.jid) : len[1];
			len[2] = len[2] < strlen(item.name) ? strlen(item.name) : len[2];
			len[3] = len[3] < strlen(item.user) ? strlen(item.user) : len[3];
			len[4] = len[4] < strlen(item.data) ? strlen(item.data) : len[4];
			len[5] = len[5] < strlen(item.period) ? strlen(item.period) : len[5];
		}
		int width = 25;
		for (int i = 0; i < 8; i++)	width += len[i];
		printTitle(title, width);
		printContent(content, len, 8);
		for (int i = 1; i <= message.getRecordCount(); i++) {
			printSegmentationLine(len, 8);
			auto item = message.getRecord(i);
			content[0] = std::to_string(i);
			content[1] = item.jid;
			content[2] = item.name;
			content[3] = item.user;
			content[4] = item.data;
			content[5] = item.period;
			if (item.roomID == 1)	content[6] = "紫阳湖";
			else if (item.roomID == 2)	content[6] = "后官湖";
			else if (item.roomID == 3)	content[6] = "清潭湖";
			else if (item.roomID == 4)	content[6] = "黄家湖";
			else if (item.roomID == 5)	content[6] = "杨春湖";
			else	content[6] = "中山湖";

			if (item.status == 0)	content[7] = "审核中";
			else if (item.status == 1)	content[7] = "已通过";
			else	content[7] = "已拒绝";
			printContent(content, len, 8);
		}
		for (int i = 0; i < width; i++) {
			if (i == 0 || i == width - 1)	std::cout << "+";
			else	std::cout << "-";
		}
		std::cout << std::endl;
	}
}