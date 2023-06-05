#pragma once
#include "controller.h"
#include "crow.h"
void init_view_login(crow::App<AuthVerefy,DatabaseVerefy> &app);