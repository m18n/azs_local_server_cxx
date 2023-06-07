#pragma once
#include"core.h"
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
    void show(){
        std::cout<<"PIST: "<<id_pist<<" TANK_ID: "<<tank_.id_tank<<" TOVAR_ID: "<<tank_.tovar_.id_tovar<<" TOVAR_PRICE: "<<tank_.tovar_.price<<" TOVAR_NAME: "<<tank_.tovar_.name<<"\n";
    }
};
struct pump{
    int id_trk;
    int x_pos=0;
    int y_pos=0;
    float scale=100;
    std::vector<pist>pists;
    void show(){
        std::cout<<"TRK_ID: "<<id_trk<<" X_POS: "<<x_pos<<" Y_POS: "<<y_pos<<" SCALE: "<<scale<<"\n";
        for(int i=0;i<pists.size();i++){
            pists[i].show();
        }
    }
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
    bool smena_bool();
    bool smena_bool(int32_t* last_id);
    void smena_change_operator(int32_t id_operator,int32_t id_smena);
    void smena_add_operator(int32_t id_operator, int32_t id_last_smena);
    ~azs_database()
    {
        if (con != NULL && con->isValid())
            delete con;
    }
};
}