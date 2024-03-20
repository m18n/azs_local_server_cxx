#include "model.h"
uint32_t model::get_rgb(int r, int g, int b) {
  return ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}
model::Tovar model::json_to_tovar(nlohmann::json json) {
  Tovar tov;
  try {
    tov.id_tovar = json["id_tovar"];
    tov.name = json["name"];
    tov.name_p = json["name_p"];
    tov.name_p_f = json["name_p_f"];
    tov.name_p_v = json["name_p_v"];
    tov.price = json["price"];

    tov.color.r = json["color"]["r"];
    tov.color.g = json["color"]["g"];
    tov.color.b = json["color"]["b"];
  } catch (const std::exception &e) {
    Tovar empty;
    return empty;
  }
  return tov;
}
bool model::Azs_Database::executeSql(std::string sql){
        try {
            stmt = con->createStatement();
            int t = stmt->executeUpdate(sql);

            delete stmt;
            return true;
        } catch (const sql::SQLException &error) {
            std::cout << "ERROR MYSQL: " << error.what() << "\n";
            isconn = false;
            return false;
        }
    }
model::Tank model::json_to_tank(nlohmann::json json) {
  Tank tank;
  try {
    tank.id_tank = json["id_tank"];
    tank.id_tovar = json["id_tovar"];
    tank.remain = json["remain"];
    tank.volume = json["volume"];
  } catch (const std::exception &e) {
    Tank empty;
    return empty;
  }
  return tank;
}
model::Trk model::json_to_trk(nlohmann::json json) {
  Trk trk;
  try {
    for (int i = 0; i < json["pists"].size(); i++) {

      Pist pist;
      pist.id_pist = json["pists"][i]["id_pist"];
      pist.id_tank = json["pists"][i]["id_tank"];
      trk.pists.push_back(pist);
    }
    trk.id_trk = json["id_trk"];
    trk.x_pos = json["x_pos"];
    trk.y_pos = json["y_pos"];
    trk.scale = json["scale"];
  } catch (const std::exception &e) {
    Trk empty;
    return empty;
  }
  return trk;
}
std::string model::floatToString(float number, int step) {
  std::stringstream stream;
  stream << std::fixed << std::setprecision(step) << number;
  return stream.str();
}
std::vector<model::User_Name> model::Azs_Database::get_Users_Name() {

  std::vector<model::User_Name> users;
  try {
    stmt = con->createStatement();
    std::string sql =
        "SELECT * FROM loc_users INNER JOIN ref_users ON loc_users.id_user = "
        "ref_users.id_user WHERE loc_users.id_station='" +
        azs_id + "';";
    sql::ResultSet *res = stmt->executeQuery(sql);
    User_Name u;
    while (res->next()) {
      u.id = res->getInt("id_user");
      u.name = res->getString("user");
      users.push_back(u);
    }
    delete res;
    delete stmt;
  } catch (const sql::SQLException &error) {
    std::cout << "ERROR MYSQL: " << error.what() << "\n";
    isconn = false;
  }
  return users;
}

