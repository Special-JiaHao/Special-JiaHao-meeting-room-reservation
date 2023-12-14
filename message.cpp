#include "message.h"

Message::Message() {
	m_nMemberCount = 0;
	m_nRecordCount = 0;
}

Message::~Message() {

}

void Message::setMessageType(bool type) {
	this->m_bType = type;
}

void Message::setstatusCode(int code) {
	this->m_nStatusCode = code;
}

void Message::setID(int id) {
	this->m_nId = id;
}

void Message::setJID(std::string& jid) {
	for (int i = 0; i < jid.size(); i++)	this->m_gJid[i] = jid[i];
	this->m_gJid[jid.size()] = '\0';
}

void Message::setName(std::string& name) {
	for (int i = 0; i < name.size(); i++)	this->m_gName[i] = name[i];
	this->m_gName[name.size()] = '\0';
}

void Message::setUser(std::string& user) {
	for (int i = 0; i < user.size(); i++)	this->m_gUser[i] = user[i];
	this->m_gUser[user.size()] = '\0';
}

void Message::setPassword(std::string& password) {
	for (int i = 0; i < password.size(); i++)	this->m_gPassword[i] = password[i];
	this->m_gPassword[password.size()] = '\0';
}

void Message::setCurStatus(int status) {
	this->m_nCurStatus = status;
}

void Message::pushMember(std::string& jid, std::string& name, std::string& user, std::string& password) {
	this->m_nMemberCount++;
	for (int i = 0; i < jid.size(); i++)	this->member[m_nMemberCount].jid[i] = jid[i];
	this->member[m_nMemberCount].jid[jid.size()] = '\0';

	for (int i = 0; i < name.size(); i++)	this->member[m_nMemberCount].name[i] = name[i];
	this->member[m_nMemberCount].name[name.size()] = '\0';

	for (int i = 0; i < user.size(); i++)	this->member[m_nMemberCount].user[i] = user[i];
	this->member[m_nMemberCount].user[user.size()] = '\0';

	for (int i = 0; i < password.size(); i++)	this->member[m_nMemberCount].password[i] = password[i];
	this->member[m_nMemberCount].password[password.size()] = '\0';
}

void Message::pushMember(std::string& user, std::string& password) {
	this->m_nMemberCount++;
	for (int i = 0; i < user.size(); i++)	this->member[m_nMemberCount].user[i] = user[i];
	this->member[m_nMemberCount].user[user.size()] = '\0';

	for (int i = 0; i < password.size(); i++)	this->member[m_nMemberCount].password[i] = password[i];
	this->member[m_nMemberCount].password[password.size()] = '\0';
}

void Message::pushRecord(Record& record) {
	this->m_nRecordCount++;
	this->record[m_nRecordCount] = record;
}

void Message::pushRecord(std::string& jid, std::string& name, std::string& user, std::string& data, std::string& period, int roomID, int status) {
	this->m_nRecordCount++;
	for (int i = 0; i < jid.size(); i++)	this->record[m_nRecordCount].jid[i] = jid[i];
	this->record[m_nRecordCount].jid[jid.size()] = '\0';

	for (int i = 0; i < name.size(); i++)	this->record[m_nRecordCount].name[i] = name[i];
	this->record[m_nRecordCount].name[name.size()] = '\0';

	for (int i = 0; i < user.size(); i++)	this->record[m_nRecordCount].user[i] = user[i];
	this->record[m_nRecordCount].user[user.size()] = '\0';

	for (int i = 0; i < data.size(); i++)	this->record[m_nRecordCount].data[i] = data[i];
	this->record[m_nRecordCount].data[data.size()] = '\0';

	for (int i = 0; i < period.size(); i++)	this->record[m_nRecordCount].period[i] = period[i];
	this->record[m_nRecordCount].period[period.size()] = '\0';

	this->record[m_nRecordCount].roomID = roomID;
	this->record[m_nRecordCount].status = status;
}

void Message::pushRecord(int recordID) {
	this->m_nRecordCount++;
	this->record[m_nRecordCount].recordID = recordID;
}

void Message::pushRecord(int recordID, int status) {
	this->m_nRecordCount++;
	this->record[m_nRecordCount].recordID = recordID;
	this->record[m_nRecordCount].status = status;
}

void Message::setFeedback(std::string feedback) {
	for (int i = 0; i < feedback.size(); i++)	this->m_gFeedback[i] = feedback[i];
	this->m_gFeedback[feedback.size()] = '\0';
}

void Message::resetPipeline() {
	this->m_nMemberCount = 0;
	this->m_nRecordCount = 0;
}

int Message::getCurStatus() const {
	return this->m_nCurStatus;
}

int Message::getID() const {
	return this->m_nId;
}

std::string Message::getUser() {
	std::string res = "";
	for (int i = 0; m_gUser[i] != '\0'; i++)	res += m_gUser[i];
	return res;
}

std::string Message::getPassword() {
	std::string res = "";
	for (int i = 0; m_gPassword[i] != '\0'; i++)	res += m_gPassword[i];
	return res;
}

int Message::getStatusCode() const {
	return this->m_nStatusCode;
}

int Message::getMemberCount() const {
	return this->m_nMemberCount;
}

int Message::getRecordCount() const {
	return this->m_nRecordCount;
}

std::string Message::getFeedback() {
	std::string res = "";
	for (int i = 0; this->m_gFeedback[i] != '\0'; i++)	res += this->m_gFeedback[i];
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
	for (int i = 0; m_gJid[i] != '\0'; i++)	res += m_gJid[i];
	return res;
}

std::string Message::getName() {
	std::string res = "";
	for (int i = 0; m_gName[i] != '\0'; i++)	res += m_gName[i];
	return res;
}