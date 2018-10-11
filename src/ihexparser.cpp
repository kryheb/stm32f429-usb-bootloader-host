

#include "ihexparser.hpp"
#include <iostream>

IHexParser::IHexParser()
{

}


bool IHexParser::openFile(const std::string& pFileName)
{
  file.open(pFileName);
  return file.is_open();
}









IHexLineParser::IHexLineParser()
{

}


boost::optional<Record> IHexLineParser::parseLine(const std::string& pLine)
{
  Record record;
  state = State::StartCode;
  
  for (size_t index = 0; index<pLine.size();) {
    switch(state) {
      case State::StartCode:
        index++;
        break;
      case State::ByteCount: {
        if (auto byteCount = getByte(pLine.substr(index), index)) {
          record.byteCount = *byteCount;
        } else {
          state = State::Error;
        }
        break;
      }
      case State::Address: {
        auto msb = getByte(pLine.substr(index), index);
        auto lsb = getByte(pLine.substr(index), index);
        if (msb && lsb) {
          record.adddress = (*msb << 8) | *lsb;
        } else {
          state = State::Error;
        }
        break;
      }
        break;
      case State::RecordType:
              break;
      case State::Data:
              break;
      case State::Checksum:
              break;
      case State::Error:
        return boost::none;
    }
  }

}


boost::optional<Byte> IHexLineParser::getByte(const std::string& pDataStr, size_t& pIndex)
{
  try {
    auto byteStr = pDataStr.substr(pIndex, 2);
    pIndex += 2;
    Byte byte = std::stoul(byteStr, nullptr, 16);
    return byte;
  } catch (std::exception& e) {
    std::cout << e.what() << '\n';
  }
  return boost::none;
}

