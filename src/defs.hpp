

#pragma once


enum class Cmd {
  Init = 0x10
};

enum class TargetResponse {
  InitializationOK = 0x11,
  InitializationNOK
};