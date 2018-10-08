

#include "bootloader.hpp"
#include <iostream>



Bootloader::Bootloader(usbhid::Device& pDevice) :
  mDevice(pDevice)
{
  initialize();
}


void Bootloader::initialize()
{
  mDevice.sendCommand(Cmd::Init, [](auto pErr){
    if (pErr->isOK()) {
      std::cout << "Command ACK" << '\n';
    } else {
      std::cout << "Command NACK: "
        << pErr->getMsg() << '\n';
    }
  });
}
