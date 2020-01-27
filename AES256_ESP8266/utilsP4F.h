#ifndef _UTILS_P4F_
#define _UTILS_P4F_

#include <string>
#include <cstring>

namespace P4F{

  std::string encrypt(std::string message, std::string passWord);
  std::string decrypt(std::string message, std::string passWord);

}
#endif //!_UTILS_P4F_