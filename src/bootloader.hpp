#include "usbhid.hpp"

class Bootloader
{
  usbhid::Device& mDevice;
  void initialize();

  public:
  Bootloader(usbhid::Device& pDevice);

};
