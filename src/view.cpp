#include "view.h"
void init_view_login(crow::App<AuthVerefy,DatabaseVerefy> &app) {
  CROW_ROUTE(app, "/public/<path>")
  ([](const crow::request &, crow::response &res, std::string path) {
    return static_files(res, path);
  });
  CROW_ROUTE(app, "/").CROW_MIDDLEWARES(app, DatabaseVerefy)
  ([]() { return auth_main(); });
  CROW_ROUTE(app, "/auth").methods("POST"_method)([]( crow::request &req, crow::response &res) {
    auth_post(req,res);
  });
   CROW_ROUTE(app, "/settings/dberror")([]( crow::request &req, crow::response &res) {
    settingsdb_error(req,res);
  });
  CROW_ROUTE(app, "/settings/dberror/send").methods("POST"_method)([]( crow::request &req, crow::response &res) {
    settingsdb_error_send(req,res);
  });
  CROW_ROUTE(app, "/settings/dberror/check")([]( crow::request &req, crow::response &res) {
    settingsdb_error_check(req,res);
  });
  CROW_ROUTE(app, "/api").CROW_MIDDLEWARES(app, DatabaseVerefy)([]( ) {
   return"{}";
  });
  CROW_ROUTE(app, URL_MAIN).CROW_MIDDLEWARES(app,AuthVerefy)
  ([](const crow::request &req, crow::response &res) {});
  CROW_ROUTE(app,  API_PUMP_SAVESCALE).methods("POST"_method).CROW_MIDDLEWARES(app,AuthVerefy)
  ([](const crow::request &req, crow::response &res) {});
   CROW_ROUTE(app,  API_PUMP_SAVEXY).methods("POST"_method).CROW_MIDDLEWARES(app,AuthVerefy)
  ([](const crow::request &req, crow::response &res) {});
    CROW_ROUTE(app,  API_OUT).CROW_MIDDLEWARES(app,AuthVerefy)
  ([](const crow::request &req, crow::response &res) {});
  CROW_ROUTE(app, API_OUTSHIFT).CROW_MIDDLEWARES(app,AuthVerefy)
  ([](const crow::request &req, crow::response &res) {});
 
  CROW_ROUTE(app,  SETTINGS_MAIN).CROW_MIDDLEWARES(app,AuthVerefy)
  ([](const crow::request &req, crow::response &res) {});

  
}