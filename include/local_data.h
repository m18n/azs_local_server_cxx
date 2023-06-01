#pragma once
#include "sqlite3.h"
#include <iostream>
struct mysql_conn_info {
    std::string ip;
    std::string name;
    std::string password;
    void show()
    {
        std::cout << "ip: " << ip << " name: " << name << " password: " << password << "\n";
    }
};
class local_data {
private:
    sqlite3* db;
    sqlite3_stmt* stmt;
    char* errMsg = NULL;

public:
    local_data()
    {
        int rc = sqlite3_open("azs.db", &db);
        if (rc) {
            std::cout << "Can't open local data: %s\n"
                      << sqlite3_errmsg(db);
        } else {
            std::cout << "local data open\n";
        }
        std::string sql = "CREATE TABLE IF NOT EXISTS mysql_db ("
                          "id INTEGER PRIMARY KEY,"
                          "ip TEXT NOT NULL,"
                          "login TEXT NOT NULL,"
                          "password TEXT NOT NULL"
                          ");";
        rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
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
            data.ip = (char*)sqlite3_column_text(stmt, 1);
            data.name = (char*)sqlite3_column_text(stmt, 2);
            data.password = (char*)sqlite3_column_text(stmt, 3);
        } else {
            std::cout << "ERROR MYSQL CONN DATA:" << ret << "\n";
        }
        sqlite3_finalize(stmt);
        stmt = NULL;
        return data;
    }
    std::string get_secret_jwt()
    {
        std::string secret = "";
        std::string sql = "SELECT * FROM jwt";
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
            std::cout << "ERROR: while compiling sql\n";
            sqlite3_finalize(stmt);
        }
        int ret = 0;
        if ((ret = sqlite3_step(stmt)) == SQLITE_ROW) {
            secret = (char*)sqlite3_column_text(stmt, 1);

        } else {
            std::cout << "ERROR MYSQL CONN DATA:" << ret << "\n";
        }
        sqlite3_finalize(stmt);
        stmt = NULL;
        return secret;
    }
    ~local_data()
    {
        if (stmt != NULL)
            sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
};