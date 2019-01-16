#include <iostream>
#include "usbhid.hpp"
#include "bootloader.hpp"
#include <stdio.h>

int main() {

  constexpr auto VENDOR_ID = 0x483;
  constexpr auto PRODUCT_ID = 0x5750;

  usbhid::BusController controller;
  controller.enumerateAll();
  if (auto devicePtr = controller.findDevice(VENDOR_ID, PRODUCT_ID)) {
    devicePtr->openCommunication();
    if (devicePtr->isOpen()) {
      std::cout << "Device Handle is open" << '\n';
      Bootloader bootloader(*devicePtr);
      bootloader.readIHex("led_bl.ihex");
      bootloader.upload();
    } else {
      std::cout << "Device Handle is not open" << '\n';
    }
  }


  std::cout << "Closing bootloader..." << '\n';
  return 0;
}
