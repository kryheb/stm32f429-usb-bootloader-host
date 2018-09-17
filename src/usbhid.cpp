

#include "usbhid.hpp"
#include <iostream>

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

