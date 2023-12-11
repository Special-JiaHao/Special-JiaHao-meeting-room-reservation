#include "message.h"

Message::Message() {
	m_memberCount = 0;
	m_recordCount = 0;
}

Message::~Message() {

}

void Message::setMessageType(bool type) {
	this->type = type;
}

void Message::setstatusCode(int code) {
	this->statusCode = code;
}

void Message::setID(int id) {
	this->id = id;
}

void Message::setJID(std::string& jid) {
	for (int i = 0; i < jid.size(); i++)	this->JID[i] = jid[i];
	this->JID[jid.size()] = '\0';
}

void Message::setName(std::string& name) {
	for (int i = 0; i < name.size(); i++)	this->name[i] = name[i];
	this->name[name.size()] = '\0';
}

void Message::setUser(std::string& user) {
	for (int i = 0; i < user.size(); i++)	this->user[i] = user[i];
	this->user[user.size()] = '\0';
}

void Message::setPassword(std::string& password) {
	for (int i = 0; i < password.size(); i++)	this->password[i] = password[i];
	this->password[password.size()] = '\0';
}

void Message::setCurStatus(int status) {
	this->curStatus = status;
}

void Message::pushMember(std::string& jid, std::string& name, std::string& user, std::string& password) {
	this->m_memberCount++;
	for (int i = 0; i < jid.size(); i++)	this->member[m_memberCount].jid[i] = jid[i];
	this->member[m_memberCount].jid[jid.size()] = '\0';

	for (int i = 0; i < name.size(); i++)	this->member[m_memberCount].name[i] = name[i];
	this->member[m_memberCount].name[name.size()] = '\0';

	for (int i = 0; i < user.size(); i++)	this->member[m_memberCount].user[i] = user[i];
	this->member[m_memberCount].user[user.size()] = '\0';

	for (int i = 0; i < password.size(); i++)	this->member[m_memberCount].password[i] = password[i];
	this->member[m_memberCount].password[password.size()] = '\0';
}

void Message::pushMember(std::string& user, std::string& password) {
	this->m_memberCount++;
	for (int i = 0; i < user.size(); i++)	this->member[m_memberCount].user[i] = user[i];
	this->member[m_memberCount].user[user.size()] = '\0';

	for (int i = 0; i < password.size(); i++)	this->member[m_memberCount].password[i] = password[i];
	this->member[m_memberCount].password[password.size()] = '\0';
}

void Message::pushRecord(Record& record) {
	this->m_recordCount++;
	this->record[m_recordCount] = record;
}

void Message::pushRecord(std::string& jid, std::string& name, std::string& user, std::string& data, std::string& period, int roomID, int status) {
	this->m_recordCount++;
	for (int i = 0; i < jid.size(); i++)	this->record[m_recordCount].jid[i] = jid[i];
	this->record[m_recordCount].jid[jid.size()] = '\0';

	for (int i = 0; i < name.size(); i++)	this->record[m_recordCount].name[i] = name[i];
	this->record[m_recordCount].name[name.size()] = '\0';

	for (int i = 0; i < user.size(); i++)	this->record[m_recordCount].user[i] = user[i];
	this->record[m_recordCount].user[user.size()] = '\0';

	for (int i = 0; i < data.size(); i++)	this->record[m_recordCount].data[i] = data[i];
	this->record[m_recordCount].data[data.size()] = '\0';

	for (int i = 0; i < period.size(); i++)	this->record[m_recordCount].period[i] = period[i];
	this->record[m_recordCount].period[period.size()] = '\0';

	this->record[m_recordCount].roomID = roomID;
	this->record[m_recordCount].status = status;
}

void Message::pushRecord(int recordID) {
	this->m_recordCount++;
	this->record[m_recordCount].recordID = recordID;
}

void Message::pushRecord(int recordID, int status) {
	this->m_recordCount++;
	this->record[m_recordCount].recordID = recordID;
	this->record[m_recordCount].status = status;
}

void Message::setFeedback(std::string feedback) {
	for (int i = 0; i < feedback.size(); i++)	this->feedback[i] = feedback[i];
	this->feedback[feedback.size()] = '\0';
}

void Message::resetPipeline() {
	this->m_memberCount = 0;
	this->m_recordCount = 0;
}

int Message::getCurStatus() const {
	return this->curStatus;
}

int Message::getID() const {
	return this->id;
}

std::string Message::getUser() {
	std::string res = "";
	for (int i = 0; user[i] != '\0'; i++)	res += user[i];
	return res;
}

std::string Message::getPassword() {
	std::string res = "";
	for (int i = 0; password[i] != '\0'; i++)	res += password[i];
	return res;
}

int Message::getStatusCode() const {
	return this->statusCode;
}

int Message::getMemberCount() const {
	return this->m_memberCount;
}

int Message::getRecordCount() const {
	return this->m_recordCount;
}

std::string Message::getFeedback() {
	std::string res = "";
	for (int i = 0; this->feedback[i] != '\0'; i++)	res += this->feedback[i];
	return res;
}

Member Message::getMember(int index) {
	return this->member[index];
}

Record Message::getRecord(int index) {
	return this->record[index];
}

std::string Message::getJID() {
	std::string res = "";
	for (int i = 0; JID[i] != '\0'; i++)	res += JID[i];
	return res;
}

std::string Message::getName() {
	std::string res = "";
	for (int i = 0; name[i] != '\0'; i++)	res += name[i];
	return res;
}