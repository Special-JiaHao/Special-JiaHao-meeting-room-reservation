#ifndef CLASS_MESSAGE
#define CLASS_MESSAGE
#include <iostream>
struct Member {
	char jid[50];
	char name[50];
	char user[50];
	char password[50];
};
struct Account {
	std::string jID;
	std::string name;
	std::string user;
	std::string password;
};
struct Record {
	int recordID;
	char jid[50];
	char name[50];
	char user[50];
	char data[50];
	char period[50];
	int status;
	int roomID;
};

/* 
	用于通信的消息体：所以成员均采用定长类型
*/
class Message {
public:
	Message();
	~Message();
	void setMessageType(bool type);
	void setstatusCode(int code);
	void setID(int id);
	void setJID(std::string&  jid);
	void setUser(std::string& user);
	void setName(std::string& name);
	void setPassword(std::string& password);
	void setCurStatus(int status);
	void pushMember(std::string& jid, std::string& name, std::string& user, std::string& password);
	void pushMember(std::string& user, std::string& password);
	void pushRecord(Record& record);
	void pushRecord(std::string& jid, std::string& name, std::string& user, std::string& data, std::string& period, int roomID, int status = 0);
	void pushRecord(int recordID);
	void pushRecord(int recordID, int status);
	void resetPipeline();
	void setFeedback(std::string feedback);
	int getCurStatus() const;
	int getID() const;
	std::string getJID();
	int getStatusCode() const;
	std::string getUser();
	std::string getPassword();
	int getMemberCount() const;
	std::string getFeedback();
	Member getMember(int index);
	int getRecordCount() const;
	Record getRecord(int index);
	std::string getName();

private:
	bool m_bType;	/* 消息体类别, false:请求体, true:响应体 */
	int m_nStatusCode;	/* 状态码 */
	char m_gFeedback[50];
	int m_nId;		/* 身份, 管理员3, 业务人员1, 行政人员2 */
	char m_gJid[50];
	char m_gName[50];
	char m_gUser[50];
	char m_gPassword[50];

	int m_nCurStatus;	/* 上一次状态(请求体) */
	int m_nReqStatus;	/* 请求状态(请求体) */

	Member member[200];
	int m_nMemberCount;

	Record record[200];
	int m_nRecordCount;
};
#endif