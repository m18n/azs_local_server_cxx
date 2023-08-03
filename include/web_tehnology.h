#pragma once
#include "crow.h"
#include "jwt-cpp/jwt.h"
#include"core.h"
#include "local_data.h"
#include "model.h"
extern model::azs_database* azs_db;
extern local_data* ld;
void init_web(model::azs_database* azs_database, local_data* local);
std::string create_token(crow::json::wvalue json);
std::string create_token();
bool verify_token(std::string token, crow::json::wvalue& jpayload);