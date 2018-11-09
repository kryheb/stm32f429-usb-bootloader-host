

#include "bootloader.hpp"
#include <iostream>
#include <unistd.h>



Bootloader::Bootloader(usbhid::Device& pDevice) :
  mDevice(pDevice)
{
  initialize();
}


void Bootloader::initialize()
{
  std::cout << "Bootloader initialization\n";

  mDevice.sendCommand(Cmd::Init, [](auto pErr){
    if (pErr->isOK()) {
      std::cout << "Command ACK" << '\n';
    } else {
      std::cout << "Command NACK: "
        << pErr->getMsg() << '\n';
    }
  });
}



void Bootloader::readIHex(const std::string& pFileName)
{
  if (!mIHexParser.openFile(pFileName)) {
    return;
  }
  mIHexParser.parseFile();
}


void Bootloader::upload()
{
  std::cout << "Upload data\n";
  auto ihexSize = mIHexParser.getDataSize();

  while(auto data = mIHexParser.getData()) {
    usleep(20000);
    auto cs = mIHexParser.getDataSize();
    double p = (1 - (double)mIHexParser.getDataSize() / (double)ihexSize);
    std::cout << "### " << std::dec << (int)(p*100) << '%' << " ###" << "\r";

  }
  std::cout<<'\n';
}


void Bootloader::applyConfiguration()
{
  std::cout << "Prepare for configuration\n";

  mDevice.sendCommand(Cmd::PerpareForConfig, [](auto pErr){
    if (pErr->isOK()) {
      std::cout << "Command ACK" << '\n';
    } else {
      std::cout << "Command NACK: "
        << pErr->getMsg() << '\n';
    }
  });
}


