/*
 * file   error.cpp
 * author Krystian Heberlein
 * email  krystianheberlein@gmail.com
 *
 * Error class and utils
 */

#include "error.hpp"


Error::Error(const uint32_t aCode, const std::string& aMsg):
  mCode(aCode),
  mMsg(aMsg)
{

}


ErrorPtr Error::err(const std::string& aMsg)
{
  return std::make_shared<Error>(DEFAULT_ERR_CODE, aMsg);
}


ErrorPtr Error::errOK()
{
  return std::make_shared<Error>(OK_ERR_CODE, "OK");
}


bool Error::isOK()
{
  return mCode == OK_ERR_CODE;
}

