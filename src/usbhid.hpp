#pragma once

#include "hidapi.h"
#include <stdint.h>
#include <string>
#include <list>
#include <memory>
#include <algorithm>

#include "error.hpp"
#include "defs.hpp"


namespace usbhid {



class Device
{
  static constexpr auto RECEIVE_COMMAND_TIMEOUT = 5000;
  using SendDataCB = std::function<void(ErrorPtr)>;

  static constexpr auto RESPONSE_BYTE_INDEX = 1;

  public:
    Device(const struct hid_device_info* pDeviceInfoPtr);
    void showDeviceInfo() const;
    bool openCommunication();
    bool isOpen() const { return mHandle; }

    void sendData(const DataBuffer& aData, SendDataCB pCallback);

    uint32_t getVendorId() const { return mVendorId; }
    uint32_t getProductId() const { return mProductId; }

  private:
    uint32_t mVendorId;
    uint32_t mProductId;
    std::string mPath;
    std::string mSerialNumber;
    std::string mManufacturerName;
    std::string mProductString;

    hid_device* mHandle;

    uint8_t receiveResponse();




};
using DevicePtr = std::shared_ptr<Device>;
using Devices = std::list<DevicePtr>;


class BusController
{

  public:
    BusController();
    void enumerateAll();
    DevicePtr findDevice(uint32_t pVendorId, uint32_t pProductId);

  private:
    Devices mDevices;
};

} // namespace usbhid
