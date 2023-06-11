#include "controller.h"


void AuthVerefy::before_handle(crow::request& req, crow::response& res,context& ctx)
{
    if(azs_db->isConnect()==false){
        res.set_header("Content-Type", "text/html");
        //mysql_conn_info last=azs_db->get_last_info();
        //crow::json::wvalue ctx={{"host",last.ip},{"user",last.name},{"password",last.password},{"database",last.database},{"port",last.port}};
        auto page = crow::mustache::load("erorrdb.html");
        auto render=page.render();
        res.write(render.body_);
        res.end();
        return;
    }
    std::string cookie = req.get_header_value("Cookie");
    std::string parse = ParseCookie(cookie);
    std::cout << "PARSE: " << parse << "\n";
    crow::json::wvalue jcookie = crow::json::load(parse);
    crow::json::wvalue payload;
    if (jcookie["refresh_token"].dump() == "null" || jcookie["refresh_token"].dump() == "\"\"") {
        res.redirect("/");
        res.end();
        return;
    }
    if (verify_token(jcookie["refresh_token"].dump(true), payload, jwt_secret)) {

        std::string time_str = payload["time_create"].dump(true);
        auto time_create = strtotime(time_str);
        auto now = std::chrono::system_clock::now();
        auto duration = now - time_create;
        int minutes = std::chrono::duration_cast<std::chrono::minutes>(duration).count();
        if (minutes < 100) {
            
            if(payload["admin"].dump(true)=="true"){
                admin.Controller(req.url,req,res);
            }else{
                user.Controller(req.url,req,res);
            }
            return;
        }
    }
    res.redirect("/");
    res.add_header("Set-Cookie", "refresh_token=;path=/;");
    res.end();
}

void AuthVerefy::after_handle(crow::request& req, crow::response& res, context& /*ctx*/)
{
    
}
void DatabaseVerefy::before_handle(crow::request& req, crow::response& res,context& ctx)
{
    if(azs_db->isConnect()==false){
       res.set_header("Content-Type", "text/html");
        //mysql_conn_info last=azs_db->get_last_info();
        //crow::json::wvalue ctx={{"host",last.ip},{"user",last.name},{"password",last.password},{"database",last.database},{"port",last.port}};
        auto page = crow::mustache::load("erorrdb.html");
        auto render=page.render();
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
    
    std::vector<model::user_name> res = azs_db->get_user_name();
    int32_t userid=0;
    bool smena=azs_db->smena_bool(&userid);
    crow::mustache::context ctx = { { "users", "" } };
    model::user_name temp;
   if(smena){
        ctx["smena"]=smena;
        for(int i=0;i<res.size();i++){
            if(res[i].id==userid){
                temp=res[0];
                res[0]=res[i];
                res[i]=temp;
                break;
            }
        }
    }
    for (int i = 0; i < res.size(); i++) {
        ctx["users"][i] = { { "id", res[i].id }, { "name", res[i].name } };
    }
     
    std::cout << ctx.dump() << "\n";
    crow::mustache::rendered_template t = crow::mustache::load("login.html").render(ctx);
    return t;
}

void auth_post(const crow::request& req, crow::response& res)
{
    crow::json::wvalue ret({ { "status", "not" } });
    auto x = crow::json::load(req.body);
    if (!x) {
        res.body = ret.dump();
        res.end();
        return;
    }
    int32_t userid = (int32_t)x[0]["value"];
    std::string password = (std::string)x[1]["value"];
    std::cout << "User: " << userid << " PASSWORD: " << password << "\n";
    bool admin=false;
    if (azs_db->auth_check(userid, password,admin)) {
        int32_t id_smena=0;
        int32_t lastnn=0;
        if(azs_db->smena_bool(&id_smena,&lastnn)){
            azs_db->smena_change_operator(userid,id_smena);
        }else{
            azs_db->smena_add_operator(userid,lastnn);
        }
        res.add_header("Set-Cookie", "refresh_token=" + create_token({ { "user_id", std::to_string(userid) },{"admin",admin} }, jwt_secret)+";path=/;");
        ret["status"] = "yes";
    }
    res.body = ret.dump();
    res.end();
}
void settingsdb_post(const crow::request& req, crow::response& res)
{
    crow::json::wvalue ret({ { "status", "not" } });
    crow::json::wvalue x = crow::json::load(req.body);
    std::string t=x.dump();
    if (x.dump()=="null"||x.dump()=="[]"||x.dump()=="\"\"") {
        res.body = ret.dump();
        res.end();
        return;
    }
    mysql_conn_info info;

    
    info.ip = x[0]["value"].dump(true);
    info.name = x[1]["value"].dump(true);
    info.password = x[2]["value"].dump(true);
    info.database = x[3]["value"].dump(true);
    info.port = x[4]["value"].dump(true);
    info.show();
    ret["status"] = "yes";
    if(azs_db->connect(info)){
        ld->set_mysql_conn_info(info);
    }
    res.body = ret.dump();
    res.end();
}
void static_files(crow::response& res, std::string path)
{
    res.set_static_file_info("site/" + path);
    res.end();
}
