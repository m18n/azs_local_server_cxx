#pragma once
#include "string"
#include "string.h"
#include "web_tehnology.h"
#define URL_MAIN "/main"
#define URL_MAIN_SETTINGS "/main/settings"
#define URL_MAIN_SETTINGS_CONFIG "/main/settings/configuration"
#define URL_API_PUMP_SAVE "/api/pump/save"
#define URL_API_OUT "/api/out"
#define URL_API_OUTSHIFT "/api/outshift"
#define URL_API_SETTINGS_GET "/api/settings/get"
#define URL_API_SETTINGS_TANKS_GET "/api/settings/tanks/get"
#define URL_API_SETTINGS_TOVARS_GET "/api/settings/tovars/get"
#define URL_API_SETTINGS_TRKS_GET "/api/settings/trks/get"
#define URL_API_SETTINGS_KASA_GET "/api/settings/kasa/get"
class Client {
protected:
    std::map<std::string, std::function<void(crow::request&, crow::response&)>> url_to_func;

protected:
   
    virtual void init(){
        url_to_func[URL_MAIN]=std::bind(&Client::main_pump, this, std::placeholders::_1, std::placeholders::_2);
        url_to_func[URL_MAIN_SETTINGS]=std::bind(&Client::main_settings, this, std::placeholders::_1, std::placeholders::_2);
        url_to_func[URL_MAIN_SETTINGS_CONFIG]=std::bind(&Client::main_settings_config, this, std::placeholders::_1, std::placeholders::_2);
        url_to_func[URL_API_PUMP_SAVE]=std::bind(&Client::api_pump_save, this, std::placeholders::_1, std::placeholders::_2);
        url_to_func[URL_API_OUT]=std::bind(&Client::api_out, this, std::placeholders::_1, std::placeholders::_2);
        url_to_func[URL_API_OUTSHIFT]=std::bind(&Client::api_outshift, this, std::placeholders::_1, std::placeholders::_2);
        url_to_func[URL_API_SETTINGS_GET]=std::bind(&Client::api_settings_get, this, std::placeholders::_1, std::placeholders::_2);
        url_to_func[URL_API_SETTINGS_TANKS_GET]=std::bind(&Client::api_settings_tanks_get, this, std::placeholders::_1, std::placeholders::_2);
        url_to_func[URL_API_SETTINGS_TOVARS_GET]=std::bind(&Client::api_settings_tovars_get, this, std::placeholders::_1, std::placeholders::_2);
        url_to_func[URL_API_SETTINGS_TRKS_GET]=std::bind(&Client::api_settings_trks_get, this, std::placeholders::_1, std::placeholders::_2);
        url_to_func[URL_API_SETTINGS_KASA_GET]=std::bind(&Client::api_settings_kasa_get, this, std::placeholders::_1, std::placeholders::_2);
    }
    virtual void main_pump(crow::request& req, crow::response& res) = 0;
    virtual void main_settings(crow::request& req, crow::response& res) = 0;
    virtual void main_settings_config(crow::request& req, crow::response& res) = 0;
    virtual void api_pump_save(crow::request& req, crow::response& res) = 0;
    virtual void api_out(crow::request& req, crow::response& res) = 0;
    virtual void api_outshift(crow::request& req, crow::response& res) = 0;
    virtual void api_settings_get(crow::request& req, crow::response& res) = 0;
    virtual void api_settings_tanks_get(crow::request& req, crow::response& res) = 0;
    virtual void api_settings_tovars_get(crow::request& req, crow::response& res) = 0;
    virtual void api_settings_trks_get(crow::request& req, crow::response& res) = 0;
    virtual void api_settings_kasa_get(crow::request& req, crow::response& res) = 0;
    void BaseController(std::string url, crow::request& req, crow::response& res)
    {
        
        auto it = url_to_func.find(url);
        if (it != url_to_func.end()) {
            it->second(req, res);
        } 
    }

public:
    Client(){
        init();
    }
    virtual void Controller(std::string url, crow::request& req, crow::response& res)
    {
        BaseController(url, req, res);
    }
};

