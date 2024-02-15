#pragma once

#include"web_tehnology.h"
#include"users_controller.h"
int check_coockie(crow::request& req, crow::response& res);
struct AuthVerefy : crow::ILocalMiddleware {
    struct context { };
    void before_handle(crow::request& req, crow::response& res,context& ctx);
    void after_handle(crow::request& req, crow::response& res, context& /*ctx*/);
};
struct DatabaseVerefy : crow::ILocalMiddleware {
    struct context { };
    void before_handle(crow::request& req, crow::response& res,context& ctx);
    void after_handle(crow::request& req, crow::response& res, context& /*ctx*/);
};

crow::mustache::rendered_template auth_main();
void auth_post( crow::request& req, crow::response &res);
void static_files(crow::response& res, std::string path);
void settingsdb_error( crow::request& req, crow::response& res);
void settingsdb_error_send( crow::request& req, crow::response& res);
void settingsdb_error_check( crow::request& req, crow::response& res);