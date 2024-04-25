#pragma once

class DB
{
public:
	DB();
	~DB();

	void DBConnect();
	void CreateTable();
	int UserLoginCheck(const std::string& name, const std::string& pw);
	int UserTableAdd(const std::string& name, const std::string& pw);

private:
	void DBDisConnect();

private:
	sql::Driver* driver = nullptr;
	sql::Connection* con = nullptr;
};

