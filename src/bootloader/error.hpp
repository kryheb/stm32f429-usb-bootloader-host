/*
 * file   error.hpp
 * author Krystian Heberlein
 * email  krystianheberlein@gmail.com
 *
 * Error class and utils
 */

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
  Error(const uint32_t aCode, const std::string& aMsg);
  static ErrorPtr err(const std::string& aMsg);
  static ErrorPtr errOK();
  bool isOK();
  std::string getMsg() const { return mMsg; }
};
