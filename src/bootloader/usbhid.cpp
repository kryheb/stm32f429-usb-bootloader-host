/*
 * file   usbhid.cpp
 * author Krystian Heberlein
 * email  krystianheberlein@gmail.com
 *
 * usb HID communication layer
 */

#include "usbhid.hpp"
#include <iostream>
#include <future>
#include <stdexcept>

#define LOG(s) BOOST_LOG_SEV(mLoggerChannel, s)
using namespace boost::log::trivial;

using namespace usbhid;


std::string wideCharToString(wchar_t* aWideCharTxtPtr)
{
  if (aWideCharTxtPtr) {
    std::wstring ws(aWideCharTxtPtr);
    return std::string(ws.begin(), ws.end());
  }
  return "";
}


Device::Device(const struct hid_device_info* aDeviceInfoPtr):
  mLoggerChannel(Logger::getInstance().addChannel("USB DEVICE"))
{
  if (aDeviceInfoPtr) {
    mVendorId = aDeviceInfoPtr->vendor_id;
    mProductId = aDeviceInfoPtr->product_id;
    mPath = aDeviceInfoPtr->path;
    mSerialNumber = wideCharToString(aDeviceInfoPtr->serial_number);
    mManufacturerName = wideCharToString(aDeviceInfoPtr->manufacturer_string);
    mProductString = wideCharToString(aDeviceInfoPtr->product_string);
  }

  showDeviceInfo();
}


void Device::showDeviceInfo() const
{
  LOG(severity_level::info) << '\n'
      << "Device Info:" << '\n'
      << " - vendor id:     " << std::hex << "0x" << mVendorId << '\n'
      << " - product id:    " << std::hex << "0x" << mProductId << '\n'
      << " - path:          " << mPath << '\n'
      << " - serial number: " << mSerialNumber << '\n'
      << " - manufacturer:  " << mManufacturerName << '\n'
      << " - product name:  " << mProductString;
}


bool Device::openCommunication()
{
  mHandle = hid_open(mVendorId, mProductId, NULL);
  return isOpen();
}


void Device::sendData(const DataBuffer& aData, TargetResponse aAckCode, SendDataCB aCallback)
{
  hid_write(mHandle, aData.data(), aData.size());

  if (aAckCode == TargetResponse::NoResponseRequired) {
    return;
  }

  std::promise<int> aReceiveResponse;
  std::future<int> fReceiveResponse = aReceiveResponse.get_future();
  std::thread([&aReceiveResponse, this]{
    try {
      auto result = receiveResponse();
      aReceiveResponse.set_value(result);
    } catch (...) {
      aReceiveResponse.set_exception(std::current_exception());
    }
  }).detach();

  LOG(severity_level::debug) << "Waiting for response...";
  fReceiveResponse.wait();
  try {
    auto response = fReceiveResponse.get();
    LOG(severity_level::debug) << "Response received: " << std::hex << response;
    if (response != toUnderlyingType(aAckCode)) {
      if (aCallback) aCallback(Error::err("Invalid response: " + response));
    }
    if (aCallback) aCallback(Error::errOK());

  } catch (std::exception& e) {
    LOG(severity_level::error) << "Response exception: " << e.what();
    if (aCallback) aCallback(Error::err("Response timeout"));
  }
}


uint8_t Device::receiveResponse()
{
  DataBuffer data{0};
  auto result = hid_read_timeout(mHandle, data.data(), data.size(), RECEIVE_COMMAND_TIMEOUT);
  if (result <= 0) {
    throw std::runtime_error("No data received");
  }
  return data.at(RESPONSE_BYTE_INDEX);
}


BusController::BusController()
{

}


void BusController::enumerateAll()
{
  struct hid_device_info *devs, *cur_dev;

  devs = hid_enumerate(0x0, 0x0);
  cur_dev = devs;
  while (cur_dev) {
    mDevices.emplace_back(std::make_shared<Device>(cur_dev));
    cur_dev = cur_dev->next;
  }
  hid_free_enumeration(devs);
}


DevicePtr BusController::findDevice(uint32_t aVendorId, uint32_t aProductId)
{
  auto it = std::find_if(mDevices.begin(), mDevices.end(), [=](auto aDevicePtr){
    return (aDevicePtr->getVendorId() == aVendorId) &&
            (aDevicePtr->getProductId() == aProductId);
  });

  if (it != mDevices.end()) {
    return *it;
  }

  return {};
}


