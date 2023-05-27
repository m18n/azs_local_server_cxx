#include "model.h"
std::vector<user_name> azs_database::get_user_name()
{
    std::vector<user_name> users;
    stmt = con->createStatement();
    sql::ResultSet* res = stmt->executeQuery("SELECT * FROM ref_users");
    user_name u;
    while (res->next()) {
        u.id = res->getInt("id_user");
        u.name = res->getString("user");
        users.push_back(u);
    }
    delete res;
    delete stmt;
    return users;
}
bool azs_database::auth_check(int32_t userid, std::string password)
{
    stmt = con->createStatement();
    sql::ResultSet* res = stmt->executeQuery("SELECT password FROM loc_users WHERE id_user=" + std::to_string(userid) + ";");
    res->next();
    bool auth = false;
    if (res->getString("password") == password) {
        auth = true;
    }
    delete res;
    delete stmt;
    return auth;
}