#include "controller.h"
azs_database* azs_db;
void init_controller(azs_database* azs_database){
    azs_db=azs_database;
}
crow::mustache::rendered_template auth_main() {
  crow::mustache::context ctx;
  return crow::mustache::load("login.html").render();
}
crow::response auth(const crow::request &req) {
  auto x = crow::json::load(req.body);
  if (!x)
    return crow::response(400);
  std::string login = (std::string)x[0]["value"];
  std::string password = (std::string)x[1]["value"];
  std::cout << "LOGIN: " << login << " PASSWORD: " << password << "\n";
  crow::json::wvalue ret({{"status", "not"}});
  if (login == "root" && password == "123") {
    ret["status"] = "yes";
  }
  return crow::response{ret};
}
void static_files(crow::response &res, std::string path) {
  res.set_static_file_info("site/" + path);
  res.end();
}