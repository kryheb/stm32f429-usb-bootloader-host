

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
  std::fill(dataBuffer.begin(), dataBuffer.end(), 0);
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
  hexSize = mIHexParser.getDataSize();
}


void Bootloader::upload()
{
  std::cout << "Upload data\n";
    auto cs = mIHexParser.getDataSize();
    double p = (1 - (double)mIHexParser.getDataSize() / (double)hexSize);
    std::cout << mIHexParser.getDataSize() << " / " << hexSize << '\n';

  if(auto rec = mIHexParser.popRecord()) {
    processRecord(*rec, [&](ErrorPtr pErr){
      if (pErr->isOK()) {
        this->upload();
      } else {
        std::cout << "Error received, breaking upload."
          << "Cause: " << pErr->getMsg() << '\n';
      }
    });


  } else {
    std::cout<<'\n';
    dataBuffer = {0x01, 0x50};
    mDevice.sendData(dataBuffer, [](auto pErr){

    });
  }
}


void Bootloader::processRecord(const Record& pRecord, CompletedCb pCallback)
{
  switch(pRecord.recordType) {
    case RecordType::Data: {
      flashData(pRecord.rawRecord(), pCallback);
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
        pCallback(Error::err("Invalid byte count"));
        return;
      }
      setAddressBase(pRecord.rawRecord(), pCallback);
      break;
    }
    case RecordType::StartLinearAddress: {
      break;
    }
    default: break;
  }
}


void Bootloader::setAddressBase(const Bytes& aData, CompletedCb pCallback)
{
  std::cout << "Prepare for configuration\n";
  std::fill(dataBuffer.begin(), dataBuffer.end(), 0);
  dataBuffer = {0x01, static_cast<uint8_t>(Cmd::SetAddressBase)};
  std::copy(aData.begin(), aData.end(), dataBuffer.begin() + 2);
  mDevice.sendData(dataBuffer, [&, pCallback](auto pErr){
    if (pErr->isOK()) {
      std::cout << "setAddressBase ACK" << '\n';
      this->initializeFlash(pCallback);
    } else {
      std::cout << "setAddressBase NACK: "
        << pErr->getMsg() << '\n';
      pCallback(pErr);
    }
  });
}


void Bootloader::initializeFlash(CompletedCb pCallback)
{
  std::cout << "Initialize flash\n";
  std::fill(dataBuffer.begin(), dataBuffer.end(), 0);
  dataBuffer = {0x01, static_cast<uint8_t>(Cmd::InitializeFlash)};
  mDevice.sendData(dataBuffer, [pCallback](auto pErr){
    if (pErr->isOK()) {
      std::cout << "setAddressBase ACK" << '\n';
    } else {
      std::cout << "setAddressBase NACK: "
        << pErr->getMsg() << '\n';
    }
    pCallback(pErr);
  });
}


void Bootloader::flashData(const Bytes& aData, CompletedCb pCallback)
{
  std::fill(dataBuffer.begin(), dataBuffer.end(), 0);
  dataBuffer = {0x01, static_cast<uint8_t>(Cmd::FlashData)};
  std::copy(aData.begin(), aData.end(), dataBuffer.begin() + 2);
  for (auto e:dataBuffer) {
    printf("|%x|", e);
  }
  mDevice.sendData(dataBuffer, [pCallback](auto pErr){
    if (pErr->isOK()) {
      std::cout << "flashData ACK" << '\n';
    } else {
      std::cout << "flashData NACK: "
        << pErr->getMsg() << '\n';
    }
    pCallback(pErr);
  });
}



