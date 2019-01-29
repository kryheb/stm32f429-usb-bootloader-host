/*
 * file   ihexparser.cpp
 * author Krystian Heberlein
 * email  krystianheberlein@gmail.com
 *
 * intel hex parser
 */

#include "ihexparser.hpp"
#include <iostream>
#include <numeric>

#define LOG(s) BOOST_LOG_SEV(mLoggerChannel, s)
using namespace boost::log::trivial;


IHexParser::IHexParser()
{

}


bool IHexParser::openFile(const std::string& aFileName)
{
  mFile.open(aFileName);
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

std::optional<Record> IHexParser::popRecord()
{
  if (mRecords.empty()) {
    return std::nullopt;
  }
  auto data = mRecords.front();
  mRecords.pop_front();
  return data;
}


IHexLineParser::IHexLineParser() :
  mLoggerChannel(Logger::getInstance().addChannel("IHEX PARSER"))
{

}


std::optional<Record> IHexLineParser::parseLine(const std::string& aLine)
{
  Record record{0};
  state = State::StartCode;
  
  for (size_t index = 0; index<=aLine.size();) {
    switch(state) {
      case State::StartCode: {
        index++;
        state = State::ByteCount;
        break;
      }
      case State::ByteCount: {
        if (auto byteCount = getByte(aLine, index)) {
          record.byteCount = *byteCount;
          state = State::Address;
        } else {
          state = State::Error;
        }
        break;
      }
      case State::Address: {
        auto msb = getByte(aLine, index);
        auto lsb = getByte(aLine, index);
        if (msb && lsb) {
          record.address = (*msb << 8) | *lsb;
          state = State::RecordType;
        } else {
          state = State::Error;
        }
        break;
      }
      case State::RecordType: {
        auto recordTypeByte = getByte(aLine, index);
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
        if (auto dataByte = getByte(aLine, index)) {
          record.data.push_back(*dataByte);
        } else {
          state = State::Error;
        }
        break;
      }
      case State::Checksum: {
        if (auto checksum = getByte(aLine, index)) {
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
        return std::nullopt;

      default:
        return std::nullopt;
    }
  }

  return std::nullopt;
}


std::optional<Byte> IHexLineParser::getByte(const std::string& aDataStr, size_t& aIndex)
{
  if (aDataStr.size() < 2) {
    return std::nullopt;
  }

  auto byteStr = aDataStr.substr(aIndex, 2);
  aIndex += 2;
  try {
    Byte byte = std::stoul(byteStr, nullptr, 16);
    return byte;
  } catch (std::exception& e) {
    LOG(severity_level::error) << "getByte exception: " << e.what();
  }

  return std::nullopt;
}


std::optional<RecordType> IHexLineParser::getRecordType(const Byte aByte)
{
  if (aByte>static_cast<int>(RecordType::StartLinearAddress)) {
    return std::nullopt;
  }
  return static_cast<RecordType>(aByte);
}

bool IHexLineParser::verifyChecksum(const Record& aRecord)
{
  uint8_t sum = 0;
  sum = aRecord.byteCount
        + ((aRecord.address >> 8) & 0xFF)
        + (aRecord.address & 0xFF)
        + static_cast<uint8_t>(aRecord.recordType)
        + std::accumulate(aRecord.data.begin(), aRecord.data.end(), 0);

  sum ^= 0xFF;
  sum++;
  return sum == aRecord.checksum;
}