class Admin : public Client {
private:
    

public:
    Admin():Client(){

    }
    virtual void Controller(std::string url, crow::request& req, crow::response& res)
    {
        BaseController(url, req, res);
    }
    void main_pump(crow::request& req, crow::response& res)
    {
        res.set_header("Content-Type", "text/html");
        auto page = crow::mustache::load("public/old/serv.html");
        crow::mustache::context ctx = { { "admin", true }, { "pump", "" } };
        model::Screen_Size s_size;
        model::VectorWrapper<model::Tank> tanks;
        model::VectorWrapper<model::Tovar> tovars;
        std::vector<model::Trk> trks = azs_db->get_Trks_with_all(&s_size,&tovars,&tanks);
        std::string price;
        for (int i = 0; i < trks.size(); i++) {
            // p[i].show();
            float scale = trks[i].scale;//(float)(3 * p[i].scale)
            std::string s = std::to_string(scale);
            ctx["screen_width"]=s_size.width;
            ctx["pump"][i] = { { "id", trks[i].id_trk }, { "x_pos", (float)trks[i].x_pos/s_size.width*100.0 }, { "y_pos", (float)trks[i].y_pos/s_size.width*100.0 }, { "scale", s }, { "pist", "" } };
            for (int j = 0; j < trks[i].pists.size(); j++) {
                
                price = std::to_string(trks[i].pists[j].tank_->tovar_->price);
                int pos = price.find(".");
                price.resize(pos + 3);
                ctx["pump"][i]["pist"][j] = { { "id_pist", trks[i].pists[j].id_pist }, { "name", trks[i].pists[j].tank_->tovar_->name }, { "price", price }, { "r", trks[i].pists[j].tank_->tovar_->color.r }, { "g", trks[i].pists[j].tank_->tovar_->color.g }, { "b", trks[i].pists[j].tank_->tovar_->color.b } };
            }
        }
        
        std::cout << "P: " << ctx.dump() << "\n";
        auto render = page.render(ctx);
        res.write(render.body_);
        res.end();
    }
    void api_pump_save(crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        crow::json::wvalue ret({ { "status", "yes" } });
        crow::json::wvalue json = crow::json::load(req.body);
        int screen_width=std::stoi(json["screen_scale"]["width"].dump(true));
        int screen_heigth=std::stoi(json["screen_scale"]["height"].dump(true));
        std::vector<model::Trk> trks;
        trks.resize(json["objects"].size());
        std::cout << "json: " << json.dump() << "\n";
        std::cout<<"OBJECT SIZE: "<<json["objects"].size();
        for(int i=0;i<json["objects"].size();i++){
            trks[i].id_trk=std::stoi(json["objects"][i]["id"].dump(true));
            std::string xstr=json["objects"][i]["x"].dump(true);
            std::string ystr=json["objects"][i]["y"].dump(true);
            if(xstr=="")
                xstr="0";
            if(ystr=="")
                ystr="0";
            float x_f=std::stof(xstr);
            float y_f=std::stof(ystr);
            trks[i].x_pos=int(screen_width/(100/x_f));
            trks[i].y_pos=int(screen_width/(100/y_f));
            trks[i].scale=std::stof(json["objects"][i]["scale"].dump(true));
            trks[i].show();
        }
        azs_db->save_Trks(std::move(trks),screen_width,screen_heigth);
        if(!azs_db->isConnect()){
            ret["status"]="not";
        }
        res.body = ret.dump();
        res.end();
    }
    
