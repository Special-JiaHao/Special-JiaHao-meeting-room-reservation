#include "database.h"

Database::Database() {
	m_pMysql = nullptr;
}

Database::Database(char* HOST, char* USER, char* PASSWORD, char* DATABASE, int PORT) {
	connectMysql(HOST, USER, PASSWORD, DATABASE, PORT);
}

Database::~Database() {
	if(m_pMysql != nullptr)	mysql_close(m_pMysql);
}

void Database::connectMysql(const char host[], const char user[], const char password[], const char database[], int port) {
	m_pMysql = mysql_init(nullptr);
	mysql_options(m_pMysql, MYSQL_SET_CHARSET_NAME, "GBK");
	if (!mysql_real_connect(m_pMysql, host, user, password, database, port, nullptr, 0)) {
		fprintf(stderr, "Failed to connect to database : %s\n", mysql_error(m_pMysql));
	}
}

bool Database::testMysqlConfigure(const char host[], const char user[], const char password[], const char database[], int port) {

	connectMysql(host, user, password, database, port);
	if (m_pMysql == nullptr) {
		printf("请重新配置Mysql参数!");
		return false;
	}
	mysql_close(m_pMysql);
	m_pMysql = nullptr;
	return true;
}


bool Database::insertAccount(std::string& table, Account& account) {
	char sql[512];
	sprintf(sql, "insert into %s (jid, name, user, password) values('%s', '%s', '%s', '%s')", table.c_str(), account.jID.c_str(), account.name.c_str(), account.user.c_str(), account.password.c_str());
	if (mysql_query(m_pMysql, sql)) {
		fprintf(stderr, "Failed to insert --- Error:%s\n", mysql_error(m_pMysql));
		return false;
	}
	return true;
}

bool Database::insertAccount(std::string& table, char jID[], char name[], char user[], char password[]) {
	char sql[512];
	sprintf(sql, "insert into %s (jid, name, user, password) values('%s', '%s', '%s', '%s')", table.c_str(), jID, name, user, password);
	if (mysql_query(m_pMysql, sql)) {
		fprintf(stderr, "Failed to insert --- Error:%s\n", mysql_error(m_pMysql));
		return false;
	}
	return true;
}

bool Database::insertAccount(std::string& table, Member member) {
	char sql[512];
	sprintf(sql, "insert into %s (jid, name, user, password) values('%s', '%s', '%s', '%s')", table.c_str(), member.jid, member.name, member.user, member.password);
	if (mysql_query(m_pMysql, sql)) {
		fprintf(stderr, "Failed to insert --- Error:%s\n", mysql_error(m_pMysql));
		return false;
	}
	return true;
}

bool Database::insertRecord(char jID[], char name[], char user[], char data[], char period[], int roomID, int status) {
	char sql[512];
	sprintf(sql, "insert into records (jid, name, user, data, period, status, room_id) values('%s', '%s', '%s', '%s', '%s', %d, %d)", jID, name, user, data, period, status, roomID);
	if (mysql_query(m_pMysql, sql)) {
		fprintf(stderr, "Failed to insert --- Error:%s\n", mysql_error(m_pMysql));
		return false;
	}
	return true;
}

bool Database::insertRecord(Record record) {
	char sql[512];
	sprintf(sql, "insert into records (jid, name, user, data, period, status, room_id) values('%s', '%s', '%s', '%s', '%s', %d, %d)", record.jid, record.name, record.user, record.data, record.period, record.status, record.roomID);
	if (mysql_query(m_pMysql, sql)) {
		fprintf(stderr, "Failed to insert --- Error:%s\n", mysql_error(m_pMysql));
		return false;
	}
	return true;
}

bool Database::deleteRecord(int recordID) {
	char sql[512];
	sprintf(sql, "delete from records where rid=%d", recordID);
	if (mysql_query(m_pMysql, sql)) {
		fprintf(stderr, "Failed to delete --- Error:%s\n", mysql_error(m_pMysql));
		return false;
	}
	return true;
}

bool Database::deleteAllRecord() {
	char sql[512];
	sprintf(sql, "delete from records");
	if (mysql_query(m_pMysql, sql)) {
		fprintf(stderr, "Failed to delete --- Error:%s\n", mysql_error(m_pMysql));
		return false;
	}
	return true;
}

bool Database::updateRecordStatus(int recordID, int status) {
	char sql[512];
	sprintf(sql, "update records set status=%d where rid=%d", status, recordID);
	if (mysql_query(m_pMysql, sql)) {
		fprintf(stderr, "Failed to update --- Error:%s\n", mysql_error(m_pMysql));
		return false;
	}
	return true;
}

std::vector<Account> Database::queryAccount(std::string& table, std::string condition) {
	char sql[512];
	if (condition == "")	sprintf(sql, "SELECT * FROM %s", table.c_str());
	else	sprintf(sql, "SELECT * FROM %s WHERE %s", table.c_str(), condition.c_str());
	std::vector<Account> account;
	if (mysql_query(m_pMysql, sql)) {
		fprintf(stderr, "Failed to query --- Error:%s\n", mysql_error(m_pMysql));
		return account;
	}
	MYSQL_RES* res = mysql_store_result(m_pMysql);
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


std::vector<Record> Database::queryRecord(std::string condition) {
	char sql[512];
	if (condition == "")	sprintf(sql, "SELECT * FROM records");
	else	sprintf(sql, "SELECT * FROM records WHERE %s", condition.c_str());
	std::vector<Record> record;
	if (mysql_query(m_pMysql, sql)) {
		fprintf(stderr, "Failed to query --- Error:%s\n", mysql_error(m_pMysql));
		return record;
	}
	MYSQL_RES* res = mysql_store_result(m_pMysql);
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
