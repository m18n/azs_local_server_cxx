#pragma once
#include "crow.h"
#include "jwt-cpp/jwt.h"
#include "local_data.h"
#include "model.h"
#include <chrono>
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> 
#include <iostream>
std::string ParseCookie(std::string cookie);
// crow::json::wvalue mapToCrowJson(std::map<std::string, picojson::value> map);
struct AuthVerefy : crow::ILocalMiddleware {
    struct context { };
    void before_handle(crow::request& req, crow::response& res, context& /*ctx*/);
    void after_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/);
};
std::string create_token(crow::json::wvalue json);
std::string create_token();
std::string timetostr(std::chrono::system_clock::time_point time);
std::chrono::system_clock::time_point strtotime(std::string time);
extern azs_database* azs_db;
extern local_data* ld;
extern std::string jwt_secret;
bool verify_token(std::string token, crow::json::wvalue& jpayload);
void init_controller(azs_database* azs_database, local_data* local);
crow::mustache::rendered_template auth_main();
void auth(const crow::request& req, crow::response &res);
void static_files(crow::response& res, std::string path);

crow::mustache::rendered_template pump();