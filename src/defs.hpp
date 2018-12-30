

#pragma once

enum class Cmd {
  Init = 0x10,
  SetAddressBase = 0x20,
  InitializeFlash = 0x30,
  FlashData = 0x40
};

enum class TargetResponse {
  InitializationOK = 0x11,
  InitializationNOK,
  AddressBaseSet = 0x21,
  InvalidAddressBase,
  FlashInitializationOK = 0x31,
  FlashInitializationNOK,
  FlashingOK = 0x41,
  FlashingNOK
};

static constexpr auto BUFFER_SIZE = 65;
using DataBuffer = std::array<uint8_t, BUFFER_SIZE>;
