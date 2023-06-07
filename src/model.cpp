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
        std::string sql = "UPDATE com_trk SET scale=" + std::to_string(scale) + " WHERE id_trk=" + std::to_string(idpump) + ";";
        int t = stmt->executeUpdate(sql);

        delete stmt;
    } catch (const sql::SQLException& error) {
        std::cout << "ERROR MYSQL: " << error.what() << "\n";
        isconn = false;
    }
}
void model::azs_database::save_pump_xy(int32_t id, int32_t x, int32_t y)
{
    try {
        stmt = con->createStatement();
        std::string sql = "UPDATE com_trk SET x_pos=" + std::to_string(x) + ", y_pos=" + std::to_string(y) + " WHERE id_trk=" + std::to_string(id) + ";";
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
        std::string sql = "SELECT com_trk.id_trk, com_trk.x_pos, com_trk.y_pos, com_trk.scale, trk.id_trk, trk.id_pist, trk.id_tank, tank.id_tovar, tovar.name, tovar.price FROM com_trk INNER JOIN trk ON com_trk.id_trk = trk.id_trk INNER JOIN tank ON trk.id_tank = tank.id_tank INNER JOIN tovar ON tank.id_tovar = tovar.id_tovar WHERE com_trk.id_azs='" + azs_id + "' ORDER BY com_trk.id_trk;";
        sql::ResultSet* res = stmt->executeQuery(sql);
        sql::ResultSetMetaData* meta = res->getMetaData();

      
        pump p_stat;
        pist pist_stat;
        int last_idtkr = -1;
        while (res->next()) {
            //std::cout << "COLUM:\n";
            // for (int i = 1; i <= meta->getColumnCount(); i++) {
            //     std::cout << "NAME:" << meta->getColumnName(i) << " VALUE: " << res->getString(i) << "\n";
            // }
            // std::cout << "\n";
            pist_stat.id_pist = res->getInt("id_pist");
            pist_stat.tank_.id_tank = res->getInt("id_tank");
            pist_stat.tank_.tovar_.id_tovar=res->getInt("id_tovar");
            pist_stat.tank_.tovar_.name=res->getString("name");
            pist_stat.tank_.tovar_.price=res->getDouble("price");
            if (res->getInt("id_trk") == last_idtkr) {
                pumps[pumps.size() - 1].pists.push_back(pist_stat);
                continue;
            }   
            p_stat.pists.push_back(pist_stat);
            p_stat.id_trk = res->getInt("id_trk");
            p_stat.x_pos = res->getInt("x_pos");
            p_stat.y_pos = res->getInt("y_pos");
            p_stat.scale = res->getDouble("scale");
            
            last_idtkr=p_stat.id_trk;
            pumps.push_back(p_stat);
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