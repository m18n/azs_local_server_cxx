#include "model.h"

std::vector<model::User_Name> model::Azs_Database::get_Users_Name()
{

    std::vector<model::User_Name> users;
    try {
        stmt = con->createStatement();
        std::string sql = "SELECT * FROM loc_users INNER JOIN ref_users ON loc_users.id_user = ref_users.id_user WHERE loc_users.id_station='" + azs_id + "';";
        sql::ResultSet* res = stmt->executeQuery(sql);
        User_Name u;
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

void model::Azs_Database::save_Trks(std::vector<model::Trk> trks,int screen_width,int screen_height){
     try {
            stmt = con->createStatement();
            std::string sql = "UPDATE loc_const SET value=\""+std::to_string(screen_width)+","+std::to_string(screen_height)+"\" WHERE descr_var=\"cnst_ScreenSize\";";
            int t = stmt->executeUpdate(sql);

            delete stmt;
        } catch (const sql::SQLException& error) {
            std::cout << "ERROR MYSQL: " << error.what() << "\n";
            isconn = false;
        }
    for(int i=0;i<trks.size();i++){
        try {
            stmt = con->createStatement();
            std::string sql = "UPDATE com_trk SET scale="+ std::to_string(trks[i].scale)+", x_pos=" + std::to_string(trks[i].x_pos) + ", y_pos=" + std::to_string(trks[i].y_pos) + " WHERE id_trk=" + std::to_string(trks[i].id_trk) + ";";
            int t = stmt->executeUpdate(sql);

            delete stmt;
        } catch (const sql::SQLException& error) {
            std::cout << "ERROR MYSQL: " << error.what() << "\n";
            isconn = false;
        }
    }
}
bool model::Azs_Database::smena_bool(int32_t* last_id, int32_t* last_nn)
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
bool model::Azs_Database::smena_bool()
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
bool model::Azs_Database::smena_bool(int32_t* userid)
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
void model::Azs_Database::smena_change_operator(int32_t id_operator, int32_t id_smena)
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
void model::Azs_Database::smena_close(){
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
void model::Azs_Database::smena_add_operator(int32_t id_operator, int32_t nn)
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
// std::vector<model::Trk> model::Azs_Database::get_Trks(Screen_Size* screen)
// {
//     std::vector<model::Trk> Trks;
//     try {
//         stmt = con->createStatement();
//         std::string sql ="SELECT * FROM loc_const WHERE descr_var=\"cnst_ScreenSize\";";
//         sql::ResultSet* res = stmt->executeQuery(sql);
//         while (res->next()) {
//             std::string size=res->getString("value");
//             char* sizes=strtok(&size[0],",");
//             screen->width=std::stoi(sizes);
//             sizes=strtok(NULL, " ");
//             screen->height=std::stoi(sizes);
//         }
//         delete res;
//         delete stmt; 
//     } catch (const sql::SQLException& error) {
//         std::cout << "ERROR MYSQL: " << error.what() << "\n";
//         isconn = false;
//     }
//     try {
//         stmt = con->createStatement();
//         std::string sql = "SELECT com_trk.id_trk, com_trk.x_pos, com_trk.y_pos, com_trk.scale, trk.id_trk, trk.id_pist, trk.id_tank, tank.id_tovar,tank.color, tovar.name, tovar.price FROM com_trk INNER JOIN trk ON com_trk.id_trk = trk.id_trk INNER JOIN tank ON trk.id_tank = tank.id_tank INNER JOIN tovar ON tank.id_tovar = tovar.id_tovar WHERE com_trk.id_azs='" + azs_id + "' ORDER BY com_trk.id_trk;";
//         sql::ResultSet* res = stmt->executeQuery(sql);
//         sql::ResultSetMetaData* meta = res->getMetaData();

//         Trk p_stat;
//         Pist pist_stat;
//         int last_idtkr = -1;
//         while (res->next()) {
//             // std::cout << "COLUM:\n";
//             // for (int i = 1; i <= meta->getColumnCount(); i++) {
//             //     std::cout << "NAME:" << meta->getColumnName(i) << " VALUE: " << res->getString(i) << "\n";
//             // }

//             // std::cout << "\n";
//             pist_stat.id_pist = res->getInt("id_pist");
//             pist_stat.tank_.id_tank = res->getInt("id_tank");
//             pist_stat.tank_.tovar_.id_tovar = res->getInt("id_tovar");
//             pist_stat.tank_.tovar_.name = res->getString("name");
//             pist_stat.tank_.tovar_.price =res->getDouble("price");
//             int32_t color=res->getInt("color");
//             unsigned int red = (color >> 16) & 0xFF;
//             unsigned int green = (color >> 8) & 0xFF;
//             unsigned int blue = color & 0xFF;
//             pist_stat.tank_.rgb.r=red;
//             pist_stat.tank_.rgb.g=green;
//             pist_stat.tank_.rgb.b=blue;
//             if (res->getInt("id_trk") == last_idtkr) {
//                 Trks[Trks.size() - 1].pists.push_back(pist_stat);
//                 continue;
//             }
//             p_stat.pists.push_back(pist_stat);
//             p_stat.id_trk = res->getInt("id_trk");
//             p_stat.x_pos = res->getInt("x_pos");
//             p_stat.y_pos = res->getInt("y_pos");
//             p_stat.scale = res->getDouble("scale");

//             last_idtkr = p_stat.id_trk;
//             Trks.push_back(p_stat);
//             p_stat.pists.resize(0);
//         }
//         delete res;
//         delete stmt; 
//         sql = "SELECT * FROM trk WHERE id_azs='" + azs_id + "';";

//     } catch (const sql::SQLException& error) {
//         std::cout << "ERROR MYSQL: " << error.what() << "\n";
//         isconn = false;
//     }

//     return Trks;
// }
std::vector<model::Tovar> get_Tovars(){
    std::vector<model::Tovar> tovars;
    return tovars;
}
bool  model::compareByid(const  model::Tank &a, const  model::Tank &b) {
    return a.id_tank < b.id_tank;
}
std::vector<model::Tovar> model::Azs_Database::get_Tovars(){
    std::vector<model::Tovar> tovars;
    try {
        stmt = con->createStatement();
        std::string sql ="SELECT * FROM tovar ORDER BY NN;";
        sql::ResultSet* res = stmt->executeQuery(sql);
        int index=0;
        
        while (res->next()) {
            Tovar tov;
            tov.id_tovar=res->getInt("id_tovar");
            tov.name=res->getString("name");
            tov.name_p=res->getString("name_p");
            tov.name_p_f=res->getString("name_p_f");
            tov.name_p_v=res->getString("name_p_v");
            int32_t color=res->getInt("color");
            unsigned int red = (color >> 16) & 0xFF;
            unsigned int green = (color >> 8) & 0xFF;
            unsigned int blue = color & 0xFF;
            tov.color.r=red;
            tov.color.g=green;
            tov.color.b=blue;
            tovars.push_back(tov);
        }
        delete res;
        delete stmt; 
    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
    }
    return tovars;
}
std::vector<model::Tank> model::Azs_Database::get_Tanks(){
    std::vector<model::Tank> tanks;
    try {
        stmt = con->createStatement();
        std::string sql ="SELECT * FROM tank ORDER BY NN;";
        sql::ResultSet* res = stmt->executeQuery(sql);
        int index=0;
        
        while (res->next()) {
            Tank tank;
            tank.id_tank=res->getInt("id_tank");
            tank.id_tovar=res->getInt("id_tovar");
            tank.remain=res->getInt("remain");
            tank.volume=res->getInt("volume");
            tanks.push_back(tank);
        }
        delete res;
        delete stmt; 
    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
    }
    return tanks;
}
std::vector<model::Trk> model::Azs_Database::get_Trks(){
    std::vector<model::Trk> trks;
    try {
         stmt = con->createStatement();
        std::string sql = "SELECT com_trk.id_trk, com_trk.x_pos, com_trk.y_pos, com_trk.scale, trk.id_trk, trk.id_pist, trk.id_tank FROM com_trk INNER JOIN trk ON com_trk.id_trk = trk.id_trk WHERE com_trk.id_azs='" + azs_id + "' ORDER BY com_trk.id_trk;";
        sql::ResultSet* res = stmt->executeQuery(sql);
        sql::ResultSetMetaData* meta = res->getMetaData();

        Trk trk;
        Pist pist;
        int last_idtkr = -1;
        while (res->next()) {
            pist.id_pist = res->getInt("id_pist");
            pist.id_tank=res->getInt("id_tank");
            
            if (res->getInt("id_trk") == last_idtkr) {
                trks[trks.size() - 1].pists.push_back(pist);
                continue;
            }

            trk.pists.push_back(pist);
            trk.id_trk = res->getInt("id_trk");
            trk.x_pos = res->getInt("x_pos");
            trk.y_pos = res->getInt("y_pos");
            trk.scale = res->getDouble("scale");

            last_idtkr = trk.id_trk;
            trks.push_back(trk);
            trk.pists.resize(0);
        }
        delete res;
        delete stmt; 
    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
    }
    return trks;
}
std::vector<model::Trk> model::Azs_Database::get_Trks_with_all(Screen_Size* screen,model::VectorWrapper<model::Tovar>* tovars,model::VectorWrapper<model::Tank>* tanks)
{
    tovars->arr.resize(0);
    tanks->arr.resize(0);
    std::vector<model::Trk> trks;
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
        std::string sql ="SELECT * FROM tovar ORDER BY NN;";
        sql::ResultSet* res = stmt->executeQuery(sql);
        int index=0;
        
        while (res->next()) {
            Tovar* tov=new Tovar;
            tov->id_tovar=res->getInt("id_tovar");
            tov->name=res->getString("name");
            tov->name_p=res->getString("name_p");
            tov->name_p_f=res->getString("name_p_f");
            tov->name_p_v=res->getString("name_p_v");
            int32_t color=res->getInt("color");
            unsigned int red = (color >> 16) & 0xFF;
            unsigned int green = (color >> 8) & 0xFF;
            unsigned int blue = color & 0xFF;
            tov->color.r=red;
            tov->color.g=green;
            tov->color.b=blue;
            tovars->arr.push_back(tov);
        }
        delete res;
        delete stmt; 
    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
    }
    try {
        stmt = con->createStatement();
        std::string sql ="SELECT * FROM tank ORDER BY NN;";
        sql::ResultSet* res = stmt->executeQuery(sql);
        int index=0;
        
        while (res->next()) {
            Tank* tank=new Tank;
            tank->id_tank=res->getInt("id_tank");
            tank->id_tovar=res->getInt("id_tovar");
            tank->remain=res->getInt("remain");
            tank->volume=res->getInt("volume");
            for(int i=0;i<tovars->arr.size();i++){
                if(tank->id_tovar==tovars->arr[i]->id_tovar){
                    tank->tovar_=tovars->arr[i];
                    break;
                }
            }
            tanks->arr.push_back(tank);
        }
        delete res;
        delete stmt; 
    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
    }
    
    try {
        stmt = con->createStatement();
        std::string sql = "SELECT com_trk.id_trk, com_trk.x_pos, com_trk.y_pos, com_trk.scale, trk.id_trk, trk.id_pist, trk.id_tank FROM com_trk INNER JOIN trk ON com_trk.id_trk = trk.id_trk WHERE com_trk.id_azs='" + azs_id + "' ORDER BY com_trk.id_trk;";
        sql::ResultSet* res = stmt->executeQuery(sql);
        sql::ResultSetMetaData* meta = res->getMetaData();

        Trk trk;
        Pist pist;
        int last_idtkr = -1;
        while (res->next()) {
            // std::cout << "COLUM:\n";
            // for (int i = 1; i <= meta->getColumnCount(); i++) {
            //     std::cout << "NAME:" << meta->getColumnName(i) << " VALUE: " << res->getString(i) << "\n";
            // }

            // std::cout << "\n";
            pist.id_pist = res->getInt("id_pist");
            pist.id_tank=res->getInt("id_tank");
            for(int i=0;i<tanks->arr.size();i++){
                if(pist.id_tank==tanks->arr[i]->id_tank){
                    pist.tank_=tanks->arr[i];
                    break;
                }
            }
            //pist_stat.tank_.id_tank = ;
            // pist_stat.tank_.tovar_.id_tovar = res->getInt("id_tovar");
            // pist_stat.tank_.tovar_.name = res->getString("name");
            // pist_stat.tank_.tovar_.price =res->getDouble("price");
            
            if (res->getInt("id_trk") == last_idtkr) {
                trks[trks.size() - 1].pists.push_back(pist);
                continue;
            }

            trk.pists.push_back(pist);
            trk.id_trk = res->getInt("id_trk");
            trk.x_pos = res->getInt("x_pos");
            trk.y_pos = res->getInt("y_pos");
            trk.scale = res->getDouble("scale");

            last_idtkr = trk.id_trk;
            trks.push_back(trk);
            trk.pists.resize(0);
        }
        delete res;
        delete stmt; 
   

    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
    }
    
    //std::sort(tanks.begin(), tanks.end(), compareByid);
    return trks;
}
void model::Azs_Database::get_azs_id()
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
bool model::Azs_Database::auth_check(int32_t userid, std::string password, bool& admin)
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