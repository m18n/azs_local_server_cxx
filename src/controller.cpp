#include "controller.h"
azs_database* azs_db;
void init_controller(azs_database* azs_database){
    azs_db=azs_database;
}
crow::mustache::rendered_template auth_main() {
  std::vector<user_name> res=azs_db->get_user_name();
  crow::mustache::context ctx={{"users",""}};
  for(int i=0;i<res.size();i++){
    ctx["users"][i]={{"id",res[i].id},{"name",res[i].name}};
  }
  std::cout<<ctx.dump()<<"\n";
  return crow::mustache::load("login.html").render(ctx);
}
crow::response auth(const crow::request &req) {
  auto x = crow::json::load(req.body);
  if (!x){
    return crow::response(400);
  }
  int32_t userid = (int32_t)x[0]["value"];
  std::string password = (std::string)x[1]["value"];
  std::cout << "User: " << userid << " PASSWORD: " << password << "\n";
  crow::json::wvalue ret({{"status", "not"}});
  if (azs_db->auth_check(userid,password)) {
    ret["status"] = "yes";
  }
  return ret;
}
void static_files(crow::response &res, std::string path) {
  res.set_static_file_info("site/" + path);
  res.end();
}
crow::mustache::rendered_template pump(){
  return crow::mustache::load("serv.html").render();
}