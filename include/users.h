#pragma once
#include "web_tehnology.h"
#include"string"
#include"string.h"
#define URL_MAIN "/main"
#define API_PUMP_SAVESCALE "/api/pump/savescale"
#define API_PUMP_SAVEXY "/api/pump/savexy"
class Client {
private:
protected:
    virtual void pump(crow::request& req, crow::response& res) = 0;
    virtual void pump_savescale(crow::request& req, crow::response& res) = 0;
    virtual void pump_savexy(crow::request& req, crow::response& res) = 0;
    void BaseController(std::string url, crow::request& req, crow::response& res)
    {
        if (url == URL_MAIN) {
            pump(req, res);
        } else if (url == API_PUMP_SAVESCALE) {
            pump_savescale(req, res);
        }else if (url == API_PUMP_SAVEXY) {
            pump_savexy(req, res);
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
        for (int i = 0; i < p.size(); i++) {
            float scale=(float)(300.0 / 100.0 * p[i].scale);
            std::string s=std::to_string(scale);
            ctx["pump"][i] = { { "id", p[i].id_trk }, { "x_pos", p[i].x_pos }, { "y_pos", p[i].y_pos }, { "scale", s } };
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
        std::cout<<"json: "<<json.dump()<<"\n";
        int32_t idpump = std::stoi(json["id"].dump(true));
        float scale = std::stof(json["scale"].dump(true));
        std::cout << "idpump: " << idpump << " scale: " << scale << "\n";
        azs_db->save_pump_scale(idpump,scale);
        res.body = ret.dump();
        std::cout<<res.body<<"\n";
        res.end();
    }
     void pump_savexy(crow::request& req, crow::response& res)
    {
         res.set_header("Content-Type", "application/json");
        crow::json::wvalue ret({ { "status", "yes" } });
        crow::json::wvalue json = crow::json::load(req.body);
        std::cout<<"json: "<<json.dump()<<"\n";
        int32_t idpump = std::stoi(json["id"].dump(true));
        int32_t x = std::stoi(json["x"].dump(true));
        int32_t y = std::stoi(json["y"].dump(true));
        azs_db->save_pump_xy(idpump,x,y);
        res.body = ret.dump();
        std::cout<<res.body<<"\n";
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
        crow::mustache::context ctx = {  { "pump", "" } };
        std::vector<model::pump> p = azs_db->get_pump();
        for (int i = 0; i < p.size(); i++) {
            float scale=(float)(300.0 / 100.0 * p[i].scale);
            std::string s=std::to_string(scale);
            ctx["pump"][i] = { { "id", p[i].id_trk }, { "x_pos", p[i].x_pos }, { "y_pos", p[i].y_pos }, { "scale", s } };
        }
        std::cout << "P: " << ctx.dump() << "\n";
        auto render = page.render(ctx);
        res.write(render.body_);
        res.end();
    }
    void pump_savescale(crow::request& req, crow::response& res)
    {
        //id,scale
        res.redirect("/main");
        res.end();
    }
    void pump_savexy(crow::request& req, crow::response& res)
    {
        //id,scale
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