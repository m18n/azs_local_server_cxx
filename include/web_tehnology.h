#pragma once

#include "jwt-cpp/jwt.h"
#include"core.h"
#include "local_data.h"
#include "model.h"
extern model::Azs_Database* azs_db;
extern local_data* ld;
void init_web(model::Azs_Database* azs_database, local_data* local);
std::string create_token(crow::json::wvalue json);
std::string create_token();
bool verify_token(std::string token, crow::json::wvalue& jpayload);