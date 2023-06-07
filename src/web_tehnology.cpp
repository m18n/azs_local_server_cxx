#include"web_tehnology.h"
model::azs_database* azs_db;
local_data* ld;
std::string jwt_secret;
void init_web(model::azs_database* azs_database, local_data* local){
    azs_db = azs_database;
    ld = local;
    jwt_secret = ld->get_secret_jwt();
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