    void api_out(crow::request& req, crow::response& res)
    {
        res.redirect("/");
        res.add_header("Set-Cookie", "refresh_token=;path=/;");
        res.end();
    }
    void api_outshift(crow::request& req, crow::response& res)
    {
        azs_db->smena_close();
        res.redirect("/");
        res.add_header("Set-Cookie", "refresh_token=;path=/;");
        res.end();
    }
    void api_settings_get(crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        model::Screen_Size s_size;
        model::VectorWrapper<model::Tank> tanks;
        model::VectorWrapper<model::Tovar> tovars;
        std::vector<model::Trk> trks = azs_db->get_Trks_with_all(&s_size,&tovars,&tanks);
        crow::mustache::context ctx = {};
        for(int i=0;i<tovars.arr.size();i++){
            
            ctx["tovars"][i]={{"id_tovar",tovars.arr[i]->id_tovar},{"name",tovars.arr[i]->name},{"name_p",tovars.arr[i]->name_p}
            ,{"name_p_f",tovars.arr[i]->name_p_f},{"name_p_v",tovars.arr[i]->name_p_v},{"price",tovars.arr[i]->price},{"color",{{"r",tovars.arr[i]->color.r},{"g",tovars.arr[i]->color.g},{"b",tovars.arr[i]->color.b}}}};
        }
        for(int i=0;i<tanks.arr.size();i++){
            ctx["tanks"][i]={{"id_tank",tanks.arr[i]->id_tank},{"id_tovar",tanks.arr[i]->id_tovar},{"volume",tanks.arr[i]->volume},{"remain",tanks.arr[i]->remain}};
        }
        for(int i=0;i<trks.size();i++){
            ctx["trks"][i]={{"id_trk",trks[i].id_trk}};
            for(int j=0;j<trks[i].pists.size();j++){
                ctx["trks"][i]["pists"][j]={{"id_pist",trks[i].pists[j].id_pist},{"id_tank",trks[i].pists[j].id_tank}};
            }
        }
        res.body=ctx.dump();
        res.end();
    }
    void api_settings_tanks_get(crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        auto tanks=azs_db->get_Tanks();
        crow::mustache::context ctx = {};
        for(int i=0;i<tanks.size();i++){
            ctx["tanks"][i]={{"id_tank",tanks[i].id_tank},{"id_tovar",tanks[i].id_tovar},{"volume",tanks[i].volume},{"remain",tanks[i].remain}};
        }
        res.body=ctx.dump();
        res.end();
    }
    void api_settings_tovars_get(crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        auto tovars=azs_db->get_Tovars();
        crow::mustache::context ctx = {};
        for(int i=0;i<tovars.size();i++){
             ctx["tovars"][i]={{"id_tovar",tovars[i].id_tovar},{"name",tovars[i].name},{"name_p",tovars[i].name_p}
            ,{"name_p_f",tovars[i].name_p_f},{"name_p_v",tovars[i].name_p_v},{"price",tovars[i].price},{"color",{{"r",tovars[i].color.r},{"g",tovars[i].color.g},{"b",tovars[i].color.b}}}};
        }
        res.body=ctx.dump();
        res.end();
    }
    void api_settings_trks_get(crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        auto trks=azs_db->get_Trks();
        crow::mustache::context ctx = {};
        for(int i=0;i<trks.size();i++){
            ctx["trks"][i]={{"id_trk",trks[i].id_trk}};
            for(int j=0;j<trks[i].pists.size();j++){
                ctx["trks"][i]["pists"][j]={{"id_pist",trks[i].pists[j].id_pist},{"id_tank",trks[i].pists[j].id_tank}};
            }
        }
        res.body=ctx.dump();
        res.end();
    }
    void api_settings_kasa_get(crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        res.end();
    }
    void main_settings(crow::request& req, crow::response& res)
    {
        crow::mustache::context ctx = { { "admin", true } };
        res.set_header("Content-Type", "text/html");
        auto page = crow::mustache::load("public/old/settings_azs.html");
        auto render = page.render(ctx);
        res.write(render.body_);
        res.end();
    }
    void main_settings_config(crow::request& req, crow::response& res){
        model::Screen_Size screen;
         model::VectorWrapper<model::Tank> tanks;
        model::VectorWrapper<model::Tovar> tovars;
        auto trks=azs_db->get_Trks_with_all(&screen,&tovars,&tanks);
        trks[0].show();
        crow::mustache::context ctx = {};
        
        
        for(int i=0;i<trks.size();i++){
            ctx["pumps"][i]={{"id_trk",trks[i].id_trk}};

            for(int j=0;j<trks[i].pists.size();j++){

                
                std::string color=std::to_string(trks[i].pists[j].tank_->tovar_->color.r)+","+std::to_string(trks[i].pists[j].tank_->tovar_->color.g)+","+std::to_string(trks[i].pists[j].tank_->tovar_->color.b);
                ctx["pumps"][i]["pists"][j]={{"id_pist",trks[i].pists[j].id_pist},{"id_tank",trks[i].pists[j].tank_->id_tank},{"color",color},
                {"id_tovar",trks[i].pists[j].tank_->tovar_->id_tovar},{"name",trks[i].pists[j].tank_->tovar_->name},
                {"price",trks[i].pists[j].tank_->tovar_->price}};
            }
           
            
        }
        for(int i=0;i<tanks.arr.size();i++){
            std::string color=std::to_string(tanks.arr[i]->tovar_->color.r)+","+std::to_string(tanks.arr[i]->tovar_->color.g)+","+std::to_string(tanks.arr[i]->tovar_->color.b);
            ctx["tanks"][i]={{"id_tank",tanks.arr[i]->id_tank},{"color",color},{"id_tovar",tanks.arr[i]->tovar_->id_tovar},{"name",tanks.arr[i]->tovar_->name},{"price",tanks.arr[i]->tovar_->price}};
        }
        res.set_header("Content-Type", "text/html");
        auto page = crow::mustache::load("public/old/configuration.html");
        auto render = page.render(ctx);
        res.write(render.body_);
        res.end();
    }
};
class User : public Client {
private:
    

public:
    User():Client(){

    }
    virtual void Controller(std::string url, crow::request& req, crow::response& res)
    {
        BaseController(url, req, res);
    }
    void main_pump(crow::request& req, crow::response& res)
    {
        res.set_header("Content-Type", "text/html");
        auto page = crow::mustache::load("public/old/serv.html");
        crow::mustache::context ctx = { { "pump", "" } };
        model::Screen_Size s_size;
        model::VectorWrapper<model::Tank> tanks;
        model::VectorWrapper<model::Tovar> tovars;
        std::vector<model::Trk> trks = azs_db->get_Trks_with_all(&s_size,&tovars,&tanks);
        std::string price;
        for (int i = 0; i < trks.size(); i++) {
            // p[i].show();
            float scale = trks[i].scale;//(float)(3 * p[i].scale)
            std::string s = std::to_string(scale);
            ctx["screen_width"]=s_size.width;
            ctx["pump"][i] = { { "id", trks[i].id_trk }, { "x_pos", (float)trks[i].x_pos/s_size.width*100.0 }, { "y_pos", (float)trks[i].y_pos/s_size.width*100.0 }, { "scale", s }, { "pist", "" } };
            for (int j = 0; j < trks[i].pists.size(); j++) {
                price = std::to_string(trks[i].pists[j].tank_->tovar_->price);
                int pos = price.find(".");
                price.resize(pos + 3);
                ctx["pump"][i]["pist"][j] = { { "id_pist", trks[i].pists[j].id_pist }, { "name", trks[i].pists[j].tank_->tovar_->name }, { "price", price }, { "r", trks[i].pists[j].tank_->tovar_->color.r }, { "g", trks[i].pists[j].tank_->tovar_->color.g }, { "b", trks[i].pists[j].tank_->tovar_->color.b } };
            }
        }
        std::cout << "P: " << ctx.dump() << "\n";
        auto render = page.render(ctx);
        res.write(render.body_);
        res.end();
    }
  
