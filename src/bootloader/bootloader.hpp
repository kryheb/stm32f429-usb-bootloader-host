/*
 * file   bootloader.hpp
 * author Krystian Heberlein
 * email  krystianheberlein@gmail.com
 *
 */

#include "usbhid.hpp"
#include "ihexparser.hpp"
#include "utils/log/log.hpp"

using CompletedCb = std::function<void(ErrorPtr)>;

class Bootloader
{
  static constexpr auto EP_ADDRESS = 1;
  usbhid::Device& mDevice;
  IHexParser mIHexParser;

  LoggerChannel& mLoggerChannel;

  uint32_t hexSize = 0;

  void initialize();

  public:
  Bootloader(usbhid::Device& aDevice);

  void readIHex(const std::string& aFileName);
  void upload();

  private:
  void setAddressBase(const Bytes& aData, CompletedCb aCallback);
  void initializeFlash(CompletedCb aCallback);
  void flashData(const Bytes& aData, CompletedCb aCallback);
  void processRecord(const Record& aRecord, CompletedCb aCallback);
  void setStartLinearAddress(const Bytes& aData, CompletedCb aCallback);
  void launchApplication();

};
