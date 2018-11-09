

#include "ihexparser.hpp"
#include <iostream>
#include <numeric>

IHexParser::IHexParser()
{

}


bool IHexParser::openFile(const std::string& pFileName)
{
  mFile.open(pFileName);
  return mFile.is_open();
}

void IHexParser::parseFile()
{
  std::string line;
  while(std::getline(mFile, line)) {
    IHexLineParser lineParser;
    auto record = lineParser.parseLine(line);
    if (!record) {
      mRecords.clear();
      return;
    }
    mRecords.push_back(*record);
  }
}

boost::optional<Record> IHexParser::getData()
{
  if (mRecords.empty()) {
    return boost::none;
  }
  auto data = mRecords.front();
  mRecords.pop_front();
  return data;
}


IHexLineParser::IHexLineParser()
{

}


boost::optional<Record> IHexLineParser::parseLine(const std::string& pLine)
{
  Record record{0};
  state = State::StartCode;
  
  for (size_t index = 0; index<=pLine.size();) {
    switch(state) {
      case State::StartCode: {
        index++;
        state = State::ByteCount;
        break;
      }
      case State::ByteCount: {
        if (auto byteCount = getByte(pLine, index)) {
          record.byteCount = *byteCount;
          state = State::Address;
        } else {
          state = State::Error;
        }
        break;
      }
      case State::Address: {
        auto msb = getByte(pLine, index);
        auto lsb = getByte(pLine, index);
        if (msb && lsb) {
          record.adddress = (*msb << 8) | *lsb;
          state = State::RecordType;
        } else {
          state = State::Error;
        }
        break;
      }
      case State::RecordType: {
        auto recordTypeByte = getByte(pLine, index);
        if (!recordTypeByte) {
          state = State::Error;
          break;
        }
        auto recordType = getRecordType(*recordTypeByte);
        if (!recordType) {
          state = State::Error;
          break;
        }
        record.recordType = *recordType;
        state = State::Data;
        break;
      }
      case State::Data: {
        if (record.data.size() == record.byteCount) {
          state = State::Checksum;
          break;
        }
        if (auto dataByte = getByte(pLine, index)) {
          record.data.push_back(*dataByte);
        } else {
          state = State::Error;
        }
        break;
      }
      case State::Checksum: {
        if (auto checksum = getByte(pLine, index)) {
          record.checksum = *checksum;
          if (verifyChecksum(record)) {
            state = State::Done;
          }
        } else {
          state = State::Error;
        }
        break;
      }
      case State::Done:
        return record;
      case State::Error:
        return boost::none;

      default:
        return boost::none;
    }
  }

  return boost::none;
}


boost::optional<Byte> IHexLineParser::getByte(const std::string& pDataStr, size_t& pIndex)
{
  if (pDataStr.size() < 2) {
    return boost::none;
  }

  auto byteStr = pDataStr.substr(pIndex, 2);
  pIndex += 2;
  try {
    Byte byte = std::stoul(byteStr, nullptr, 16);
    return byte;
  } catch (std::exception& e) {
    std::cout << e.what() << '\n';
  }

  return boost::none;
}


boost::optional<RecordType> IHexLineParser::getRecordType(const Byte pByte)
{
  if (pByte>static_cast<int>(RecordType::StartLinearAddress)) {
    return boost::none;
  }
  return static_cast<RecordType>(pByte);
}

bool IHexLineParser::verifyChecksum(const Record& pRecord)
{
  uint8_t sum = 0;
  sum = pRecord.byteCount
        + ((pRecord.adddress >> 8) & 0xFF)
        + (pRecord.adddress & 0xFF)
        + static_cast<uint8_t>(pRecord.recordType)
        + std::accumulate(pRecord.data.begin(), pRecord.data.end(), 0);

  sum ^= 0xFF;
  sum++;
  return sum == pRecord.checksum;
}

