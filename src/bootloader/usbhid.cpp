

#include "usbhid.hpp"
#include <iostream>
#include <future>
#include <stdexcept>


using namespace usbhid;


std::string wideCharToString(wchar_t* pWideCharTxtPtr)
{
  if (pWideCharTxtPtr) {
    std::wstring ws(pWideCharTxtPtr);
    return std::string(ws.begin(), ws.end());
  }
  return "";
}


Device::Device(const struct hid_device_info* pDeviceInfoPtr)
{
  if (pDeviceInfoPtr) {
    mVendorId = pDeviceInfoPtr->vendor_id;
    mProductId = pDeviceInfoPtr->product_id;
    mPath = pDeviceInfoPtr->path;
    mSerialNumber = wideCharToString(pDeviceInfoPtr->serial_number);
    mManufacturerName = wideCharToString(pDeviceInfoPtr->manufacturer_string);
    mProductString = wideCharToString(pDeviceInfoPtr->product_string);
  }

  showDeviceInfo();
}


void Device::showDeviceInfo() const
{
  std::cout
      << "Device Info:" << '\n'
      << " - vendor id:     " << std::hex << "0x" << mVendorId << '\n'
      << " - product id:    " << std::hex << "0x" << mProductId << '\n'
      << " - path:          " << mPath << '\n'
      << " - serial number: " << mSerialNumber << '\n'
      << " - manufacturer:  " << mManufacturerName << '\n'
      << " - product name:  " << mProductString << '\n'
      << '\n';
}


bool Device::openCommunication()
{
  mHandle = hid_open(mVendorId, mProductId, NULL);
  return isOpen();
}


void Device::sendData(const DataBuffer& aData, TargetResponse pAckCode, SendDataCB pCallback)
{
  hid_write(mHandle, aData.data(), aData.size());

  if (pAckCode == TargetResponse::NoResponseRequired) {
    return;
  }

  std::promise<int> pReceiveResponse;
  std::future<int> fReceiveResponse = pReceiveResponse.get_future();
  std::thread([&pReceiveResponse, this]{
    try {
      auto result = receiveResponse();
      pReceiveResponse.set_value(result);
    } catch (...) {
      pReceiveResponse.set_exception(std::current_exception());
    }
  }).detach();

  std::cout << "Waiting..." << '\n';
  fReceiveResponse.wait();
  try {
    auto response = fReceiveResponse.get();
    std::cout << "Response " << std::hex << response << '\n';
    if (response != toUnderlyingType(pAckCode)) {
      if (pCallback) pCallback(Error::err("Invalid response: " + response));
    }
    if (pCallback) pCallback(Error::errOK());

  } catch (std::exception& e) {
    std::cout << "Response exception: " << e.what() << '\n';
    if (pCallback) pCallback(Error::err("Response timeout"));
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


DevicePtr BusController::findDevice(uint32_t pVendorId, uint32_t pProductId)
{
  auto it = std::find_if(mDevices.begin(), mDevices.end(), [=](auto pDevicePtr){
    return (pDevicePtr->getVendorId() == pVendorId) &&
            (pDevicePtr->getProductId() == pProductId);
  });

  if (it != mDevices.end()) {
    return *it;
  }

  return {};
}


