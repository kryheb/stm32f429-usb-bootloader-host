#include <iostream>
#include "usbhid.hpp"
#include "bootloader.hpp"
#include <stdio.h>
#include "utils/log/log.hpp"

#define LOG(s) BOOST_LOG_SEV(generalChannel, s)
using namespace boost::log::trivial;

int main() {

  auto& logger = Logger::getInstance();
  logger.init();

  auto generalChannel = logger.addChannel("MAIN");

  constexpr auto VENDOR_ID = 0x483;
  constexpr auto PRODUCT_ID = 0x5750;

  usbhid::BusController controller;
  controller.enumerateAll();
  if (auto devicePtr = controller.findDevice(VENDOR_ID, PRODUCT_ID)) {
    devicePtr->openCommunication();
    if (devicePtr->isOpen()) {
      LOG(severity_level::info) << "Device handle is open";
      Bootloader bootloader(*devicePtr);
      bootloader.readIHex("led_bl.ihex");
      bootloader.upload();
    } else {
      LOG(severity_level::error) << "Cannot open device handle";
    }
  }


  LOG(severity_level::info) << "Closing bootloader...";
  return 0;
}
