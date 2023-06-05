#pragma once
#include "mysql_connection.h"
#include <chrono>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <vector>
#include"local_data.h"
namespace model{


struct user_name {
    int32_t id = 0;
    std::string name;
};
struct pump{
    std::string id_trk;
    int x_pos=0;
    int y_pos=0;
    float scale=100;
};
class azs_database {
private:
    sql::Driver* driver;
    sql::Connection* con = NULL;
    sql::Statement* stmt;
    sql::ResultSet* res;
    bool isconn=false;
    std::string azs_id;
private:
void get_azs_id();
public:
    azs_database()
    {
        driver = get_driver_instance();
        // con = driver->connect("tcp://127.0.0.1:3306", "root", "root");
        // con->setSchema("test");
    }
    bool isConnect(){
        return isconn;
    }
    bool connect(mysql_conn_info info)
    {
        try {
            con = driver->connect("tcp://" + info.ip + ":3306", info.name, info.password);
            con->setSchema(info.database);
            isconn=con->isValid();
            get_azs_id();
            return isconn;
        } catch (const sql::SQLException& error) {
            std::cout<<"ERROR MYSQL: "<<error.what()<<"\n";
            isconn=false;
            return isconn;
        }
    }
    
    bool auth_check(int32_t userid, std::string password, bool& admin);
    std::vector<user_name> get_user_name();
    std::vector<pump> get_pump();
    void save_pump_scale(int32_t id,float scale);
    ~azs_database()
    {
        if (con != NULL && con->isValid())
            delete con;
    }
};
}