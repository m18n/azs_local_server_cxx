#pragma once
#include "controller.h"
#include "crow.h"
#include "crow/middlewares/cors.h"
void init_view_login(crow::App<AuthVerefy,DatabaseVerefy,crow::CORSHandler> &app);