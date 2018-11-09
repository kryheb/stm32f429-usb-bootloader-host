#include "usbhid.hpp"
#include "ihexparser.hpp"

class Bootloader
{
  usbhid::Device& mDevice;
  IHexParser mIHexParser;
  void initialize();

  public:
  Bootloader(usbhid::Device& pDevice);

  void readIHex(const std::string& pFileName);
  void upload();

  private:
  void applyConfiguration();

};
