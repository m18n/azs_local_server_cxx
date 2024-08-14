#include"web_tehnology.h"
model::Azs_Database* azs_db;
local_data* ld;
void init_web(model::Azs_Database* azs_database, local_data* local){
    azs_db = azs_database;
    ld = local;
}

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
                            .allow_algorithm(jwt::algorithm::hs256 { ld->get_secret_jwt() });

        verifier.verify(decoded_token);

        return true;
    } catch (const std::exception& ex) {
        std::cerr << "Token verification failed: " << ex.what() << std::endl;
        return false;
    }
}

std::string create_token(crow::json::wvalue json)
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
    
    std::string token_string = token.sign(jwt::algorithm::hs256 { ld->get_secret_jwt() });
    return token_string;
}
std::string create_token()
{
    auto token = jwt::create();
    std::string time_now = timetostr(std::chrono::system_clock::now());
    token.set_type("JWS");
    token.set_payload_claim("time_create", jwt::claim(time_now));

    std::string token_string = token.sign(jwt::algorithm::hs256 { ld->get_secret_jwt() });
    std::cout << "TOKEN: " << token_string << "\n";
    return token_string;
}