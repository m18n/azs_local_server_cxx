#include "controller.h"
int check_coockie(crow::request& req, crow::response& res){
    std::string cookie = req.get_header_value("Cookie");
    std::string parse = ParseCookie(cookie);
    std::cout << "PARSE: " << parse << "\n";
    crow::json::wvalue jcookie = crow::json::load(parse);
    crow::json::wvalue payload;
    if (jcookie["refresh_token"].dump() == "null" || jcookie["refresh_token"].dump() == "\"\"") {
        return -1;
    }
    if (verify_token(jcookie["refresh_token"].dump(true), payload)) {

        std::string time_str = payload["time_create"].dump(true);
        auto time_create = strtotime(time_str);
        auto now = std::chrono::system_clock::now();
        auto duration = now - time_create;
        int minutes = std::chrono::duration_cast<std::chrono::minutes>(duration).count();
        if (minutes < 100) {

            if (payload["admin"].dump(true) == "true") {
                return 1;
            } else {
                return 0;
            }
        
        }
    }
    return -1;
}
void AuthVerefy::before_handle(crow::request& req, crow::response& res, context& ctx)
{
    //res.add_header("Access-Control-Allow-Origin", "*");
    if (azs_db->isConnect() == false) {
        azs_db->connect_async(azs_db->get_last_connect_info());
        res.set_header("Content-Type", "text/html");
        // mysql_conn_info last=azs_db->get_last_info();
        // nlohmann::json ctx={{"host",last.ip},{"user",last.name},{"password",last.password},{"database",last.database},{"port",last.port}};
        auto page = crow::mustache::load("public/old/erorr_db.html");
        auto render = page.render();
        res.write(render.body_);
        res.end();
        return;
    }
    #ifdef DEBUG_BUILD
        admin.Controller(req.url, req, res);
    #elif defined(RELEASE_BUILD)
        int result=check_coockie(req,res);
        if(result==1){
            admin.Controller(req.url, req, res);
        }else if(result==0){
            user.Controller(req.url, req, res);
        }else{
            res.redirect("/");
            res.add_header("Set-Cookie", "refresh_token=;path=/;");
            res.end();
            return;
        }
    #endif
    
}

void AuthVerefy::after_handle(crow::request& req, crow::response& res, context& /*ctx*/)
{
}
void DatabaseVerefy::before_handle(crow::request& req, crow::response& res, context& ctx)
{
    //res.add_header("Access-Control-Allow-Origin", "*");
    if (azs_db->isConnect() == false) {
        azs_db->connect_async(azs_db->get_last_connect_info());
        res.set_header("Content-Type", "text/html");
        // mysql_conn_info last=azs_db->get_last_info();
        // nlohmann::json ctx={{"host",last.ip},{"user",last.name},{"password",last.password},{"database",last.database},{"port",last.port}};
        auto page = crow::mustache::load("public/old/erorr_db.html");
        auto render = page.render();
        res.write(render.body_);
        res.end();
        return;
    }
}

void DatabaseVerefy::after_handle(crow::request& req, crow::response& res, context& /*ctx*/)
{
}

crow::mustache::rendered_template auth_main()
{

    std::vector<model::User_Name> res = azs_db->get_Users_Name();
    int32_t userid = 0;
    bool smena = azs_db->smena_bool(&userid);
    crow::mustache::context ctx = { { "users", "" } };
    model::User_Name temp;
    if (smena) {
        ctx["smena"] = smena;
        for (int i = 0; i < res.size(); i++) {
            if (res[i].id == userid) {
                temp = res[0];
                res[0] = res[i];
                res[i] = temp;
                break;
            }
        }
    }
    for (int i = 0; i < res.size(); i++) {
        ctx["users"][i] = { { "id", res[i].id }, { "name", res[i].name } };
    }

    std::cout << ctx.dump() << "\n";
    crow::mustache::rendered_template t = crow::mustache::load("public/old/login.html").render(ctx);
    return t;
}

void auth_post( crow::request& req, crow::response& res)
{
    nlohmann::json ret({ { "status", "not" } });
    nlohmann::json x = nlohmann::json::parse(req.body);
    std::string str=x[0]["value"];
    int32_t userid = std::stoi(str);
    std::string password = x[1]["value"];
    std::cout << "User: " << userid << " PASSWORD: " << password << "\n";
    bool admin = false;
    if (azs_db->auth_check(userid, password, admin)) {
        int32_t id_smena = 0;
        int32_t lastnn = 0;
        if (azs_db->smena_bool(&id_smena, &lastnn)) {
            azs_db->smena_change_operator(userid, id_smena);
        } else {
            azs_db->smena_add_operator(userid, lastnn);
        }
        res.add_header("Set-Cookie", "refresh_token=" + create_token({ { "user_id", std::to_string(userid) }, { "admin", admin } }) + ";path=/;");
        ret["status"] = "yes";
    }
    res.body = ret.dump();
    res.end();
}
void settingsdb_error( crow::request& req, crow::response& res)
{
    if(azs_db->isConnect()==false||check_coockie(req,res)==1){
        res.set_header("Content-Type", "text/html");
        mysql_conn_info last = azs_db->get_last_input_info();
        nlohmann::json ctx = { { "host", last.ip }, { "user", last.name }, { "password", last.password }, { "database", last.database }, { "port", last.port }};
        auto page = crow::mustache::load("public/old/settings_db_error.html");
        
        auto render = page.render(crow::json::load(ctx.dump()));
        res.write(render.body_);
        res.end();
    }else{
        res.redirect("/");
        res.end();
    }
    
}
void settingsdb_error_send(crow::request& req, crow::response& res)
{
    if(azs_db->isConnect()==false||check_coockie(req,res)==1){
        nlohmann::json ret({ { "status", "not" } });
        nlohmann::json x = nlohmann::json::parse(req.body);
        std::string t = x.dump();
        if (x.dump() == "null" || x.dump() == "[]" || x.dump() == "\"\"") {
            res.body = ret.dump();
            res.end();
            return;
        }
        mysql_conn_info info;

        info.ip = x[0]["value"];
        info.name = x[1]["value"];
        info.password = x[2]["value"];
        info.database = x[3]["value"];
        info.port = x[4]["value"];
        info.show();
        ret["status"] = "ok";
        
        // if (azs_db->connect(info) == true) {
        //     ret["status"] = "yes";
        //     ld->set_mysql_conn_info(info);
        // }
        azs_db->connect_async(info);
        res.body = ret.dump();
        res.end();
    }else{
        res.redirect("/");
        res.end();
    }
}
void settingsdb_error_check( crow::request& req, crow::response& res){
    if(azs_db->isConnect()==false||check_coockie(req,res)==1){
        nlohmann::json ret({ { "status", "not" } });
        
        if(azs_db->while_connect()==true){
            ret["status"]="process";
        }else if(azs_db->isConnect()==true){
            ret["status"]="connect";
        }else{
            ret["status"]="disconnect";
        }
      
        res.body = ret.dump();
        res.end();
    }else{
        res.redirect("/");
        res.end();
    }
}
void static_files(crow::response& res, std::string path)
{
    res.set_static_file_info("azs_site/" + path);
    res.end();
}
