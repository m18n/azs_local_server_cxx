#pragma once
#include "sqlite3.h"
#include "string"
#include <fstream>
#include <iostream>
#include <random>
struct mysql_conn_info {
    std::string ip;
    std::string name;
    std::string password;
    std::string database;
    std::string port;
    void show()
    {
        std::cout << "ip: " << ip << " name: " << name << " password: " << password << " Database: " << database << " PORT: " << port << "\n";
    }
     bool operator==(const mysql_conn_info& other) const {
        return ip == other.ip && name == other.name && password == other.password && database == other.database && port == other.port;
    }
    bool operator!=(const mysql_conn_info& other) const {
        return !(*this == other);
    }
};
std::string generate_secret_key(int length);
class local_data {
private:
    sqlite3* db;
    sqlite3_stmt* stmt;
    int id_last = -1;
    char* errMsg = NULL;
    std::string secretjwt = "";
    bool open_localdatabase()
    {
        int rc = sqlite3_open("azs.db", &db);
        if (rc) {
            const char* filename = "azs.db";
            std::ifstream ifile(filename);
            bool file_exists = ifile.good();
            ifile.close();
            return false;
        } else {
            std::cout << "local data open\n";
            return true;
        }
    }

public:
    local_data()
    {

        std::cout << "OPEN LOCAL DATABASE\n";
        while (!open_localdatabase()) {
        }

        std::string sql = "CREATE TABLE IF NOT EXISTS mysql_db ("
                          "id INTEGER PRIMARY KEY,"
                          "ip TEXT NOT NULL,"
                          "login TEXT NOT NULL,"
                          "password TEXT NOT NULL,"
                          "database TEXT NOT NULL,"
                          "port TEXT NOT NULL"
                          ");";
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "Error open/creating table: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        }
        sql = "CREATE TABLE IF NOT EXISTS jwt ("
              "id INTEGER PRIMARY KEY,"
              "jwt TEXT NOT NULL"
              ");";
        rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "Error open/creating table: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        }
        std::cout << "Table open\n";
    }
    bool set_mysql_conn_info(mysql_conn_info mysql_info)
    {
        if (id_last == -1) {
            std::cout << "INSERT\n";
            std::string sql = "INSERT INTO mysql_db (ip,login,password,database,port) VALUES (\"" + mysql_info.ip + "\",\"" + mysql_info.name + "\",\"" + mysql_info.password + "\",\"" + mysql_info.database + "\",\"" + mysql_info.port + "\");";
            char* errmsg = 0;
            int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errmsg);
            if (rc != SQLITE_OK) {
                std::cout << "ERROR SET MYSQL DB: " << errmsg << "\n";
                return false;
            }
        } else {
            std::cout << "UPDATE\n";
            std::string sql = "UPDATE mysql_db SET ip=\"" + mysql_info.ip + "\", login=\"" + mysql_info.name + "\",password=\"" + mysql_info.password + "\",database=\"" + mysql_info.database + "\",port=\"" + mysql_info.port + "\" WHERE id=" + std::to_string(id_last);
            char* errmsg = 0;
            int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errmsg);
            if (rc != SQLITE_OK) {
                std::cout << "ERROR SET MYSQL DB:" << errmsg << "\n";
                return false;
            }
        }
        return true;
    }
    mysql_conn_info get_mysql_conn_info()
    {
        mysql_conn_info data;
        std::string sql = "SELECT * FROM mysql_db";
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
            std::cout << "ERROR: while compiling sql\n";
            sqlite3_finalize(stmt);
        }
        int ret = 0;
        if ((ret = sqlite3_step(stmt)) == SQLITE_ROW) {
            id_last = sqlite3_column_int(stmt, 0);
            data.ip = (char*)sqlite3_column_text(stmt, 1);
            data.name = (char*)sqlite3_column_text(stmt, 2);
            data.password = (char*)sqlite3_column_text(stmt, 3);
            data.database = (char*)sqlite3_column_text(stmt, 4);
            data.port = (char*)sqlite3_column_text(stmt, 5);
        } else {
            std::cout << "NON DATA IN MYSQL_DB" << ret << "\n";
        }
        sqlite3_finalize(stmt);
        stmt = NULL;
        return data;
    }
    std::string generatejwt_secret(bool reginerate)
    {
        std::string secret = generate_secret_key(15);
        int idsecret = -1;
        std::string sql = "SELECT * FROM jwt";
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
            std::cout << "ERROR: while compiling sql\n";
            sqlite3_finalize(stmt);
            return "";
        }
        int ret = 0;
        if ((ret = sqlite3_step(stmt)) == SQLITE_ROW) {
            idsecret = sqlite3_column_int(stmt, 0);
            if (reginerate == true) {
                sql = "UPDATE jwt SET jwt=\"" + secret + "\" WHERE id=" + std::to_string(idsecret);
                char* errmsg = 0;
                int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errmsg);
                if (rc != SQLITE_OK) {
                    std::cout << "ERROR SET MYSQL DB:" << errmsg << "\n";
                    return "";
                }
            }else{
                secret = (char*)sqlite3_column_text(stmt, 1);
            }
            sqlite3_finalize(stmt);
        } else {

            sql = "INSERT INTO jwt (jwt) VALUES (\"" + secret + "\");";
            char* errmsg = 0;
            int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errmsg);
            if (rc != SQLITE_OK) {
                std::cout << "ERROR JWT GENERATE: " << errmsg << "\n";
                return "";
            }
            sqlite3_finalize(stmt);
        }

        this->secretjwt = secret;
        std::cout<<"SECRET JWT: "<<this->secretjwt<<"\n";
        return secret;
    }
    void setjwt_secret()
    {
    }
    std::string get_secret_jwt()
    {

        if (this->secretjwt == "") {
            generatejwt_secret(false);
        }
        std::cout << "GEN " << secretjwt << "\n";
        return secretjwt;
    }
    ~local_data()
    {
        if (stmt != NULL)
            sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
};