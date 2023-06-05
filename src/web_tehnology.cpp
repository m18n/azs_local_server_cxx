#include"web_tehnology.h"
model::azs_database* azs_db;
local_data* ld;
std::string jwt_secret;
void init_web(model::azs_database* azs_database, local_data* local){
    azs_db = azs_database;
    ld = local;
    jwt_secret = ld->get_secret_jwt();
}
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
            if(i == cookie.length() - 1&&cookie[i] != '='){
                size_record++;
            }
            
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
            } else {
                if (cookie[i] == '=') {
                    memcpy(&parse[index_parse], ":\"\"", 3);
                    index_parse += 3;
                }
            }

            index_last_record = i + 1;
        } else if (cookie[i] != ' ' && mark == true) {

            index_last_record = i;
            mark = false;
        }
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
bool verify_token(std::string token, crow::json::wvalue& jpayload,std::string jwt_secret)
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

std::string create_token(crow::json::wvalue json,std::string jwt_secret)
{
    auto token = jwt::create();
    std::string time_now = timetostr(std::chrono::system_clock::now());
    token.set_type("JWS");
    token.set_payload_claim("time_create", jwt::claim(time_now));

    std::vector<std::string> keys = json.keys();
    for (int i = 0; i < keys.size(); i++) {
        std::string value = json[keys[i]].dump(true);
        token.set_payload_claim(keys[i], jwt::claim(value));
    }
    std::string token_string = token.sign(jwt::algorithm::hs256 { jwt_secret });
    return token_string;
}
std::string create_token(std::string jwt_secret)
{
    auto token = jwt::create();
    std::string time_now = timetostr(std::chrono::system_clock::now());
    token.set_type("JWS");
    token.set_payload_claim("time_create", jwt::claim(time_now));

    std::string token_string = token.sign(jwt::algorithm::hs256 { jwt_secret });
    std::cout << "TOKEN: " << token_string << "\n";
    return token_string;
}