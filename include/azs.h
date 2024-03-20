#pragma once
#include "local_data.h"
#include "view.h"

class azs {
private:
    crow::App<AuthVerefy,DatabaseVerefy,crow::CORSHandler> app;
    local_data ld;
    mysql_conn_info mysql_info;
    model::Azs_Database azs_db;
public:
    
    azs()
    {
       
        //init crow
        init_view_login(app);
        init_web(&azs_db,&ld);
        crow::mustache::set_global_base("azs_site");
        crow::mustache::set_base("azs_site");
        
        //init db
        azs_db.initlocaldata(&ld);
        mysql_info=ld.get_mysql_conn_info();
        mysql_info.show();
        azs_db.setfirst_infomysql(mysql_info);
        bool conn=azs_db.connect(mysql_info);
        if(!conn){
            std::cout<<"DONT CONNECT\n";
        }
    }
    void start()
    {
        std::cout << "START SERVER\n";
        app.port(3000).run();
    }
};