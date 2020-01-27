#include "base64.h"
#include "picosha2.h"
#include "aes.h"
#include "utilsP4F.h"

namespace P4F {
  static uint8_t gInForAES[64];

  //https://stackoverflow.com/questions/17261798/converting-a-hex-string-to-a-byte-array
  int char2int(char input)
  {
    if(input >= '0' && input <= '9')
      return input - '0';
    if(input >= 'A' && input <= 'F')
      return input - 'A' + 10;
    if(input >= 'a' && input <= 'f')
      return input - 'a' + 10;
  }

  //https://stackoverflow.com/questions/17261798/converting-a-hex-string-to-a-byte-array
  // This function assumes src to be a zero terminated sanitized string with
  // an even number of [0-9a-f] characters, and target to be sufficiently large
  void hex2bin(const char* src, char* target, int size)
  {
    while(size-->0)
    {
      *(target++) = char2int(*src)*16 + char2int(src[1]);
      src += 2;
    }
  }

  static void initKeyAES(std::string passWord, uint8_t* keyAES){
    std::string pass = std::string(passWord.c_str());
    std::string hashh = picosha2::hash256_hex_string(pass);
    hex2bin(hashh.c_str(), (char*)keyAES, 32);
  }

  std::string PKCS7Padding(std::string in){
    std::string out;
    out = in;

    if ((in.length()+1)%16 != 0){
      int oldLength = in.length();
      int newLength = ((in.length()+16-1)/16)*16;
      if(in.length()%16 == 0){
        newLength = in.length()/16;
        newLength = 16*(newLength+1);
      }
      int paddingValue = newLength-oldLength;
      out.append(newLength-oldLength,(char)paddingValue);
    }

    return out;
  }

  std::string encrypt(std::string message, std::string passWord){

    if(message.length() > sizeof(gInForAES)){
      return std::string("ERROR_OVER_LENGTH");
    }

    uint8_t keyAES[32];
    struct AES_ctx ctx;
    uint8_t iv[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::string messageWithPadding = PKCS7Padding(message);
    std::memcpy(gInForAES, messageWithPadding.c_str(), messageWithPadding.length());

    initKeyAES(passWord, keyAES);
    AES_init_ctx_iv(&ctx, keyAES, iv);
    AES_CBC_encrypt_buffer(&ctx, gInForAES, messageWithPadding.length());

    std::string encryptedStr = base64_encode(gInForAES, messageWithPadding.length());

    return encryptedStr;
  }

  std::string decrypt(std::string message, std::string passWord){

    memset(gInForAES, 0, sizeof(gInForAES));

    uint8_t keyAES[32];
    struct AES_ctx ctx;
    uint8_t iv[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    std::string messageDecoded;
    messageDecoded = base64_decode(message);
    std::memcpy(gInForAES, messageDecoded.c_str(), messageDecoded.length());

    initKeyAES(passWord, keyAES);
    AES_init_ctx_iv(&ctx, keyAES, iv);
    AES_CBC_decrypt_buffer(&ctx, gInForAES, sizeof(gInForAES));

    int validIdx;
    for (validIdx = 0; validIdx < sizeof(gInForAES); validIdx++){
      if(gInForAES[validIdx] < 32 || gInForAES[validIdx]>126){
        break;
      }
    }

    std::string ret = std::string((char*)gInForAES).substr(0, validIdx);

    return ret;
  }

}

