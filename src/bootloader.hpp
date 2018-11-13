#include "usbhid.hpp"
#include "ihexparser.hpp"

class Bootloader
{
  usbhid::Device& mDevice;
  IHexParser mIHexParser;
  DataBuffer dataBuffer{0};

  void initialize();

  public:
  Bootloader(usbhid::Device& pDevice);

  void readIHex(const std::string& pFileName);
  void upload();

  private:
  void setAddressBase(const Bytes& aData);
  void processRecord(const Record& pRecord);

};
