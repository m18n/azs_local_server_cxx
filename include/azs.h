#pragma once
#include "local_data.h"
#include "view.h"
class azs {
private:
    crow::SimpleApp app;
    local_data ld;
    mysql_conn_info mysql_info;
    azs_database azs_db;
public:
    azs()
    {
        //init crow
        init_view_login(app);
        init_controller(&azs_db);
        crow::mustache::set_global_base("site");
        //init db
        mysql_info=ld.get_mysql_conn_info();
        mysql_info.show();
        azs_db.connect(mysql_info.ip,mysql_info.name,mysql_info.password);
        
    }
    void start()
    {
        std::cout << "START SERVER\n";
        app.port(3000).run();
    }
};