void model::Azs_Database::save_Trks(std::vector<model::Trk> trks,
                                    int screen_width, int screen_height) {
  try {
    stmt = con->createStatement();
    std::string sql = "UPDATE loc_const SET value=\"" +
                      std::to_string(screen_width) + "," +
                      std::to_string(screen_height) +
                      "\" WHERE descr_var=\"cnst_ScreenSize\";";
    int t = stmt->executeUpdate(sql);

    delete stmt;
  } catch (const sql::SQLException &error) {
    std::cout << "ERROR MYSQL: " << error.what() << "\n";
    isconn = false;
  }
  for (int i = 0; i < trks.size(); i++) {
    try {
      stmt = con->createStatement();
      std::string sql =
          "UPDATE com_trk SET scale=" + std::to_string(trks[i].scale) +
          ", x_pos=" + std::to_string(trks[i].x_pos) +
          ", y_pos=" + std::to_string(trks[i].y_pos) +
          " WHERE id_trk=" + std::to_string(trks[i].id_trk) + ";";
      int t = stmt->executeUpdate(sql);

      delete stmt;
    } catch (const sql::SQLException &error) {
      std::cout << "ERROR MYSQL: " << error.what() << "\n";
      isconn = false;
    }
  }
}
void model::Azs_Database::set_Tovar(Tovar &tovar) {
  try {
    stmt = con->createStatement();
    std::string price = floatToString(tovar.price, 2);
    std::cout << "PRICE: " << price << "\n";
    auto color = get_rgb(tovar.color.r, tovar.color.g, tovar.color.b);
    std::string sql =
        "UPDATE tovar SET id_tovar=" + std::to_string(tovar.id_tovar) +
        ", price=" + price + ", name=\"" + tovar.name + "\", name_p=\"" +
        tovar.name_p + "\", name_p_f=\"" + tovar.name_p_f + "\", name_p_v=\"" +
        tovar.name_p_v + "\", color=" + std::to_string(color) +
        " WHERE id_tovar=" + std::to_string(tovar.id_tovar) + ";";
    int t = stmt->executeUpdate(sql);

    delete stmt;
  } catch (const sql::SQLException &error) {
    std::cout << "ERROR MYSQL: " << error.what() << "\n";
    isconn = false;
  }
}
void model::Azs_Database::set_Trk(Trk &trk) {
  try {
    stmt = con->createStatement();
    std::string sql =
        "UPDATE com_trk SET id_trk=" + std::to_string(trk.id_trk) +
        ", x_pos=" + std::to_string(trk.x_pos) +
        ", y_pos=" + std::to_string(trk.y_pos) +
        ", scale=" + std::to_string(trk.scale) +
        " WHERE id_trk=" + std::to_string(trk.id_trk) + ";";
    int t = stmt->executeUpdate(sql);

    delete stmt;
    int max_id_pist = 0;
    stmt = con->createStatement();
    sql = "SELECT MAX(id_pist) AS max_id_pist FROM trk WHERE id_trk=" +
          std::to_string(trk.id_trk) + ";";
    sql::ResultSet *res = stmt->executeQuery(sql);
    if (res->next()) { // Тут змінено на if, тому що ми очікуємо лише один рядок
                       // результату.
      max_id_pist = res->getInt("max_id_pist");
    }
    delete res;
    delete stmt;
    int n=0;
    for (int i = 0; i < trk.pists.size(); i++) {
      

      if (trk.pists[i].id_pist == 0) {
        n++;
        sql = "INSERT INTO trk (id_trk,id_pist,id_tank) VALUES (" +
              std::to_string(trk.id_trk) + ", " +
              std::to_string(max_id_pist+n) + ", " +
              std::to_string(trk.pists[i].id_tank) + ");";
      } else {
        sql = "UPDATE trk SET id_trk=" + std::to_string(trk.id_trk) +
              ", id_pist=" + std::to_string(trk.pists[i].id_pist) +
              ", id_tank=" + std::to_string(trk.pists[i].id_tank) +
              " WHERE id_trk=" + std::to_string(trk.id_trk) +
              " AND id_pist=" + std::to_string(trk.pists[i].id_pist) + ";";
      }

      stmt = con->createStatement();

      int t = stmt->executeUpdate(sql);

      delete stmt;
    }
  } catch (const sql::SQLException &error) {
    std::cout << "ERROR MYSQL: " << error.what() << "\n";
    isconn = false;
  }
}
void model::Azs_Database::set_Tank(Tank &tank) {
  try {
    stmt = con->createStatement();
    std::string sql =
        "UPDATE tank SET id_tank=" + std::to_string(tank.id_tank) +
        ", id_tovar=" + std::to_string(tank.id_tovar) +
        ", volume=" + std::to_string(tank.volume) +
        ", remain=" + std::to_string(tank.remain) +
        " WHERE id_tank=" + std::to_string(tank.id_tank) + ";";
    int t = stmt->executeUpdate(sql);

    delete stmt;
  } catch (const sql::SQLException &error) {
    std::cout << "ERROR MYSQL: " << error.what() << "\n";
    isconn = false;
  }
}
void model::Azs_Database::delete_Tovar(Tovar &tovar){
  executeSql("DELETE FROM tovar WHERE id_tovar=" + std::to_string(tovar.id_tovar) + ";");
}
void model::Azs_Database::delete_Trk(Trk &trk,bool smart_delete){
  if(smart_delete==false){
       executeSql("DELETE FROM com_trk WHERE id_trk=" + std::to_string(trk.id_trk) + ";");
       executeSql("DELETE FROM trk WHERE id_trk=" + std::to_string(trk.id_trk) + ";");
    }else{
      if(trk.pists.size()==0){
        executeSql("DELETE FROM com_trk WHERE id_trk=" + std::to_string(trk.id_trk) + ";");
      }else{
        for(int i=0;i<trk.pists.size();i++){
          executeSql("DELETE FROM trk WHERE id_trk=" + std::to_string(trk.id_trk) + " AND id_pist="+std::to_string(trk.pists[i].id_pist)+";");
        }
      }
    }
}
void model::Azs_Database::delete_Tank(Tank &tank){
  executeSql("DELETE FROM tank WHERE id_tank=" + std::to_string(tank.id_tank) + ";");
}
bool model::Azs_Database::smena_bool(int32_t *last_id, int32_t *last_nn) {
  bool status = false;
  try {
    stmt = con->createStatement();
    std::string sql = "SELECT * FROM smena WHERE id_azs='" + azs_id +
                      "' ORDER BY NSmen DESC LIMIT 1;";
    sql::ResultSet *res = stmt->executeQuery(sql);
    sql::ResultSetMetaData *meta = res->getMetaData();
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
  } catch (const sql::SQLException &error) {
    std::cout << "ERROR MYSQL: " << error.what() << "\n";
    isconn = false;
    return false;
  }
  return status;
}
bool model::Azs_Database::smena_bool() {
  bool status = false;
  try {
    stmt = con->createStatement();
    std::string sql = "SELECT * FROM smena WHERE id_azs='" + azs_id +
                      "' ORDER BY NSmen DESC LIMIT 1;";
    sql::ResultSet *res = stmt->executeQuery(sql);
    sql::ResultSetMetaData *meta = res->getMetaData();

    while (res->next()) {

      std::cout << "NN:" << res->getInt("NSmen");
      status = res->getInt("status");
    }
    delete res;
    delete stmt;
  } catch (const sql::SQLException &error) {
    std::cout << "ERROR MYSQL: " << error.what() << "\n";
    isconn = false;
    return false;
  }
  return status;
}
bool model::Azs_Database::smena_bool(int32_t *userid) {
  bool status = false;
  try {
    stmt = con->createStatement();
    std::string sql = "SELECT * FROM smena WHERE id_azs='" + azs_id +
                      "' ORDER BY NSmen DESC LIMIT 1;";
    sql::ResultSet *res = stmt->executeQuery(sql);
    sql::ResultSetMetaData *meta = res->getMetaData();

    while (res->next()) {

      std::cout << "NN:" << res->getInt("NSmen");
      *userid = res->getInt("id_operator");
      status = res->getInt("status");
    }
    delete res;
    delete stmt;
  } catch (const sql::SQLException &error) {
    std::cout << "ERROR MYSQL: " << error.what() << "\n";
    isconn = false;
    return false;
  }
  return status;
}
void model::Azs_Database::smena_change_operator(int32_t id_operator,
                                                int32_t id_smena) {
  try {
    stmt = con->createStatement();
    std::string sql =
        "UPDATE smena SET id_operator=" + std::to_string(id_operator) +
        " WHERE NSmen=" + std::to_string(id_smena) + ";";
    int t = stmt->executeUpdate(sql);
    delete stmt;
  } catch (const sql::SQLException &error) {
    std::cout << "ERROR MYSQL: " << error.what() << "\n";
    isconn = false;
  }
}
void model::Azs_Database::smena_close() {
  try {
    int32_t last_id = -1;
    int32_t nn = -1;
    bool b = smena_bool(&last_id, &nn);
    std::string time_now = timetostr(std::chrono::system_clock::now());
    stmt = con->createStatement();
    std::string sql = "UPDATE smena SET status=0, sm_end=\"" + time_now +
                      "\"  WHERE NSmen=" + std::to_string(last_id) + ";";
    int t = stmt->executeUpdate(sql);
    delete stmt;
  } catch (const sql::SQLException &error) {
    std::cout << "ERROR MYSQL: " << error.what() << "\n";
    isconn = false;
  }
}
void model::Azs_Database::smena_add_operator(int32_t id_operator, int32_t nn) {
  try {
    stmt = con->createStatement();
    std::string time_now = timetostr(std::chrono::system_clock::now());
    nn++;
    std::string sql =
        "INSERT INTO smena "
        "(NN,id_azs,sm_start,sm_end,id_operator,status,id_ppo,znum) VALUES (" +
        std::to_string(nn) + "," + azs_id + ",'" + time_now + "','" + time_now +
        "'," + std::to_string(id_operator) + ",1,10,10);";
    std::cout << "SQL: " << sql << "\n";
    int t = stmt->executeUpdate(sql);
    std::cout << "INSERT OK\n";
    delete stmt;
  } catch (const sql::SQLException &error) {
    std::cout << "ERROR MYSQL: " << error.what() << "\n";
    isconn = false;
  }
}
// std::vector<model::Trk> model::Azs_Database::get_Trks(Screen_Size* screen)
// {
//     std::vector<model::Trk> Trks;
//     try {
//         stmt = con->createStatement();
//         std::string sql ="SELECT * FROM loc_const WHERE
//         descr_var=\"cnst_ScreenSize\";"; sql::ResultSet* res =
//         stmt->executeQuery(sql); while (res->next()) {
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
//         std::string sql = "SELECT com_trk.id_trk, com_trk.x_pos,
//         com_trk.y_pos, com_trk.scale, trk.id_trk, trk.id_pist, trk.id_tank,
//         tank.id_tovar,tank.color, tovar.name, tovar.price FROM com_trk INNER
//         JOIN trk ON com_trk.id_trk = trk.id_trk INNER JOIN tank ON
//         trk.id_tank = tank.id_tank INNER JOIN tovar ON tank.id_tovar =
//         tovar.id_tovar WHERE com_trk.id_azs='" + azs_id + "' ORDER BY
//         com_trk.id_trk;"; sql::ResultSet* res = stmt->executeQuery(sql);
//         sql::ResultSetMetaData* meta = res->getMetaData();

