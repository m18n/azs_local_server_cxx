#include "view.h"
void init_view_login(crow::App<AuthVerefy> &app) {
  CROW_ROUTE(app, "/public/<path>")
  ([](const crow::request &, crow::response &res, std::string path) {
    return static_files(res, path);
  });
  CROW_ROUTE(app, "/")
  ([]() { return auth_main(); });
  CROW_ROUTE(app, "/auth").methods("POST"_method)([](const crow::request &req, crow::response &res) {
    auth(req,res);
  });
  CROW_ROUTE(app, "/main").CROW_MIDDLEWARES(app, AuthVerefy)
  ([]() { return pump(); });
}