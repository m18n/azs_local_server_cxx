#pragma once
#include "crow.h"
#include "jwt-cpp/jwt.h"
#include"model.h"
#include <chrono>
extern azs_database* azs_db;
void init_controller(azs_database* azs_database);
crow::mustache::rendered_template auth_main();
crow::response auth(const crow::request &req);
void static_files(crow::response &res, std::string path);

crow::mustache::rendered_template pump();