#include <iostream>
#include "usbhid.hpp"
#include <stdio.h>

int main() {

  usbhid::BusController controller;
  controller.enumerateAll();
  if (auto devicePtr = controller.findDevice(0x483, 0x5750)) {
    devicePtr->openCommunication();
    if (devicePtr->isOpen()) {
      std::cout << "Device Handle is Open" << '\n';
    } else {
      std::cout << "Device Handle is not Open" << '\n';
    }
  }



  return 0;
}
