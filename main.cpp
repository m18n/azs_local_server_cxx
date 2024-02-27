#include <iostream>
#include "azs.h"

int main() {
  try {
    azs web;
    web.start();
  } catch (const sql::SQLException& error) {
    std::cerr << "MySQL error: " << error.what() << std::endl;
    return 1;
  }

  return 0;
}