    void api_out(crow::request& req, crow::response& res)
    {
        res.redirect("/");
        res.add_header("Set-Cookie", "refresh_token=;path=/;");
        res.end();
    }
    void api_outshift(crow::request& req, crow::response& res)
    {
        azs_db->smena_close();
        res.redirect("/");
        res.add_header("Set-Cookie", "refresh_token=;path=/;");
        res.end();
    }
    void api_settings_get(crow::request& req, crow::response& res){
        res.redirect("/");
        res.add_header("Set-Cookie", "refresh_token=;path=/;");
        res.end();
    }
    void api_settings_tanks_get(crow::request& req, crow::response& res){
        res.redirect("/");
        res.add_header("Set-Cookie", "refresh_token=;path=/;");
        res.end();
    }
    void api_settings_tovars_get(crow::request& req, crow::response& res){
        res.redirect("/");
        res.add_header("Set-Cookie", "refresh_token=;path=/;");
        res.end();
    }
    void api_settings_trks_get(crow::request& req, crow::response& res){
        res.redirect("/");
        res.add_header("Set-Cookie", "refresh_token=;path=/;");
        res.end();
    }
    void api_settings_kasa_get(crow::request& req, crow::response& res){
        res.redirect("/");
        res.add_header("Set-Cookie", "refresh_token=;path=/;");
        res.end();
    }
    void main_settings(crow::request& req, crow::response& res)
    {

        res.set_header("Content-Type", "text/html");
        auto page = crow::mustache::load("public/old/settings_azs.html");
        auto render = page.render();
        res.write(render.body_);
        res.end();
    }
    void main_settings_config(crow::request& req, crow::response& res)
    {

        res.redirect("/");
        res.add_header("Set-Cookie", "refresh_token=;path=/;");
        res.end();
    }
    
    
   void api_pump_save(crow::request& req, crow::response& res){
        res.redirect("/main");
        res.end();
    }
    
};
extern Admin admin;
extern User user;