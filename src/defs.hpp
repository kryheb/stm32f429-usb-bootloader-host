

#pragma once


enum class Cmd {
  Init = 0x10,
  SetAddressBase = 0x20
};

enum class TargetResponse {
  InitializationOK = 0x11,
  InitializationNOK,
  AddressBaseSet = 0x21,
  InvalidAddressBase

};

static constexpr auto BUFFER_SIZE = 65;
using DataBuffer = std::array<uint8_t, BUFFER_SIZE>;