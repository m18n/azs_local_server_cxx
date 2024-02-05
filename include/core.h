#pragma once
#include <chrono>
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> 
#include <iostream>
#include <cstring>
std::string timetostr(std::chrono::system_clock::time_point time);
std::chrono::system_clock::time_point strtotime(std::string time);
std::string ParseCookie(std::string cookie);