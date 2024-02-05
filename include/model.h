#pragma once
#include "core.h"
#include "local_data.h"
#include "mysql_connection.h"
#include <chrono>
#include <cmath>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <mutex>
#include <thread>
#include <vector>
#include <string.h>

// #include <windows.h>
namespace model {

struct user_name {
    int32_t id = 0;
    std::string name;
};
struct tovar {
    int32_t id_tovar = 0;
    float price = 0.0;
    std::string name;
};
struct color {
    int32_t r = 0;
    int32_t g = 0;
    int32_t b = 0;
};

struct tank {
    int32_t id_tank = 0;
    color rgb;
    tovar tovar_;
};
bool compareByid(const tank &a, const tank &b);

struct pist {
    int32_t id_pist = 0;
    tank tank_;
    void show()
    {
        std::cout << "PIST: " << id_pist << " TANK_ID: " << tank_.id_tank << " TOVAR_ID: " << tank_.tovar_.id_tovar << " TOVAR_PRICE: " << tank_.tovar_.price << " TOVAR_NAME: " << tank_.tovar_.name << "\n";
    }
};
struct pump {
    int id_trk;
    int x_pos = 0;
    int y_pos = 0;
    float scale = 100;
    std::vector<pist> pists;
    void show()
    {
        std::cout << "TRK_ID: " << id_trk << " X_POS: " << x_pos << " Y_POS: " << y_pos << " SCALE: " << scale << "\n";
        for (int i = 0; i < pists.size(); i++) {
            pists[i].show();
        }
    }
};
struct screen_size{
    int width=0;
    int height=0;
};
class azs_database {
private:
    sql::Driver* driver;
    sql::Connection* con = NULL;
    sql::Statement* stmt;
    sql::ResultSet* res;
    bool isconn = false;
    std::string azs_id;
    mysql_conn_info last_input_info;
    mysql_conn_info last_connect_info;
    bool while_conn = false;
    std::mutex connmutex;
    local_data* l_db;
private:
    void get_azs_id();
    

public:
    azs_database()
    {
        driver = get_driver_instance();
        // con = driver->connect("tcp://127.0.0.1:3306", "root", "root");
        // con->setSchema("test");
    }
    void initlocaldata(local_data* local){
        this->l_db=local;
    }
    bool isConnect()
    {
        return isconn;
    }
    void setfirst_infomysql(mysql_conn_info info){
        last_connect_info=info;
    }
    mysql_conn_info get_last_input_info()
    {
        return last_input_info;
    }
    mysql_conn_info get_last_connect_info()
    {
        return last_connect_info;
    }
    bool connect(mysql_conn_info info)
    {
        
        connmutex.lock();
        last_input_info = info;
            try {
                con = driver->connect("tcp://" + info.ip + ":" + info.port, info.name, info.password);
                con->setSchema(info.database);
                isconn = con->isValid();
                if(last_connect_info!=info){
                    l_db->generatejwt_secret(true);
                }
                l_db->set_mysql_conn_info(info);
                last_connect_info=info;
                get_azs_id();
            } catch (const sql::SQLException& error) {
                std::cout << "ERROR MYSQL: " << error.what() << "\n";
                isconn = false;
            }
         while_conn = false;
        connmutex.unlock();
        return isconn;
    }
    bool while_connect(){
        return this->while_conn;
    }
    void connect_async(mysql_conn_info info)
    {
        if (while_conn == false) {
            last_input_info = info;
            std::thread th(&azs_database::connect, this, info);
            th.detach();
            while_conn = true;
        }
    }
    bool auth_check(int32_t userid, std::string password, bool& admin);
    std::vector<user_name> get_user_name();
    std::vector<pump> get_pump(screen_size* screen);
    std::vector<pump> get_pump(screen_size* screen,std::vector<model::tank>& tanks);
    void save_pump(std::vector<pump> pumps,int screen_width,int screen_height);
    bool smena_bool();
    bool smena_bool(int32_t* userid);
    bool smena_bool(int32_t* last_id, int32_t* last_nn);
    void smena_change_operator(int32_t id_operator, int32_t id_smena);
    void smena_add_operator(int32_t id_operator, int32_t nn);
    void smena_close();
    ~azs_database()
    {
        if (con != NULL && con->isValid())
            delete con;
    }
};
}