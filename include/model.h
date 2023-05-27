#pragma once
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <vector>
struct user_name{
  int32_t id=0;
  std::string name;
};
class azs_database {
private:
    sql::Driver* driver;
    sql::Connection* con;
    sql::Statement* stmt;
    sql::ResultSet* res;

public:
    azs_database()
    {
        driver = get_driver_instance();
        // con = driver->connect("tcp://127.0.0.1:3306", "root", "root");
        // con->setSchema("test");
    }
    void connect(std::string ip, std::string name, std::string password)
    {
        con = driver->connect("tcp://" + ip + ":3306", name, password);
        con->setSchema("azs");
    }
    bool auth_check(int32_t userid, std::string password);
    std::vector<user_name> get_user_name();
    ~azs_database()
    {
        delete con;
    }
};