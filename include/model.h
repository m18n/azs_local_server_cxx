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
struct tovar{
    int id_tovar=0;
    float price=0.0;
    std::string name;
};
struct tank{
    int id_tank=0;
    tovar tovar_;
};
struct pist{
    int id_pist=0;
    tank tank_;
};
struct pump{
    int id_trk;
    int x_pos=0;
    int y_pos=0;
    float scale=100;
    std::vector<pist>pists;
};
class azs_database {
private:
    sql::Driver* driver;
    sql::Connection* con = NULL;
    sql::Statement* stmt;
    sql::ResultSet* res;
    bool isconn=false;
    std::string azs_id;
    mysql_conn_info last_info;
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
    mysql_conn_info get_last_info(){
        return last_info;
    }
    bool connect(mysql_conn_info info)
    {
        last_info=info;
        try {
            con = driver->connect("tcp://" + info.ip + ":"+info.port, info.name, info.password);
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
    void save_pump_xy(int32_t id,int32_t x,int32_t y);
    ~azs_database()
    {
        if (con != NULL && con->isValid())
            delete con;
    }
};
}