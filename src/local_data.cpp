#include"local_data.h"
std::string generate_secret_key(int length) {
  std::string secret_key;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 35);
  const std::string chars = "abcdefghijklmnopqrstuvwxyz0123456789";
  for (int i = 0; i < length; ++i) {
    secret_key += chars[dis(gen)];
  }
  return secret_key;
}