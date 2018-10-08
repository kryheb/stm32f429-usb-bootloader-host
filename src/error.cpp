

#include "error.hpp"


Error::Error(const uint32_t pCode, const std::string& pMsg):
  mCode(pCode),
  mMsg(pMsg)
{

}


ErrorPtr Error::err(const std::string& pMsg)
{
  return std::make_shared<Error>(DEFAULT_ERR_CODE, pMsg);
}


ErrorPtr Error::errOK()
{
  return std::make_shared<Error>(OK_ERR_CODE, "OK");
}


bool Error::isOK()
{
  return mCode == OK_ERR_CODE;
}

