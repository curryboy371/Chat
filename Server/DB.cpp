#include "pch.h"
#include "DB.h"

DB::DB()
	:driver(nullptr), con(nullptr)
{

}

DB::~DB()
{
	DBDisConnect();
}

void DB::DBConnect()
{
	driver = get_driver_instance();
	con = driver->connect("tcp://127.0.0.1:3306", "root", "chan371");
	con->setSchema("chat");
}

void DB::DBDisConnect()
{
	delete con;
}

int DB::UserTableAdd(const std::string& name, const std::string& pw)
{
	try
	{
		// Statement 객체 생성
		std::unique_ptr<sql::Statement> stmt(con->createStatement());

		std::string query = "INSERT INTO usertable (name, password) VALUES ('" + name + "', '" + pw + "')";

		stmt->execute(query.c_str());

		return 0;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException in insertUserData(). #ERR: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		return e.getErrorCode();
	}

	return -1;

}

void DB::CreateTable()
{
	try {
		// Table Create
		std::unique_ptr<sql::Statement> stmt(con->createStatement());
		stmt->execute("CREATE TABLE IF NOT EXISTS UserTable (id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(255));");

		// Pw Col Add
		stmt.reset(con->createStatement()); // Reuse the statement object
		stmt->execute("ALTER TABLE UserTable ADD COLUMN password VARCHAR(255);");

		// Add UNIQUE Constraint
		stmt.reset(con->createStatement()); // Reuse the statement object
		stmt->execute("ALTER TABLE UserTable ADD UNIQUE (name);");

		std::cout << "UserTable setup completed successfully." << std::endl;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException in setupUserTable(). #ERR: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}
}

int DB::UserLoginCheck(const std::string& name, const std::string& pw)
{
	try
	{
		// PreparedStatement 객체 생성
		std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("SELECT name, password FROM usertable WHERE name = ? AND password = ?"));

		// 바인딩
		pstmt->setString(1, name);
		pstmt->setString(2, pw);

		// 쿼리 실행
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

		// 결과 확인
		if (res->next()) 
		{
			return 0;
		}
		else 
		{
			return 10000; // 불일치
		}
	}
	catch (sql::SQLException& e) 
	{
		std::cerr << "SQLException in setupUserTable(). #ERR: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << endl;

		return e.getErrorCode();
	}

	return -1;
}


