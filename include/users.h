#pragma once
#include "string"
#include "string.h"
#include "web_tehnology.h"
#define URL_MAIN "/main"
#define API_PUMP_SAVESCALE "/api/pump/savescale"
#define API_PUMP_SAVEXY "/api/pump/savexy"
#define SETTINGS_MAIN "/main/settings"
#define SETTINGS_DATABASE "/main/settings/database"
#define API_OUT "/api/out"
#define API_OUTSHIFT "/api/outshift"
class Client {
private:
protected:
    virtual void pump(crow::request& req, crow::response& res) = 0;
    virtual void pump_savescale(crow::request& req, crow::response& res) = 0;
    virtual void pump_savexy(crow::request& req, crow::response& res) = 0;
    virtual void settings_main(crow::request& req, crow::response& res) = 0;
    virtual void settings_database(crow::request& req, crow::response& res) = 0;
    virtual void out(crow::request& req, crow::response& res) = 0;
    virtual void outshift(crow::request& req, crow::response& res) = 0;
    void BaseController(std::string url, crow::request& req, crow::response& res)
    {
        if (url == URL_MAIN) {
            pump(req, res);
        } else if (url == API_PUMP_SAVESCALE) {
            pump_savescale(req, res);
        } else if (url == API_PUMP_SAVEXY) {
            pump_savexy(req, res);
        } else if (url == SETTINGS_MAIN) {
            settings_main(req, res);
        }else if (url == SETTINGS_DATABASE) {
            settings_database(req,res);
        }else if (url == API_OUT) {
            out(req,res);
        }else if(url==API_OUTSHIFT){
            outshift(req,res);
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
    void pump(crow::request& req, crow::response& res)
    {
        res.set_header("Content-Type", "text/html");
        auto page = crow::mustache::load("serv.html");
        crow::mustache::context ctx = { { "admin", true }, { "pump", "" } };
        std::vector<model::pump> p = azs_db->get_pump();
        std::string price;
        for (int i = 0; i < p.size(); i++) {
            // p[i].show();
            float scale = (float)(300.0 / 100.0 * p[i].scale);
            std::string s = std::to_string(scale);
            ctx["pump"][i] = { { "id", p[i].id_trk }, { "x_pos", p[i].x_pos }, { "y_pos", p[i].y_pos }, { "scale", s }, { "pist", "" } };
            for (int j = 0; j < p[i].pists.size(); j++) {
                price=std::to_string(p[i].pists[j].tank_.tovar_.price);
                int pos=price.find(".");
                price.resize(pos+3);
                ctx["pump"][i]["pist"][j] = { { "id_pist", p[i].pists[j].id_pist }, { "name", p[i].pists[j].tank_.tovar_.name }, { "price", price},{"r",p[i].pists[j].tank_.rgb.r},{"g",p[i].pists[j].tank_.rgb.g},{"b",p[i].pists[j].tank_.rgb.b} };
            }
        }
        std::cout << "P: " << ctx.dump() << "\n";
        auto render = page.render(ctx);
        res.write(render.body_);
        res.end();
    }
    void pump_savescale(crow::request& req, crow::response& res)
    {
        res.set_header("Content-Type", "application/json");
        crow::json::wvalue ret({ { "status", "yes" } });
        crow::json::wvalue json = crow::json::load(req.body);
        std::cout << "json: " << json.dump() << "\n";
        int32_t idpump = std::stoi(json["id"].dump(true));
        float scale = std::stof(json["scale"].dump(true));
        std::cout << "idpump: " << idpump << " scale: " << scale << "\n";
        azs_db->save_pump_scale(idpump, scale);
        res.body = ret.dump();
        std::cout << res.body << "\n";
        res.end();
    }
    void pump_savexy(crow::request& req, crow::response& res)
    {
        res.set_header("Content-Type", "application/json");
        crow::json::wvalue ret({ { "status", "yes" } });
        crow::json::wvalue json = crow::json::load(req.body);
        std::cout << "json: " << json.dump() << "\n";
        int32_t idpump = std::stoi(json["id"].dump(true));
        int32_t x = std::stoi(json["x"].dump(true));
        int32_t y = std::stoi(json["y"].dump(true));
        azs_db->save_pump_xy(idpump, x, y);
        res.body = ret.dump();
        std::cout << res.body << "\n";
        res.end();
    }
    void out(crow::request& req, crow::response& res){
        res.redirect("/");
        res.add_header("Set-Cookie", "refresh_token=;path=/;");
        res.end();
    }
    void outshift(crow::request& req, crow::response& res){
        azs_db->smena_close();
        res.redirect("/");
        res.add_header("Set-Cookie", "refresh_token=;path=/;");
        res.end();
    }
    void settings_database(crow::request& req, crow::response& res){
        res.set_header("Content-Type", "text/html");
        mysql_conn_info last=azs_db->get_last_info();
        crow::json::wvalue ctx={{"host",last.ip},{"user",last.name},{"password",last.password},{"database",last.database},{"port",last.port}};
        auto page = crow::mustache::load("settingsazs.html");
        auto render=page.render(ctx);
        res.write(render.body_);
        res.end();
    }
    void settings_main(crow::request& req, crow::response& res)
    {
        crow::mustache::context ctx = { { "admin", true }};
        res.set_header("Content-Type", "text/html");
        auto page = crow::mustache::load("configurationazs.html");
        auto render = page.render(ctx);
        res.write(render.body_);
        res.end();
    }

public:
    virtual void Controller(std::string url, crow::request& req, crow::response& res)
    {
        BaseController(url, req, res);
    }
};
class User : public Client {
private:
    void pump(crow::request& req, crow::response& res)
    {
        res.set_header("Content-Type", "text/html");
        auto page = crow::mustache::load("serv.html");
        crow::mustache::context ctx = { { "pump", "" } };
        std::vector<model::pump> p = azs_db->get_pump();
        std::string price;
        for (int i = 0; i < p.size(); i++) {
            // p[i].show();
            float scale = (float)(300.0 / 100.0 * p[i].scale);
            std::string s = std::to_string(scale);
            ctx["pump"][i] = { { "id", p[i].id_trk }, { "x_pos", p[i].x_pos }, { "y_pos", p[i].y_pos }, { "scale", s }, { "pist", "" } };
            for (int j = 0; j < p[i].pists.size(); j++) {
                price=std::to_string(p[i].pists[j].tank_.tovar_.price);
                int pos=price.find(".");
                price.resize(pos+3);
                ctx["pump"][i]["pist"][j] = { { "id_pist", p[i].pists[j].id_pist }, { "name", p[i].pists[j].tank_.tovar_.name }, { "price", price},{"r",p[i].pists[j].tank_.rgb.r},{"g",p[i].pists[j].tank_.rgb.g},{"b",p[i].pists[j].tank_.rgb.b} };
            }
        }
        std::cout << "P: " << ctx.dump() << "\n";
        auto render = page.render(ctx);
        res.write(render.body_);
        res.end();
    }
    void out(crow::request& req, crow::response& res){
        res.redirect("/");
        res.add_header("Set-Cookie", "refresh_token=;path=/;");
        res.end();
    }
    void outshift(crow::request& req, crow::response& res){
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
    void settings_database(crow::request& req, crow::response& res){
        res.redirect("/main");
        res.end();
    }
    void pump_savescale(crow::request& req, crow::response& res)
    {
        // id,scale
        res.redirect("/main");
        res.end();
    }
    void pump_savexy(crow::request& req, crow::response& res)
    {
        // id,scale
        res.redirect("/main");
        res.end();
    }

public:
    virtual void Controller(std::string url, crow::request& req, crow::response& res)
    {
        BaseController(url, req, res);
    }
};
extern Admin admin;
extern User user;