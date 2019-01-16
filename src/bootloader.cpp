

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
  DataBuffer dataBuffer = {EP_ADDRESS, toUnderlyingType(Cmd::Init)};

  mDevice.sendData(dataBuffer, TargetResponse::InitializationOK, [](auto pErr){
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
  std::cout << mIHexParser.getDataSize() << " / " << hexSize << '\n';

  if(auto rec = mIHexParser.popRecord()) {
    processRecord(*rec, [&](ErrorPtr pErr){
      if (pErr->isOK()) {
        this->upload();
      } else {
        std::cout << "Error received, breaking upload."
          << " Reason:" << pErr->getMsg() << '\n';
      }
    });
  } else {
    std::cout << "Upload finished, going to launch application...\n";
    launchApplication();
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
      pCallback(Error::errOK());
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
      setStartLinearAddress(pRecord.rawRecord(), pCallback);
      break;
    }
    default: break;
  }
}


void Bootloader::setAddressBase(const Bytes& aData, CompletedCb pCallback)
{
  std::cout << "Prepare for configuration\n";

  DataBuffer dataBuffer = {EP_ADDRESS, toUnderlyingType(Cmd::SetAddressBase)};
  std::copy(aData.begin(), aData.end(), dataBuffer.begin() + 2);
  mDevice.sendData(dataBuffer, TargetResponse::AddressBaseSet, [&, pCallback](auto pErr){
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
  DataBuffer dataBuffer = {EP_ADDRESS, toUnderlyingType(Cmd::InitializeFlash)};
  mDevice.sendData(dataBuffer, TargetResponse::FlashInitializationOK, [pCallback](auto pErr){
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
  DataBuffer dataBuffer = {EP_ADDRESS, toUnderlyingType(Cmd::FlashData)};
  std::copy(aData.begin(), aData.end(), dataBuffer.begin() + 2);
  for (auto e:dataBuffer) {
    printf("%x ", e);
  }
  mDevice.sendData(dataBuffer, TargetResponse::FlashingOK, [pCallback](auto pErr){
    if (pErr->isOK()) {
      std::cout << "flashData ACK" << '\n';
    } else {
      std::cout << "flashData NACK: "
        << pErr->getMsg() << '\n';
    }
    pCallback(pErr);
  });
}


void Bootloader::setStartLinearAddress(const Bytes& aData, CompletedCb pCallback)
{
  DataBuffer dataBuffer = {EP_ADDRESS, toUnderlyingType(Cmd::SetStartAddress)};
  std::copy(aData.begin(), aData.end(), dataBuffer.begin() + 2);
  mDevice.sendData(dataBuffer, TargetResponse::SetStartAddressOK, [pCallback](auto pErr){
    if (pErr->isOK()) {
      std::cout << "setStartLinearAddress ACK" << '\n';
    } else {
      std::cout << "setStartLinearAddress NACK: "
        << pErr->getMsg() << '\n';
    }
    pCallback(pErr);
  });
}


void Bootloader::launchApplication()
{
  DataBuffer dataBuffer = {EP_ADDRESS, toUnderlyingType(Cmd::LaunchApplication)};
  mDevice.sendData(dataBuffer); 
}






  
