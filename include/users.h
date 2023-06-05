#pragma once
#include "web_tehnology.h"
#include"string"
#include"string.h"
#define URL_MAIN "/main"
#define API_PUMP_SAVESCALE "/api/pump/savescale"
class Client {
private:
protected:
    virtual void pump(crow::request& req, crow::response& res) = 0;
    virtual void pump_savescale(crow::request& req, crow::response& res) = 0;
    void BaseController(std::string url, crow::request& req, crow::response& res)
    {
        if (url == URL_MAIN) {
            pump(req, res);
        } else if (url == API_PUMP_SAVESCALE) {
            pump_savescale(req, res);
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
        auto render = page.render();
        res.write(render.body_);
        res.end();
    }
    void pump_savescale(crow::request& req, crow::response& res)
    {
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
        crow::mustache::context ctx = { { "admin", false }, { "pump", "" } };
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
        res.set_header("Content-Type", "application/json");
        crow::json::wvalue ret({ { "status", "yes" } });
        crow::json::wvalue x = crow::json::load(req.body);
        std::cout<<"X: "<<x.dump()<<"\n";
        int32_t idpump = std::stoi(x["id"].dump(true));
        float scale = std::stof(x["scale"].dump(true));
        std::cout << "idpump: " << idpump << " scale: " << scale << "\n";
        azs_db->save_pump_scale(idpump,scale);
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
extern Admin admin;
extern User user;