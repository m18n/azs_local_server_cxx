#pragma once
#include "crow.h"
#include "jwt-cpp/jwt.h"
#include"core.h"
#include "local_data.h"
#include "model.h"
extern model::azs_database* azs_db;
extern local_data* ld;
extern std::string jwt_secret;
void init_web(model::azs_database* azs_database, local_data* local);
std::string create_token(crow::json::wvalue json,std::string jwt_secret);
std::string create_token(std::string jwt_secret);
bool verify_token(std::string token, crow::json::wvalue& jpayload,std::string jwt_secret);