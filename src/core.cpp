#include"core.h"
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