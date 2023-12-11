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

void showTableData(Message& message, std::string title, std::string type) {
	if (type == "person") {
		int len0 = 3, len1 = 4, len2 = 4, len3 = 8;
		for (int i = 1; i <= message.getMemberCount(); i++) {
			auto item = message.getMember(i);
			len0 = len0 < strlen(item.jid) ? strlen(item.jid) : len0;
			len1 = len1 < strlen(item.name) ? strlen(item.name) : len1;
			len2 = len2 < strlen(item.user) ? strlen(item.user) : len2;
			len3 = len3 < strlen(item.password) ? strlen(item.password) : len3;
		}
		int width = len0 + len1 + len2 + len3 + 13;
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

		std::cout << "| ";
		for (int i = 0; i < (len0 - 3) / 2; i++)	std::cout << ' ';
		std::cout << "JID";
		for (int i = 0; i < len0 - 3 - (len0 - 3) / 2; i++)	std::cout << ' ';
		std::cout << " | ";

		for (int i = 0; i < (len1 - 4) / 2; i++)	std::cout << ' ';
		std::cout << "Name";
		for (int i = 0; i < len1 - 4 - (len1 - 4) / 2; i++)	std::cout << ' ';
		std::cout << " | ";

		for (int i = 0; i < (len2 - 4) / 2; i++)	std::cout << ' ';
		std::cout << "User";
		for (int i = 0; i < len2 - 4 - (len2 - 4) / 2; i++)	std::cout << ' ';
		std::cout << " | ";

		for (int i = 0; i < (len3 - 8) / 2; i++)	std::cout << ' ';
		std::cout << "Password";
		for (int i = 0; i < len3 - 8 - (len3 - 8) / 2; i++)	std::cout << ' ';
		std::cout << " | " << std::endl;


		for (int i = 1; i <= message.getMemberCount(); i++) {
			auto item = message.getMember(i);
			std::cout << "+-";
			for (int j = 0; j < len0; j++)	std::cout << '-';
			std::cout << "-+-";
			for (int j = 0; j < len1; j++)	std::cout << '-';
			std::cout << "-+-";
			for (int j = 0; j < len2; j++)	std::cout << '-';
			std::cout << "-+-";
			for (int j = 0; j < len3; j++)	std::cout << '-';
			std::cout << "-+" << std::endl;

			int t1 = strlen(item.jid), t2 = strlen(item.name), t3 = strlen(item.user), t4 = strlen(item.password);


			std::cout << "| ";
			for (int i = 0; i < (len0 - t1) / 2; i++)	std::cout << ' ';
			std::cout << item.jid;
			for (int i = 0; i < len0 - t1 - (len0 - t1) / 2; i++)	std::cout << ' ';
			std::cout << " | ";

			for (int i = 0; i < (len1 - t2) / 2; i++)	std::cout << ' ';
			std::cout << item.name;
			for (int i = 0; i < len1 - t2 - (len1 - t2) / 2; i++)	std::cout << ' ';
			std::cout << " | ";

			for (int i = 0; i < (len2 - t3) / 2; i++)	std::cout << ' ';
			std::cout << item.user;
			for (int i = 0; i < len2 - t3 - (len2 - t3) / 2; i++)	std::cout << ' ';
			std::cout << " | ";

			for (int i = 0; i < (len3 - t4) / 2; i++)	std::cout << ' ';
			std::cout << item.password;
			for (int i = 0; i < len3 - t4 - (len3 - t4) / 2; i++)	std::cout << ' ';
			std::cout << " | " << std::endl;
			//std::cout << "| " << item.jid << " | " << item.name << " | " << item.user << " | " << item.password << " |" << std::endl;
		}
		for (int i = 0; i < width; i++) {
			if (i == 0 || i == width - 1)	std::cout << "+";
			else	std::cout << "-";
		}
		std::cout << std::endl;
	}
	else {
		int len0 = 2, len1 = 3, len2 = 4, len3 = 4, len4 = 4, len5 = 6, len6 = 6, len7 = 6;
		for (int i = 1; i <= message.getRecordCount(); i++) {
			auto item = message.getRecord(i);
			len1 = len1 < strlen(item.jid) ? strlen(item.jid) : len1;
			len2 = len2 < strlen(item.name) ? strlen(item.name) : len2;
			len3 = len3 < strlen(item.user) ? strlen(item.user) : len3;
			len4 = len4 < strlen(item.data) ? strlen(item.data) : len4;
			len5 = len5 < strlen(item.period) ? strlen(item.period) : len5;
		}
		int width = len0 + len1 + len2 + len3 + len4 + len5 + len6 + len7 + 25;
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

		std::cout << "| ";
		for (int i = 0; i < (len0 - 2) / 2; i++)	std::cout << ' ';
		std::cout << "ID";
		for (int i = 0; i < len0 - 2 - (len0 - 3) / 2; i++)	std::cout << ' ';
		std::cout << " | ";

		for (int i = 0; i < (len1 - 3) / 2; i++)	std::cout << ' ';
		std::cout << "JID";
		for (int i = 0; i < len1 - 3 - (len1 - 3) / 2; i++)	std::cout << ' ';
		std::cout << " | ";

		for (int i = 0; i < (len2 - 4) / 2; i++)	std::cout << ' ';
		std::cout << "Name";
		for (int i = 0; i < len2 - 4 - (len2 - 4) / 2; i++)	std::cout << ' ';
		std::cout << " | ";

		for (int i = 0; i < (len3 - 4) / 2; i++)	std::cout << ' ';
		std::cout << "User";
		for (int i = 0; i < len3 - 4 - (len3 - 4) / 2; i++)	std::cout << ' ';
		std::cout << " | ";

		for (int i = 0; i < (len4 - 4) / 2; i++)	std::cout << ' ';
		std::cout << "Date";
		for (int i = 0; i < len4 - 4 - (len4 - 4) / 2; i++)	std::cout << ' ';
		std::cout << " | ";

		for (int i = 0; i < (len5 - 6) / 2; i++)	std::cout << ' ';
		std::cout << "Period";
		for (int i = 0; i < len5 - 6 - (len5 - 6) / 2; i++)	std::cout << ' ';
		std::cout << " | ";

		for (int i = 0; i < (len7 - 4) / 2; i++)	std::cout << ' ';
		std::cout << "Room";
		for (int i = 0; i < len7 - 4 - (len7 - 4) / 2; i++)	std::cout << ' ';
		std::cout << " | ";

		for (int i = 0; i < (len6 - 6) / 2; i++)	std::cout << ' ';
		std::cout << "Status";
		for (int i = 0; i < len6 - 6 - (len6 - 6) / 2; i++)	std::cout << ' ';
		std::cout << " | " << std::endl;


		for (int i = 1; i <= message.getRecordCount(); i++) {
			auto item = message.getRecord(i);
			std::cout << "+-";
			for (int j = 0; j < len0; j++)	std::cout << '-';
			std::cout << "-+-";
			for (int j = 0; j < len1; j++)	std::cout << '-';
			std::cout << "-+-";
			for (int j = 0; j < len2; j++)	std::cout << '-';
			std::cout << "-+-";
			for (int j = 0; j < len3; j++)	std::cout << '-';
			std::cout << "-+-";
			for (int j = 0; j < len4; j++)	std::cout << '-';
			std::cout << "-+-";
			for (int j = 0; j < len5; j++)	std::cout << '-';
			std::cout << "-+-";
			for (int j = 0; j < len7; j++)	std::cout << '-';
			std::cout << "-+-";
			for (int j = 0; j < len6; j++)	std::cout << '-';
			std::cout << "-+" << std::endl;

			int t0 = 1, t1 = strlen(item.jid), t2 = strlen(item.name), t3 = strlen(item.user), t4 = strlen(item.data), t5 = strlen(item.period), t6 = 6, t7 = 6;

			std::cout << "| ";
			for (int i = 0; i < (len0 - t0) / 2; i++)	std::cout << ' ';
			std::cout << i;
			for (int i = 0; i < len0 - t0 - (len0 - t0) / 2; i++)	std::cout << ' ';
			std::cout << " | ";

			for (int i = 0; i < (len1 - t1) / 2; i++)	std::cout << ' ';
			std::cout << item.jid;
			for (int i = 0; i < len1 - t1 - (len1 - t1) / 2; i++)	std::cout << ' ';
			std::cout << " | ";

			for (int i = 0; i < (len2 - t2) / 2; i++)	std::cout << ' ';
			std::cout << item.name;
			for (int i = 0; i < len2 - t2 - (len2 - t2) / 2; i++)	std::cout << ' ';
			std::cout << " | ";

			for (int i = 0; i < (len3 - t3) / 2; i++)	std::cout << ' ';
			std::cout << item.user;
			for (int i = 0; i < len3 - t3 - (len3 - t3) / 2; i++)	std::cout << ' ';
			std::cout << " | ";

			for (int i = 0; i < (len4 - t4) / 2; i++)	std::cout << ' ';
			std::cout << item.data;
			for (int i = 0; i < len4 - t4 - (len4 - t4) / 2; i++)	std::cout << ' ';
			std::cout << " | ";

			for (int i = 0; i < (len5 - t5) / 2; i++)	std::cout << ' ';
			std::cout << item.period;
			for (int i = 0; i < len5 - t5 - (len5 - t5) / 2; i++)	std::cout << ' ';
			std::cout << " | ";

			for (int i = 0; i < (len6 - t6) / 2; i++)	std::cout << ' ';
			if (item.roomID == 1)	std::cout << "紫阳湖";
			else if (item.roomID == 2)	std::cout << "后官湖";
			else if (item.roomID == 3)	std::cout << "清潭湖";
			else if (item.roomID == 4)	std::cout << "黄家湖";
			else if (item.roomID == 5)	std::cout << "杨春湖";
			else	std::cout << "中山湖";
			for (int i = 0; i < len6 - t6 - (len6 - t6) / 2; i++)	std::cout << ' ';
			std::cout << " | ";

			for (int i = 0; i < (len7 - t7) / 2; i++)	std::cout << ' ';
			if (item.status == 0)	std::cout << "审核中";
			else if (item.status == 1)	std::cout << "已通过";
			else	std::cout << "已拒绝";
			for (int i = 0; i < len7 - t7 - (len7 - t7) / 2; i++)	std::cout << ' ';
			std::cout << " | " << std::endl;
		}
		for (int i = 0; i < width; i++) {
			if (i == 0 || i == width - 1)	std::cout << "+";
			else	std::cout << "-";
		}
		std::cout << std::endl;
	}
}