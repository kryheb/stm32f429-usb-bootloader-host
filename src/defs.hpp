

#pragma once


enum class Cmd {
  Init = 0x10,
  PerpareForConfig = 0x20
};

enum class TargetResponse {
  InitializationOK = 0x11,
  InitializationNOK,
  ReadyForConfig = 0x21
};