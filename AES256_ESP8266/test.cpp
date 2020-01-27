#include <iostream>
#include <string>
#include <cstring>
#include "utilsP4F.h"

int main(){
  std::string test = "Password12345679";
  std::string passWord = "12345678";

  std::string encryptedStr = P4F::encrypt(test, passWord);
  std::string decryptedStr = P4F::decrypt(encryptedStr, passWord);

  std::cout << encryptedStr << "\n";
  std::cout << decryptedStr << "\n";

  return 0;
}