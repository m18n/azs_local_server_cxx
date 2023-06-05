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
void model::azs_database::save_pump_scale(int32_t idpump, float scale)
{
    try {
        stmt = con->createStatement();
        std::string sql = "UPDATE com_trk SET scale="+std::to_string(scale)+" WHERE id_trk=" + std::to_string(idpump) + ";";
        int t = stmt->executeUpdate(sql);
        
        delete stmt;
    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
    }
}
std::vector<model::pump> model::azs_database::get_pump()
{
    std::vector<model::pump> pumps;
    try {
        stmt = con->createStatement();
        std::string sql = "SELECT * FROM com_trk WHERE id_azs='" + azs_id + "';";
        sql::ResultSet* res = stmt->executeQuery(sql);
        pump p;
        while (res->next()) {
            p.id_trk = res->getString("id_trk");
            p.x_pos = res->getInt("x_pos");
            p.y_pos = res->getInt("y_pos");
            p.scale = res->getDouble("scale");
            pumps.push_back(p);
        }
        delete res;
        delete stmt;
    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
    }
    pumps.resize(2);
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