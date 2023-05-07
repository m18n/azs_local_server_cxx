#include<iostream>
#include"crow.h"
int main(){
    std::cout<<"START PROGRAM\n";
    crow::SimpleApp app;
    crow::mustache::set_global_base("site");
    CROW_ROUTE(app, "/")
    ([]() {
        crow::mustache::context ctx;
        return crow::mustache::load("login.html").render();
        
    });
    CROW_ROUTE(app, "/public/<path>")
    ([](const crow::request&, crow::response& res,std::string path) {
        res.set_static_file_info("site/"+path);
        res.end();
    });
    CROW_ROUTE(app, "/auth")
      .methods("POST"_method)([](const crow::request& req) {
          auto x = crow::json::load(req.body);
          if (!x)
              return crow::response(400);
          std::string login=(std::string)x[0]["value"];
          std::string password=(std::string)x[1]["value"];
          std::cout<<"LOGIN: "<<login<<" PASSWORD: "<<password<<"\n";
          crow::json::wvalue ret({{"status", "not"}});
          if(login=="root"&&password=="123"){
            ret["status"]="yes";
          }
          return crow::response{ret};
        
    });
    app.port(3000).run();

    std::cin.get();
    std::cin.get();
    return 0;
}
    