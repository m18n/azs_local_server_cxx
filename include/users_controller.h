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
    }
    virtual void main_pump(crow::request& req, crow::response& res) = 0;
    virtual void main_settings(crow::request& req, crow::response& res) = 0;
    virtual void main_settings_config(crow::request& req, crow::response& res) = 0;
    virtual void api_pump_save(crow::request& req, crow::response& res) = 0;
    virtual void api_out(crow::request& req, crow::response& res) = 0;
    virtual void api_outshift(crow::request& req, crow::response& res) = 0;

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
        auto page = crow::mustache::load("serv.html");
        crow::mustache::context ctx = { { "admin", true }, { "pump", "" } };
        model::screen_size s_size;
        std::vector<model::pump> p = azs_db->get_pump(&s_size);
        std::string price;
        for (int i = 0; i < p.size(); i++) {
            // p[i].show();
            float scale = p[i].scale;//(float)(3 * p[i].scale)
            std::string s = std::to_string(scale);
            ctx["screen_width"]=s_size.width;
            ctx["pump"][i] = { { "id", p[i].id_trk }, { "x_pos", (float)p[i].x_pos/s_size.width*100.0 }, { "y_pos", (float)p[i].y_pos/s_size.width*100.0 }, { "scale", s }, { "pist", "" } };
            for (int j = 0; j < p[i].pists.size(); j++) {
                price = std::to_string(p[i].pists[j].tank_.tovar_.price);
                int pos = price.find(".");
                price.resize(pos + 3);
                ctx["pump"][i]["pist"][j] = { { "id_pist", p[i].pists[j].id_pist }, { "name", p[i].pists[j].tank_.tovar_.name }, { "price", price }, { "r", p[i].pists[j].tank_.rgb.r }, { "g", p[i].pists[j].tank_.rgb.g }, { "b", p[i].pists[j].tank_.rgb.b } };
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
        std::vector<model::pump> pumps;
        pumps.resize(json["objects"].size());
        std::cout << "json: " << json.dump() << "\n";
        std::cout<<"OBJECT SIZE: "<<json["objects"].size();
        for(int i=0;i<json["objects"].size();i++){
            pumps[i].id_trk=std::stoi(json["objects"][i]["id"].dump(true));
            std::string xstr=json["objects"][i]["x"].dump(true);
            std::string ystr=json["objects"][i]["y"].dump(true);
            if(xstr=="")
                xstr="0";
            if(ystr=="")
                ystr="0";
            float x_f=std::stof(xstr);
            float y_f=std::stof(ystr);
            pumps[i].x_pos=int(screen_width/(100/x_f));
            pumps[i].y_pos=int(screen_width/(100/y_f));
            pumps[i].scale=std::stof(json["objects"][i]["scale"].dump(true));
            pumps[i].show();
        }
        azs_db->save_pump(std::move(pumps),screen_width,screen_heigth);
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
    
    void main_settings(crow::request& req, crow::response& res)
    {
        crow::mustache::context ctx = { { "admin", true } };
        res.set_header("Content-Type", "text/html");
        auto page = crow::mustache::load("settings_azs.html");
        auto render = page.render(ctx);
        res.write(render.body_);
        res.end();
    }
    void main_settings_config(crow::request& req, crow::response& res){
        model::screen_size screen;
        std::vector<model::tank> tanks;
        auto pumps=azs_db->get_pump(&screen,tanks);
        pumps[0].show();
        crow::mustache::context ctx = {};
        
        
        for(int i=0;i<pumps.size();i++){
            ctx["pumps"][i]={{"id_trk",pumps[i].id_trk}};

            for(int j=0;j<pumps[i].pists.size();j++){

                
                std::string color=std::to_string(pumps[i].pists[j].tank_.rgb.r)+","+std::to_string(pumps[i].pists[j].tank_.rgb.g)+","+std::to_string(pumps[i].pists[j].tank_.rgb.b);
                ctx["pumps"][i]["pists"][j]={{"id_pist",pumps[i].pists[j].id_pist},{"id_tank",pumps[i].pists[j].tank_.id_tank},{"color",color},
                {"id_tovar",pumps[i].pists[j].tank_.tovar_.id_tovar},{"name",pumps[i].pists[j].tank_.tovar_.name},
                {"price",pumps[i].pists[j].tank_.tovar_.price}};
            }
           
            
        }
        for(int i=0;i<tanks.size();i++){
            std::string color=std::to_string(tanks[i].rgb.r)+","+std::to_string(tanks[i].rgb.g)+","+std::to_string(tanks[i].rgb.b);
            ctx["tanks"][i]={{"id_tank",tanks[i].id_tank},{"color",color},{"id_tovar",tanks[i].tovar_.id_tovar},{"name",tanks[i].tovar_.name},{"price",tanks[i].tovar_.price}};
        }
        res.set_header("Content-Type", "text/html");
        auto page = crow::mustache::load("configuration.html");
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
        auto page = crow::mustache::load("serv.html");
        crow::mustache::context ctx = { { "pump", "" } };
        model::screen_size s_size;
        std::vector<model::pump> p = azs_db->get_pump(&s_size);
        std::string price;
        for (int i = 0; i < p.size(); i++) {
            // p[i].show();
            float scale = p[i].scale;//(float)(3 * p[i].scale)
            std::string s = std::to_string(scale);
            ctx["screen_width"]=s_size.width;
            ctx["pump"][i] = { { "id", p[i].id_trk }, { "x_pos", (float)p[i].x_pos/s_size.width*100.0 }, { "y_pos", (float)p[i].y_pos/s_size.width*100.0 }, { "scale", s }, { "pist", "" } };
            for (int j = 0; j < p[i].pists.size(); j++) {
                price = std::to_string(p[i].pists[j].tank_.tovar_.price);
                int pos = price.find(".");
                price.resize(pos + 3);
                ctx["pump"][i]["pist"][j] = { { "id_pist", p[i].pists[j].id_pist }, { "name", p[i].pists[j].tank_.tovar_.name }, { "price", price }, { "r", p[i].pists[j].tank_.rgb.r }, { "g", p[i].pists[j].tank_.rgb.g }, { "b", p[i].pists[j].tank_.rgb.b } };
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
    void main_settings(crow::request& req, crow::response& res)
    {

        res.set_header("Content-Type", "text/html");
        auto page = crow::mustache::load("settings_azs.html");
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