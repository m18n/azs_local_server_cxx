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

    app.port(3000).run();

    std::cin.get();
    std::cin.get();
    return 0;
}
    