/*
 * file   defs.hpp
 * author Krystian Heberlein
 * email  krystianheberlein@gmail.com
 *
 * Common definitions
 */

#pragma once

#include <stdint.h>
#include <vector>


enum class Cmd: uint8_t {
  Init = 0x10,
  SetAddressBase = 0x20,
  InitializeFlash = 0x30,
  FlashData = 0x40,
  SetStartAddress = 0x50,
  LaunchApplication = 0x60
};

enum class TargetResponse: uint8_t  {
  NoResponseRequired = 0x0,
  InitializationOK = 0x11,
  InitializationNOK,
  AddressBaseSet = 0x21,
  InvalidAddressBase,
  FlashInitializationOK = 0x31,
  FlashInitializationNOK,
  FlashingOK = 0x41,
  FlashingNOK,
  SetStartAddressOK = 0x51,
  SetStartAddressNOK,
  UnknownCommand
};

template<typename T>
constexpr auto toUnderlyingType(T aVal)
{
  return static_cast<std::underlying_type_t<T>>(aVal);
}

static constexpr auto BUFFER_SIZE = 65;
using DataBuffer = std::array<uint8_t, BUFFER_SIZE>;