//         Trk p_stat;
//         Pist pist_stat;
//         int last_idtkr = -1;
//         while (res->next()) {
//             // std::cout << "COLUM:\n";
//             // for (int i = 1; i <= meta->getColumnCount(); i++) {
//             //     std::cout << "NAME:" << meta->getColumnName(i) << " VALUE:
//             " << res->getString(i) << "\n";
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
std::vector<model::Tovar> get_Tovars() {
  std::vector<model::Tovar> tovars;
  return tovars;
}
bool model::compareByid(const model::Tank &a, const model::Tank &b) {
  return a.id_tank < b.id_tank;
}
std::vector<model::Tovar> model::Azs_Database::get_Tovars() {
  std::vector<model::Tovar> tovars;
  try {
    stmt = con->createStatement();
    std::string sql = "SELECT * FROM tovar ORDER BY NN;";
    sql::ResultSet *res = stmt->executeQuery(sql);
    int index = 0;

    while (res->next()) {
      Tovar tov;
      tov.id_tovar = res->getInt("id_tovar");
      tov.name = res->getString("name");
      tov.name_p = res->getString("name_p");
      tov.name_p_f = res->getString("name_p_f");
      tov.name_p_v = res->getString("name_p_v");
      int32_t color = res->getInt("color");
      // unsigned int red = GetRValue(color);
      // unsigned int green = GetGValue(color);
      // unsigned int blue = GetBValue(color);
      unsigned int red = (color >> 16) & 0xFF;
      unsigned int green = (color >> 8) & 0xFF;
      unsigned int blue = color & 0xFF;
      tov.color.r = red;
      tov.color.g = green;
      tov.color.b = blue;
      tovars.push_back(tov);
    }
    delete res;
    delete stmt;
  } catch (const sql::SQLException &error) {
    std::cout << "ERROR MYSQL: " << error.what() << "\n";
    isconn = false;
  }
  return tovars;
}
std::vector<model::Tank> model::Azs_Database::get_Tanks() {
  std::vector<model::Tank> tanks;
  try {
    stmt = con->createStatement();
    std::string sql = "SELECT * FROM tank ORDER BY NN;";
    sql::ResultSet *res = stmt->executeQuery(sql);
    int index = 0;

    while (res->next()) {
      Tank tank;
      tank.id_tank = res->getInt("id_tank");
      tank.id_tovar = res->getInt("id_tovar");
      tank.remain = res->getInt("remain");
      tank.volume = res->getInt("volume");
      tanks.push_back(tank);
    }
    delete res;
    delete stmt;
  } catch (const sql::SQLException &error) {
    std::cout << "ERROR MYSQL: " << error.what() << "\n";
    isconn = false;
  }
  return tanks;
}
std::vector<model::Trk> model::Azs_Database::get_Trks() {
  std::vector<model::Trk> trks;
  try {
    stmt = con->createStatement();
    std::string sql =
        "SELECT com_trk.id_trk, com_trk.x_pos, com_trk.y_pos, com_trk.scale, "
        "trk.id_trk, trk.id_pist, trk.id_tank FROM com_trk INNER JOIN trk ON "
        "com_trk.id_trk = trk.id_trk WHERE com_trk.id_azs='" +
        azs_id + "' ORDER BY com_trk.id_trk;";
    sql::ResultSet *res = stmt->executeQuery(sql);
    sql::ResultSetMetaData *meta = res->getMetaData();

    Trk trk;
    Pist pist;
    int last_idtkr = -1;
    while (res->next()) {
      pist.id_pist = res->getInt("id_pist");
      pist.id_tank = res->getInt("id_tank");

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
  } catch (const sql::SQLException &error) {
    std::cout << "ERROR MYSQL: " << error.what() << "\n";
    isconn = false;
  }
  return trks;
}
std::vector<model::Trk> model::Azs_Database::get_Trks_with_all(
    Screen_Size *screen, model::VectorWrapper<model::Tovar> *tovars,
    model::VectorWrapper<model::Tank> *tanks) {
  tovars->arr.resize(0);
  tanks->arr.resize(0);
  std::vector<model::Trk> trks;
  try {
    stmt = con->createStatement();
    std::string sql =
        "SELECT * FROM loc_const WHERE descr_var=\"cnst_ScreenSize\";";
    sql::ResultSet *res = stmt->executeQuery(sql);
    while (res->next()) {
      std::string size = res->getString("value");
      char *sizes = strtok(&size[0], ",");
      screen->width = std::stoi(sizes);
      sizes = strtok(NULL, " ");
      screen->height = std::stoi(sizes);
    }
    delete res;
    delete stmt;
  } catch (const sql::SQLException &error) {
    std::cout << "ERROR MYSQL: " << error.what() << "\n";
    isconn = false;
  }
  try {
    stmt = con->createStatement();
    std::string sql = "SELECT * FROM tovar ORDER BY NN;";
    sql::ResultSet *res = stmt->executeQuery(sql);
    int index = 0;

    while (res->next()) {
      Tovar *tov = new Tovar;
      tov->id_tovar = res->getInt("id_tovar");
      tov->name = res->getString("name");
      tov->name_p = res->getString("name_p");
      tov->name_p_f = res->getString("name_p_f");
      tov->name_p_v = res->getString("name_p_v");
      tov->price = res->getDouble("price");
      int32_t color = res->getInt("color");
      unsigned int red = (color >> 16) & 0xFF;
      unsigned int green = (color >> 8) & 0xFF;
      unsigned int blue = color & 0xFF;
      tov->color.r = red;
      tov->color.g = green;
      tov->color.b = blue;
      tovars->arr.push_back(tov);
    }
    delete res;
    delete stmt;
  } catch (const sql::SQLException &error) {
    std::cout << "ERROR MYSQL: " << error.what() << "\n";
    isconn = false;
  }
  try {
    stmt = con->createStatement();
    std::string sql = "SELECT * FROM tank ORDER BY NN;";
    sql::ResultSet *res = stmt->executeQuery(sql);
    int index = 0;

    while (res->next()) {
      Tank *tank = new Tank;
      tank->id_tank = res->getInt("id_tank");
      tank->id_tovar = res->getInt("id_tovar");
      tank->remain = res->getInt("remain");
      tank->volume = res->getInt("volume");
      for (int i = 0; i < tovars->arr.size(); i++) {
        if (tank->id_tovar == tovars->arr[i]->id_tovar) {
          tank->tovar_ = tovars->arr[i];
          break;
        }
      }
      tanks->arr.push_back(tank);
    }
    delete res;
    delete stmt;
  } catch (const sql::SQLException &error) {
    std::cout << "ERROR MYSQL: " << error.what() << "\n";
    isconn = false;
  }

  try {
    stmt = con->createStatement();
    std::string sql =
        "SELECT com_trk.id_trk, com_trk.x_pos, com_trk.y_pos, com_trk.scale, "
        "trk.id_trk, trk.id_pist, trk.id_tank FROM com_trk INNER JOIN trk ON "
        "com_trk.id_trk = trk.id_trk WHERE com_trk.id_azs='" +
        azs_id + "' ORDER BY com_trk.id_trk;";
    sql::ResultSet *res = stmt->executeQuery(sql);
    sql::ResultSetMetaData *meta = res->getMetaData();

    Trk trk;
    Pist pist;
    int last_idtkr = -1;
    while (res->next()) {
      // std::cout << "COLUM:\n";
      // for (int i = 1; i <= meta->getColumnCount(); i++) {
      //     std::cout << "NAME:" << meta->getColumnName(i) << " VALUE: " <<
      //     res->getString(i) << "\n";
      // }

      // std::cout << "\n";
      pist.id_pist = res->getInt("id_pist");
      pist.id_tank = res->getInt("id_tank");
      for (int i = 0; i < tanks->arr.size(); i++) {
        if (pist.id_tank == tanks->arr[i]->id_tank) {
          pist.tank_ = tanks->arr[i];
          break;
        }
      }
      // pist_stat.tank_.id_tank = ;
      //  pist_stat.tank_.tovar_.id_tovar = res->getInt("id_tovar");
      //  pist_stat.tank_.tovar_.name = res->getString("name");
      //  pist_stat.tank_.tovar_.price =res->getDouble("price");

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

  } catch (const sql::SQLException &error) {
    std::cout << "ERROR MYSQL: " << error.what() << "\n";
    isconn = false;
  }

  // std::sort(tanks.begin(), tanks.end(), compareByid);
  return trks;
}
void model::Azs_Database::get_azs_id() {
  try {
    stmt = con->createStatement();
    sql::ResultSet *res = stmt->executeQuery(
        "SELECT * FROM loc_const WHERE descr_var='cnst_ID_Station';");
    res->next();
    bool auth = false;
    azs_id = res->getString("value");

    delete res;
    delete stmt;

  } catch (const sql::SQLException &error) {
    std::cout << "ERROR MYSQL: " << error.what() << "\n";
    isconn = false;
  }
}
bool model::Azs_Database::auth_check(int32_t userid, std::string password,
                                     bool &admin) {
  try {
    stmt = con->createStatement();
    sql::ResultSet *res =
        stmt->executeQuery("SELECT password FROM loc_users WHERE id_user=" +
                           std::to_string(userid) + ";");
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
  } catch (const sql::SQLException &error) {
    std::cout << "ERROR MYSQL: " << error.what() << "\n";
    isconn = false;
    return false;
  }
}