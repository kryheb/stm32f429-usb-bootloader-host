/*
 * file   bootloader.cpp
 * author Krystian Heberlein
 * email  krystianheberlein@gmail.com
 *
 */

#include "bootloader.hpp"
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <iomanip>


#define LOG(s) BOOST_LOG_SEV(mLoggerChannel, s)
using namespace boost::log::trivial;


Bootloader::Bootloader(usbhid::Device& aDevice) :
  mDevice(aDevice),
  mLoggerChannel(Logger::getInstance().addChannel("BOOTLOADER"))
{
  initialize();
}


void Bootloader::initialize()
{
  LOG(severity_level::info) << "Bootloader initialization";
  DataBuffer dataBuffer = {EP_ADDRESS, toUnderlyingType(Cmd::Init)};

  mDevice.sendData(dataBuffer, TargetResponse::InitializationOK, [=](auto aErr){
    if (aErr->isOK()) {
      LOG(severity_level::debug) << "Command ACK";
    } else {
      LOG(severity_level::debug) << "Command NACK: " << aErr->getMsg();
    }
  });
}



void Bootloader::readIHex(const std::string& aFileName)
{
  if (!mIHexParser.openFile(aFileName)) {
    LOG(severity_level::error) << "Cannot open file " << aFileName;
    return;
  }
  mIHexParser.parseFile();
  hexSize = mIHexParser.getDataSize();
}


void Bootloader::upload()
{
  LOG(severity_level::debug) << "Upload data";
  LOG(severity_level::debug) << mIHexParser.getDataSize() << " / " << hexSize;

  if(auto rec = mIHexParser.popRecord()) {
    processRecord(*rec, [&](ErrorPtr aErr){
      if (aErr->isOK()) {
        this->upload();
      } else {
        LOG(severity_level::error)
          << "Error received, breaking upload."
          << " Reason:" << aErr->getMsg();
      }
    });
  } else {
    LOG(severity_level::info) << "Upload finished, going to launch application...";
    launchApplication();
  }
}


void Bootloader::processRecord(const Record& aRecord, CompletedCb aCallback)
{
  switch(aRecord.recordType) {
    case RecordType::Data: {
      flashData(aRecord.rawRecord(), aCallback);
      break;
    }
    case RecordType::EndOfFile: {
      aCallback(Error::errOK());
      break;
    }
    case RecordType::ExtendedSegmentAddress: {
      break;
    }
    case RecordType::StartSegmentAddress: {
      break;
    }
    case RecordType::ExtendedLinearAddress: {
      if (aRecord.byteCount!=2) {
        aCallback(Error::err("Invalid byte count"));
        return;
      }
      setAddressBase(aRecord.rawRecord(), aCallback);
      break;
    }
    case RecordType::StartLinearAddress: {
      setStartLinearAddress(aRecord.rawRecord(), aCallback);
      break;
    }
    default: break;
  }
}


void Bootloader::setAddressBase(const Bytes& aData, CompletedCb aCallback)
{
  LOG(severity_level::info) << "Prepare for configuration";

  DataBuffer dataBuffer = {EP_ADDRESS, toUnderlyingType(Cmd::SetAddressBase)};
  std::copy(aData.begin(), aData.end(), dataBuffer.begin() + 2);
  mDevice.sendData(dataBuffer, TargetResponse::AddressBaseSet, [=](auto aErr){
    if (aErr->isOK()) {
      LOG(severity_level::debug) << "setAddressBase ACK";
      this->initializeFlash(aCallback);
    } else {
      LOG(severity_level::debug)
        << "setAddressBase NACK: "
        << aErr->getMsg();
      aCallback(aErr);
    }
  });
}


void Bootloader::initializeFlash(CompletedCb aCallback)
{
  LOG(severity_level::info) << "Initialize flash";
  DataBuffer dataBuffer = {EP_ADDRESS, toUnderlyingType(Cmd::InitializeFlash)};
  mDevice.sendData(dataBuffer, TargetResponse::FlashInitializationOK, [=](auto aErr){
    if (aErr->isOK()) {
      LOG(severity_level::debug) << "setAddressBase ACK";
    } else {
      LOG(severity_level::debug)
        << "setAddressBase NACK: "
        << aErr->getMsg();
    }
    aCallback(aErr);
  });
}


void Bootloader::flashData(const Bytes& aData, CompletedCb aCallback)
{
  DataBuffer dataBuffer = {EP_ADDRESS, toUnderlyingType(Cmd::FlashData)};
  std::copy(aData.begin(), aData.end(), dataBuffer.begin() + 2);

  std::stringstream hexDump;
  hexDump << "Data buffer:";
  for (auto i=0; i<dataBuffer.size(); ++i) {
    if (i%16==0) {
      hexDump << '\n'
              << std::setfill(' ') << std::setw(60)
              << ' ';
    }
    hexDump << std::setfill('0') << std::setw(2)
            << std::hex << static_cast<int>(dataBuffer.at(i))
            << ' ';
  }
  LOG(severity_level::debug) << hexDump.str();

  mDevice.sendData(dataBuffer, TargetResponse::FlashingOK, [=](auto aErr){
    if (aErr->isOK()) {
      LOG(severity_level::debug) << "flashData ACK";
    } else {
      LOG(severity_level::debug)
        << "flashData NACK: "
        << aErr->getMsg();
    }
    aCallback(aErr);
  });
}


void Bootloader::setStartLinearAddress(const Bytes& aData, CompletedCb aCallback)
{
  DataBuffer dataBuffer = {EP_ADDRESS, toUnderlyingType(Cmd::SetStartAddress)};
  std::copy(aData.begin(), aData.end(), dataBuffer.begin() + 2);
  mDevice.sendData(dataBuffer, TargetResponse::SetStartAddressOK, [=](auto aErr){
    if (aErr->isOK()) {
      LOG(severity_level::debug) << "setStartLinearAddress ACK";
    } else {
      LOG(severity_level::debug)
        << "setStartLinearAddress NACK: "
        << aErr->getMsg();
    }
    aCallback(aErr);
  });
}


void Bootloader::launchApplication()
{
  DataBuffer dataBuffer = {EP_ADDRESS, toUnderlyingType(Cmd::LaunchApplication)};
  mDevice.sendData(dataBuffer);
}



