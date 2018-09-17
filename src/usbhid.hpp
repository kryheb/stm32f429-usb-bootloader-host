

#include "hidapi.h"
#include <stdint.h>
#include <string>
#include <list>
#include <memory>
#include <algorithm>


namespace usbhid {

class Device
{
  public:
    Device(const struct hid_device_info* pDeviceInfoPtr);
    void showDeviceInfo() const;
    bool openCommunication();
    bool isOpen() const { return mHandle; }

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
