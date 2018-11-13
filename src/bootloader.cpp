

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
  dataBuffer.clear();
  dataBuffer = {0x01, static_cast<uint8_t>(Cmd::Init)};

  mDevice.sendData(dataBuffer, [](auto pErr){
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

  while(auto rec = mIHexParser.popRecord()) {
    processRecord(*rec);
    auto cs = mIHexParser.getDataSize();
    double p = (1 - (double)mIHexParser.getDataSize() / (double)ihexSize);
    std::cout << "### " << std::dec << (int)(p*100) << '%' << " ###" << "\r";

  }
  std::cout<<'\n';
}


void Bootloader::processRecord(const Record& pRecord)
{
  switch(pRecord.recordType) {
    case RecordType::Data: {
      break;
    }
    case RecordType::EndOfFile: {
      break;
    }
    case RecordType::ExtendedSegmentAddress: {
      break;
    }
    case RecordType::StartSegmentAddress: {
      break;
    }
    case RecordType::ExtendedLinearAddress: {
      if (pRecord.byteCount!=2) {
        return; // TODO: some error
      }
      setAddressBase(pRecord.data);
      break;
    }
    case RecordType::StartLinearAddress: {
      break;
    }
    default: break;
  }
}


void Bootloader::setAddressBase(const Bytes& aData)
{
  std::cout << "Prepare for configuration\n";
  dataBuffer.clear();
  dataBuffer = {0x01, static_cast<uint8_t>(Cmd::SetAddressBase), aData.at(0), aData.at(1)};
  mDevice.sendData(dataBuffer, [](auto pErr){
    if (pErr->isOK()) {
      std::cout << "setAddressBase ACK" << '\n';
    } else {
      std::cout << "setAddressBase NACK: "
        << pErr->getMsg() << '\n';
    }
  });
}


