#pragma once

#include <stdint.h>
#include <string>
#include <memory>


#include "defs.hpp"

class Error;
using ErrorPtr = std::shared_ptr<Error>;

constexpr auto OK_ERR_CODE = 0;
constexpr auto DEFAULT_ERR_CODE = 1;

class Error
{
  uint32_t mCode;
  const std::string mMsg;

  public:
  Error(const uint32_t pCode, const std::string& pMsg);
  static ErrorPtr err(const std::string& pMsg);
  static ErrorPtr errOK();
  bool isOK();
  std::string getMsg() const { return mMsg; }
};
