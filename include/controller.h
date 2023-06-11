#pragma once

#include"web_tehnology.h"
#include"users.h"

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
void auth_post(const crow::request& req, crow::response &res);
void settingsdb_post(const crow::request& req, crow::response& res);
void static_files(crow::response& res, std::string path);
void settingsdb_error(const crow::request& req, crow::response& res);
void settingsdb_error_send(const crow::request& req, crow::response& res);