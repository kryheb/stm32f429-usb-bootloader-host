#include "usbhid.hpp"
#include "ihexparser.hpp"

using CompletedCb = std::function<void(ErrorPtr)>;

class Bootloader
{
  static constexpr auto EP_ADDRESS = 1;
  usbhid::Device& mDevice;
  IHexParser mIHexParser;

  uint32_t hexSize = 0;

  void initialize();

  public:
  Bootloader(usbhid::Device& pDevice);

  void readIHex(const std::string& pFileName);
  void upload();

  private:
  void setAddressBase(const Bytes& aData, CompletedCb pCallback);
  void initializeFlash(CompletedCb pCallback);
  void flashData(const Bytes& aData, CompletedCb pCallback);
  void processRecord(const Record& pRecord, CompletedCb pCallback);
  void setStartLinearAddress(const Bytes& aData, CompletedCb pCallback);
  void launchApplication();

};
