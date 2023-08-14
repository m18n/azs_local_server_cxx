#pragma once
#include "string"
#include "string.h"
#include "web_tehnology.h"
#define URL_MAIN "/main"
#define API_PUMP_SAVE "/api/pump/save"
#define SETTINGS_MAIN "/main/settings"
#define API_OUT "/api/out"
#define API_OUTSHIFT "/api/outshift"

class Client {
private:
protected:
    virtual void pump(crow::request& req, crow::response& res) = 0;
    virtual void pump_save(crow::request& req, crow::response& res) = 0;
    virtual void settings_main(crow::request& req, crow::response& res) = 0;
    virtual void out(crow::request& req, crow::response& res) = 0;
    virtual void outshift(crow::request& req, crow::response& res) = 0;
    void BaseController(std::string url, crow::request& req, crow::response& res)
    {
        if (url == URL_MAIN) {
            pump(req, res);
        }else if (url == API_PUMP_SAVE) {
            pump_save(req, res);
        } else if (url == SETTINGS_MAIN) {
            settings_main(req, res);
        }else if (url == API_OUT) {
            out(req, res);
        } else if (url == API_OUTSHIFT) {
            outshift(req, res);
        }
    }

public:
    virtual void Controller(std::string url, crow::request& req, crow::response& res)
    {
        BaseController(url, req, res);
    }
};
class Admin : public Client {
private:
    

public:
    virtual void Controller(std::string url, crow::request& req, crow::response& res)
    {
        BaseController(url, req, res);
    }
    void pump(crow::request& req, crow::response& res)
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
    
    void pump_save(crow::request& req, crow::response& res){
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
    
    void out(crow::request& req, crow::response& res)
    {
        res.redirect("/");
        res.add_header("Set-Cookie", "refresh_token=;path=/;");
        res.end();
    }
    void outshift(crow::request& req, crow::response& res)
    {
        azs_db->smena_close();
        res.redirect("/");
        res.add_header("Set-Cookie", "refresh_token=;path=/;");
        res.end();
    }
    
    void settings_main(crow::request& req, crow::response& res)
    {
        crow::mustache::context ctx = { { "admin", true } };
        res.set_header("Content-Type", "text/html");
        auto page = crow::mustache::load("configurationazs.html");
        auto render = page.render(ctx);
        res.write(render.body_);
        res.end();
    }
};
class User : public Client {
private:
    

public:
    virtual void Controller(std::string url, crow::request& req, crow::response& res)
    {
        BaseController(url, req, res);
    }
    void pump(crow::request& req, crow::response& res)
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
  
    void out(crow::request& req, crow::response& res)
    {
        res.redirect("/");
        res.add_header("Set-Cookie", "refresh_token=;path=/;");
        res.end();
    }
    void outshift(crow::request& req, crow::response& res)
    {
        azs_db->smena_close();
        res.redirect("/");
        res.add_header("Set-Cookie", "refresh_token=;path=/;");
        res.end();
    }
    void settings_main(crow::request& req, crow::response& res)
    {

        res.set_header("Content-Type", "text/html");
        auto page = crow::mustache::load("configurationazs.html");
        auto render = page.render();
        res.write(render.body_);
        res.end();
    }
    
   void pump_save(crow::request& req, crow::response& res){
        res.redirect("/main");
        res.end();
    }
    
};
extern Admin admin;
extern User user;