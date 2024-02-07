#include "model.h"

std::vector<model::user_name> model::azs_database::get_user_name()
{

    std::vector<model::user_name> users;
    try {
        stmt = con->createStatement();
        std::string sql = "SELECT * FROM loc_users INNER JOIN ref_users ON loc_users.id_user = ref_users.id_user WHERE loc_users.id_station='" + azs_id + "';";
        sql::ResultSet* res = stmt->executeQuery(sql);
        user_name u;
        while (res->next()) {
            u.id = res->getInt("id_user");
            u.name = res->getString("user");
            users.push_back(u);
        }
        delete res;
        delete stmt;
    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
    }
    return users;
}

void model::azs_database::save_pump(std::vector<pump> pumps,int screen_width,int screen_height){
     try {
            stmt = con->createStatement();
            std::string sql = "UPDATE loc_const SET value=\""+std::to_string(screen_width)+","+std::to_string(screen_height)+"\" WHERE descr_var=\"cnst_ScreenSize\";";
            int t = stmt->executeUpdate(sql);

            delete stmt;
        } catch (const sql::SQLException& error) {
            std::cout << "ERROR MYSQL: " << error.what() << "\n";
            isconn = false;
        }
    for(int i=0;i<pumps.size();i++){
        try {
            stmt = con->createStatement();
            std::string sql = "UPDATE com_trk SET scale="+ std::to_string(pumps[i].scale)+", x_pos=" + std::to_string(pumps[i].x_pos) + ", y_pos=" + std::to_string(pumps[i].y_pos) + " WHERE id_trk=" + std::to_string(pumps[i].id_trk) + ";";
            int t = stmt->executeUpdate(sql);

            delete stmt;
        } catch (const sql::SQLException& error) {
            std::cout << "ERROR MYSQL: " << error.what() << "\n";
            isconn = false;
        }
    }
}
bool model::azs_database::smena_bool(int32_t* last_id, int32_t* last_nn)
{
    bool status = false;
    try {
        stmt = con->createStatement();
        std::string sql = "SELECT * FROM smena WHERE id_azs='" + azs_id + "' ORDER BY NSmen DESC LIMIT 1;";
        sql::ResultSet* res = stmt->executeQuery(sql);
        sql::ResultSetMetaData* meta = res->getMetaData();
        while (res->next()) {
            std::cout << "COLUM:\n";
            for (int i = 1; i <= meta->getColumnCount(); i++) {
                std::cout << "NAME:" << meta->getColumnName(i) << "\n";
            }

            std::cout << "\n";
            *last_id = res->getInt("NSmen");
            *last_nn = res->getInt("NN");
            status = res->getInt("status");
        }
        delete res;
        delete stmt;
    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
        return false;
    }
    return status;
}
bool model::azs_database::smena_bool()
{
    bool status = false;
    try {
        stmt = con->createStatement();
        std::string sql = "SELECT * FROM smena WHERE id_azs='" + azs_id + "' ORDER BY NSmen DESC LIMIT 1;";
        sql::ResultSet* res = stmt->executeQuery(sql);
        sql::ResultSetMetaData* meta = res->getMetaData();

        while (res->next()) {

            std::cout << "NN:" << res->getInt("NSmen");
            status = res->getInt("status");
        }
        delete res;
        delete stmt;
    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
        return false;
    }
    return status;
}
bool model::azs_database::smena_bool(int32_t* userid)
{
    bool status = false;
    try {
        stmt = con->createStatement();
        std::string sql = "SELECT * FROM smena WHERE id_azs='" + azs_id + "' ORDER BY NSmen DESC LIMIT 1;";
        sql::ResultSet* res = stmt->executeQuery(sql);
        sql::ResultSetMetaData* meta = res->getMetaData();

        while (res->next()) {

            std::cout << "NN:" << res->getInt("NSmen");
            *userid = res->getInt("id_operator");
            status = res->getInt("status");
        }
        delete res;
        delete stmt;
    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
        return false;
    }
    return status;
}
void model::azs_database::smena_change_operator(int32_t id_operator, int32_t id_smena)
{
    try {
        stmt = con->createStatement();
        std::string sql = "UPDATE smena SET id_operator=" + std::to_string(id_operator) + " WHERE NSmen=" + std::to_string(id_smena) + ";";
        int t = stmt->executeUpdate(sql);
        delete stmt;
    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
    }
}
void model::azs_database::smena_close(){
    try {
        int32_t last_id=-1;
        int32_t nn=-1;
        bool b=smena_bool(&last_id,&nn);
        std::string time_now = timetostr(std::chrono::system_clock::now());
        stmt = con->createStatement();
        std::string sql = "UPDATE smena SET status=0, sm_end=\""+time_now+"\"  WHERE NSmen=" + std::to_string(last_id) + ";";
        int t = stmt->executeUpdate(sql);
        delete stmt;
    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
    }
}
void model::azs_database::smena_add_operator(int32_t id_operator, int32_t nn)
{
    try {
        stmt = con->createStatement();
        std::string time_now = timetostr(std::chrono::system_clock::now());
        nn++;
        std::string sql = "INSERT INTO smena (NN,id_azs,sm_start,sm_end,id_operator,status,id_ppo,znum) VALUES (" + std::to_string(nn) + "," + azs_id + ",'" + time_now + "','" + time_now + "'," + std::to_string(id_operator) + ",1,10,10);";
        std::cout << "SQL: " << sql << "\n";
        int t = stmt->executeUpdate(sql);
        std::cout << "INSERT OK\n";
        delete stmt;
    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
    }
}
std::vector<model::pump> model::azs_database::get_pump(screen_size* screen)
{
    std::vector<model::pump> pumps;
    try {
        stmt = con->createStatement();
        std::string sql ="SELECT * FROM loc_const WHERE descr_var=\"cnst_ScreenSize\";";
        sql::ResultSet* res = stmt->executeQuery(sql);
        while (res->next()) {
            std::string size=res->getString("value");
            char* sizes=strtok(&size[0],",");
            screen->width=std::stoi(sizes);
            sizes=strtok(NULL, " ");
            screen->height=std::stoi(sizes);
        }
        delete res;
        delete stmt; 
    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
    }
    try {
        stmt = con->createStatement();
        std::string sql = "SELECT com_trk.id_trk, com_trk.x_pos, com_trk.y_pos, com_trk.scale, trk.id_trk, trk.id_pist, trk.id_tank, tank.id_tovar,tank.color, tovar.name, tovar.price FROM com_trk INNER JOIN trk ON com_trk.id_trk = trk.id_trk INNER JOIN tank ON trk.id_tank = tank.id_tank INNER JOIN tovar ON tank.id_tovar = tovar.id_tovar WHERE com_trk.id_azs='" + azs_id + "' ORDER BY com_trk.id_trk;";
        sql::ResultSet* res = stmt->executeQuery(sql);
        sql::ResultSetMetaData* meta = res->getMetaData();

        pump p_stat;
        pist pist_stat;
        int last_idtkr = -1;
        while (res->next()) {
            // std::cout << "COLUM:\n";
            // for (int i = 1; i <= meta->getColumnCount(); i++) {
            //     std::cout << "NAME:" << meta->getColumnName(i) << " VALUE: " << res->getString(i) << "\n";
            // }

            // std::cout << "\n";
            pist_stat.id_pist = res->getInt("id_pist");
            pist_stat.tank_.id_tank = res->getInt("id_tank");
            pist_stat.tank_.tovar_.id_tovar = res->getInt("id_tovar");
            pist_stat.tank_.tovar_.name = res->getString("name");
            pist_stat.tank_.tovar_.price =res->getDouble("price");
            int32_t color=res->getInt("color");
            unsigned int red = (color >> 16) & 0xFF;
            unsigned int green = (color >> 8) & 0xFF;
            unsigned int blue = color & 0xFF;
            pist_stat.tank_.rgb.r=red;
            pist_stat.tank_.rgb.g=green;
            pist_stat.tank_.rgb.b=blue;
            if (res->getInt("id_trk") == last_idtkr) {
                pumps[pumps.size() - 1].pists.push_back(pist_stat);
                continue;
            }
            p_stat.pists.push_back(pist_stat);
            p_stat.id_trk = res->getInt("id_trk");
            p_stat.x_pos = res->getInt("x_pos");
            p_stat.y_pos = res->getInt("y_pos");
            p_stat.scale = res->getDouble("scale");

            last_idtkr = p_stat.id_trk;
            pumps.push_back(p_stat);
            p_stat.pists.resize(0);
        }
        delete res;
        delete stmt; 
        sql = "SELECT * FROM trk WHERE id_azs='" + azs_id + "';";

    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
    }

    return pumps;
}
bool  model::compareByid(const  model::tank &a, const  model::tank &b) {
    return a.id_tank < b.id_tank;
}
std::vector<model::pump> model::azs_database::get_pump(screen_size* screen,std::vector<model::tank>& tanks)
{
    std::vector<model::pump> pumps;
    try {
        stmt = con->createStatement();
        std::string sql ="SELECT * FROM loc_const WHERE descr_var=\"cnst_ScreenSize\";";
        sql::ResultSet* res = stmt->executeQuery(sql);
        while (res->next()) {
            std::string size=res->getString("value");
            char* sizes=strtok(&size[0],",");
            screen->width=std::stoi(sizes);
            sizes=strtok(NULL, " ");
            screen->height=std::stoi(sizes);
        }
        delete res;
        delete stmt; 
    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
    }
    try {
        stmt = con->createStatement();
        std::string sql = "SELECT com_trk.id_trk, com_trk.x_pos, com_trk.y_pos, com_trk.scale, trk.id_trk, trk.id_pist, trk.id_tank, tank.id_tovar,tank.color, tovar.name, tovar.price FROM com_trk INNER JOIN trk ON com_trk.id_trk = trk.id_trk INNER JOIN tank ON trk.id_tank = tank.id_tank INNER JOIN tovar ON tank.id_tovar = tovar.id_tovar WHERE com_trk.id_azs='" + azs_id + "' ORDER BY com_trk.id_trk;";
        sql::ResultSet* res = stmt->executeQuery(sql);
        sql::ResultSetMetaData* meta = res->getMetaData();

        pump p_stat;
        pist pist_stat;
        int last_idtkr = -1;
        while (res->next()) {
            // std::cout << "COLUM:\n";
            // for (int i = 1; i <= meta->getColumnCount(); i++) {
            //     std::cout << "NAME:" << meta->getColumnName(i) << " VALUE: " << res->getString(i) << "\n";
            // }

            // std::cout << "\n";
            pist_stat.id_pist = res->getInt("id_pist");
            
            pist_stat.tank_.id_tank = res->getInt("id_tank");
            pist_stat.tank_.tovar_.id_tovar = res->getInt("id_tovar");
            pist_stat.tank_.tovar_.name = res->getString("name");
            pist_stat.tank_.tovar_.price =res->getDouble("price");
            bool find=false;
            for(int i=0;i<tanks.size();i++){
                if(tanks[i].id_tank==pist_stat.tank_.id_tank){
                    find=true;
                    break;
                }
            }
           
            int32_t color=res->getInt("color");
            unsigned int red = (color >> 16) & 0xFF;
            unsigned int green = (color >> 8) & 0xFF;
            unsigned int blue = color & 0xFF;
            pist_stat.tank_.rgb.r=red;
            pist_stat.tank_.rgb.g=green;
            pist_stat.tank_.rgb.b=blue;
            if(find==false){
                model::tank t=pist_stat.tank_;
                tanks.push_back(t);
            }
            if (res->getInt("id_trk") == last_idtkr) {
                pumps[pumps.size() - 1].pists.push_back(pist_stat);
                continue;
            }

            p_stat.pists.push_back(pist_stat);
            p_stat.id_trk = res->getInt("id_trk");
            p_stat.x_pos = res->getInt("x_pos");
            p_stat.y_pos = res->getInt("y_pos");
            p_stat.scale = res->getDouble("scale");

            last_idtkr = p_stat.id_trk;
            pumps.push_back(p_stat);
            p_stat.pists.resize(0);
        }
        delete res;
        delete stmt; 
        sql = "SELECT * FROM trk WHERE id_azs='" + azs_id + "';";

    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
    }
    std::sort(tanks.begin(), tanks.end(), compareByid);
    return pumps;
}
void model::azs_database::get_azs_id()
{
    try {
        stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery("SELECT * FROM loc_const WHERE descr_var='cnst_ID_Station';");
        res->next();
        bool auth = false;
        azs_id = res->getString("value");

        delete res;
        delete stmt;

    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
    }
}
bool model::azs_database::auth_check(int32_t userid, std::string password, bool& admin)
{
    try {
        stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery("SELECT password FROM loc_users WHERE id_user=" + std::to_string(userid) + ";");
        res->next();
        bool auth = false;
        if (res->getString("password") == password) {
            auth = true;
            if (userid >= 1000000)
                admin = true;
        }

        delete res;
        delete stmt;
        return auth;
    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
        return false;
    }
}