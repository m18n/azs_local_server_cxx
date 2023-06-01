#include "controller.h"
azs_database* azs_db;
local_data* ld;
std::string jwt_secret;
std::string ParseCookie(std::string cookie)
{
    int size_parse = 2;
    int element_not_parse = 0;
    int count_element_pars = 0;
    std::string parse;
    bool mark = false;
    for (int i = 0; i < cookie.length(); i++) {
        if (cookie[i] == ';') {
            mark = true;
        } else if (cookie[i] == '=') {
            count_element_pars++;
        } else if (cookie[i] != ' ') {
            mark = false;
        } else if (cookie[i] == ' ' && mark == true) {
            element_not_parse++;
        }
    }
    size_parse += cookie.size() + 4 * count_element_pars - element_not_parse;
    parse.resize(size_parse);
    int index_last_record = 0;
    int index_parse = 1;
    parse[0] = '{';
    mark = false;
    for (int i = 0; i < cookie.length(); i++) {

        if (cookie[i] == '=' || cookie[i] == ';' || i == cookie.length() - 1) {
            // if (i == cookie.length() - 1) {
            //     i++;
            // }
            parse[index_parse] = '\"';
            index_parse++;
            int size_record = i - index_last_record;
            memcpy(&parse[index_parse], &cookie[index_last_record], size_record);
            index_parse += size_record;
            parse[index_parse] = '\"';
            index_parse++;
            if (i != cookie.length() - 1) {
                if (cookie[i] == '=') {
                    parse[index_parse] = ':';
                    index_parse++;
                } else {
                    parse[index_parse] = ',';
                    index_parse++;
                    mark = true;
                }
            }

            index_last_record = i + 1;
        } else if (cookie[i] != ' ' && mark == true) {

            index_last_record = i;
            mark = false;
        }
    }
    if (cookie[index_last_record - 1] == '=') {
        memcpy(&parse[index_parse], ":\"\"", 3);
        index_parse += 3;
    }
    parse[index_parse] = '}';

    return parse;
}
std::string timetostr(std::chrono::system_clock::time_point time)
{
    auto time_time_t = std::chrono::system_clock::to_time_t(time);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_time_t), "%Y-%m-%d %H:%M:%S");
    std::string str = ss.str();
    return str;
}
std::chrono::system_clock::time_point strtotime(std::string time)
{
    std::tm tm = {};

    std::istringstream ss(time);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    tm.tm_isdst = -1;
    std::time_t t = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(t);
}
// crow::json::wvalue mapToCrowJson(std::map<std::string, picojson::value> map){
//     crow::json::wvalue res;

//      for(auto& e : map)
//      {
//         res[e.first]=e.second.to_str();
//      }
//      return res;
// }
bool verify_token(std::string token, crow::json::wvalue& jpayload)
{
    try {
        if (token[0] == '\"') {
            token.erase(0, 1);
            token.resize(token.size() - 1);
        }
        auto decoded_token = jwt::decode(token);
        jpayload = crow::json::load(decoded_token.get_payload());
        auto verifier = jwt::verify()
                            .allow_algorithm(jwt::algorithm::hs256 { jwt_secret });

        verifier.verify(decoded_token);

        return true;
    } catch (const std::exception& ex) {
        std::cerr << "Token verification failed: " << ex.what() << std::endl;
        return false;
    }
}

void AuthVerefy::before_handle(crow::request& req, crow::response& res, context& /*ctx*/)
{
    std::string cookie = req.get_header_value("Cookie");
    std::string parse = ParseCookie(cookie);
    std::cout << "PARSE: " << parse << "\n";
    crow::json::wvalue jcookie = crow::json::load(parse);
    crow::json::wvalue payload;
    if (jcookie["refresh_token"].dump() == "null"||jcookie["refresh_token"].dump() == "\"\"") {
        res.redirect("/");
        res.end();
        return;
    }
    if (verify_token(jcookie["refresh_token"].dump(), payload)) {

        std::string time = payload["time_create"].dump();
        time.erase(0, 1);
        time.resize(time.size() - 1);
        auto t = strtotime(time);
        auto now = std::chrono::system_clock::now();
        auto duration = now - t;
        int minutes = std::chrono::duration_cast<std::chrono::minutes>(duration).count();
        std::cout << "REFRESH TOKEN: " << minutes << "\n";
        if (minutes > 10) {
            res.redirect("/");
            res.add_header("Set-Cookie", "refresh_token=");
            res.end();
            return;
        }
    }

    
}

void AuthVerefy::after_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/)
{
}
void init_controller(azs_database* azs_database, local_data* local)
{
    azs_db = azs_database;
    ld = local;
    jwt_secret = ld->get_secret_jwt();
}
crow::mustache::rendered_template auth_main()
{
    std::vector<user_name> res = azs_db->get_user_name();
    crow::mustache::context ctx = { { "users", "" } };
    for (int i = 0; i < res.size(); i++) {
        ctx["users"][i] = { { "id", res[i].id }, { "name", res[i].name } };
    }
    std::cout << ctx.dump() << "\n";
    crow::mustache::rendered_template t = crow::mustache::load("login.html").render(ctx);
    return t;
}
std::string create_token(crow::json::wvalue json)
{
    auto token = jwt::create();
    std::string time_now = timetostr(std::chrono::system_clock::now());
    token.set_type("JWS");
    token.set_payload_claim("time_create", jwt::claim(time_now));

    std::vector<std::string> keys = json.keys();
    for (int i = 0; i < keys.size(); i++) {
        std::string value = json[keys[i]].dump();
        value.erase(0, 1);
        value.resize(value.size() - 1);
        token.set_payload_claim(keys[i], jwt::claim(value));
    }
    std::string token_string = token.sign(jwt::algorithm::hs256 { jwt_secret });
    return token_string;
}
std::string create_token()
{
    auto token = jwt::create();
    std::string time_now = timetostr(std::chrono::system_clock::now());
    token.set_type("JWS");
    token.set_payload_claim("time_create", jwt::claim(time_now));

    std::string token_string = token.sign(jwt::algorithm::hs256 { jwt_secret });
    std::cout << "TOKEN: " << token_string << "\n";
    return token_string;
}
void auth(const crow::request& req, crow::response& res)
{
    crow::json::wvalue ret({ { "status", "not" } });
    auto x = crow::json::load(req.body);
    if (!x) {
        res.body = ret.dump();
        res.end();
        return;
    }
    int32_t userid = (int32_t)x[0]["value"];
    std::string password = (std::string)x[1]["value"];
    std::cout << "User: " << userid << " PASSWORD: " << password << "\n";
    if (azs_db->auth_check(userid, password)) {
        res.add_header("Set-Cookie", "refresh_token=" + create_token({ { "user_id", std::to_string(userid) } }));
        ret["status"] = "yes";
    }
    res.body = ret.dump();
    res.end();
}
void static_files(crow::response& res, std::string path)
{
    res.set_static_file_info("site/" + path);
    res.end();
}
crow::mustache::rendered_template pump()
{
    return crow::mustache::load("serv.html").render();
}