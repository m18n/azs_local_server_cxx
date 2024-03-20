#pragma once
#include "crow.h"
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
#include <nlohmann/json.hpp>
#include <iomanip> // Для std::setprecision і std::fixed
#include <sstream> // Для std::stringstream
#include <string>  // Для std::string
#include <cstdint> // для uint32_t
// #include <windows.h>
#define GetRValue(rgb)      (LOBYTE(rgb))
#define GetGValue(rgb)      (LOBYTE(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)      (LOBYTE((rgb)>>16))
namespace model {
template<typename T>
class VectorWrapper {
public:
    VectorWrapper() = default;

    // Забороняємо копіювання для уникнення витоків пам'яті
    VectorWrapper(const VectorWrapper&) = delete;
    VectorWrapper& operator=(const VectorWrapper&) = delete;

    // Дозволяємо переміщення
    VectorWrapper(VectorWrapper&&) noexcept = default;
    VectorWrapper& operator=(VectorWrapper&&) noexcept = default;

    ~VectorWrapper() {
        for(int i=0;i<arr.size();i++){
            delete arr[i];
        }
        arr.clear();
    }
    std::vector<T*> arr;
    
};
struct User_Name {
    int32_t id = 0;
    std::string name;
};
struct Color {
    int32_t r = 0;
    int32_t g = 0;
    int32_t b = 0;
};
struct Tovar {
    int32_t id_tovar = -1;
    float price = -1;
    std::string name;
    std::string name_p;
    std::string name_p_f;
    std::string name_p_v;
    Color color;
};


uint32_t get_rgb(int r, int g, int b);
Tovar json_to_tovar(nlohmann::json json);

struct Tank {
    int32_t id_tank = -1;
    int32_t id_tovar=-1;
    int32_t volume=-1;
    int32_t remain=-1;
    Tovar* tovar_=NULL;
};
Tank json_to_tank(nlohmann::json json);
bool compareByid(const Tank &a, const Tank &b);

struct Pist {
    int32_t id_pist = -1;
    int32_t id_tank=-1;
    Tank* tank_=NULL;
    void show()
    {
        if(tank_!=NULL&&tank_->tovar_!=NULL)
            std::cout << "PIST: " << id_pist << " TANK_ID: " << tank_->id_tank << " TOVAR_ID: " << tank_->tovar_->id_tovar << " TOVAR_PRICE: " << tank_->tovar_->price << " TOVAR_NAME: " << tank_->tovar_->name << "\n";
    }
};
struct Trk {
    int id_trk=-1;
    int x_pos = 0;
    int y_pos = 0;
    float scale = 100;
    std::vector<Pist> pists;
    void show()
    {
        std::cout << "TRK_ID: " << id_trk << " X_POS: " << x_pos << " Y_POS: " << y_pos << " SCALE: " << scale << "\n";
        for (int i = 0; i < pists.size(); i++) {
            pists[i].show();
        }
    }
};
Trk json_to_trk(nlohmann::json json);

struct Screen_Size{
    int width=0;
    int height=0;
};


std::string floatToString(float number,int step);
class Azs_Database {
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
    Azs_Database()
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
            std::thread th(&Azs_Database::connect, this, info);
            th.detach();
            while_conn = true;
        }
    }
    bool executeSql(std::string sql);
    bool auth_check(int32_t userid, std::string password, bool& admin);
    std::vector<User_Name> get_Users_Name();
    //std::vector<Trk> get_Trks(Screen_Size* screen);
    std::vector<Trk> get_Trks_with_all(Screen_Size* screen,model::VectorWrapper<model::Tovar>* tovars,model::VectorWrapper<model::Tank>* tanks);
    std::vector<Tovar> get_Tovars();
    std::vector<Tank> get_Tanks();
    std::vector<Trk> get_Trks();
    void save_Trks(std::vector<Trk> trks,int screen_width,int screen_height);
    void set_Tovar(Tovar& tovar);
    void set_Trk(Trk& trk);
    void set_Tank(Tank& tank);
    void delete_Tovar(Tovar& tovar);
    void delete_Trk(Trk& trk,bool smart_delete);
    void delete_Tank(Tank& tank);
    bool smena_bool();
    bool smena_bool(int32_t* userid);
    bool smena_bool(int32_t* last_id, int32_t* last_nn);
    void smena_change_operator(int32_t id_operator, int32_t id_smena);
    void smena_add_operator(int32_t id_operator, int32_t nn);
    void smena_close();
    ~Azs_Database()
    {
        if (con != NULL && con->isValid())
            delete con;
    }
};
}