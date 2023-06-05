#pragma once
#include "sqlite3.h"
#include <fstream>
#include <iostream>
struct mysql_conn_info {
    std::string ip;
    std::string name;
    std::string password;
    std::string database;
    void show()
    {
        std::cout << "ip: " << ip << " name: " << name << " password: " << password << " Database: " << database << "\n";
    }
};
class local_data {
private:
    sqlite3* db;
    sqlite3_stmt* stmt;
    char* errMsg = NULL;
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
                          "database TEXT NOT NULL"
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
        std::string sql = "INSERT INTO mysql_db (ip,login,password,database) VALUES (\"" + mysql_info.ip + "\",\"" + mysql_info.name + "\",\"" + mysql_info.password + "\",\"" + mysql_info.database + "\");";
        char* errmsg = 0;
        int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errmsg);
        if (rc != SQLITE_OK) {
            std::cout << "ERROR: " << errmsg << "\n";
            return false;
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
            data.ip = (char*)sqlite3_column_text(stmt, 1);
            data.name = (char*)sqlite3_column_text(stmt, 2);
            data.password = (char*)sqlite3_column_text(stmt, 3);
            data.database = (char*)sqlite3_column_text(stmt, 4);
        } else {
            std::cout << "ERROR LOCAL BASE DATA:" << ret << "\n";
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
            std::cout << "ERROR LOCAL BASE CONN DATA:" << ret << "\n";
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