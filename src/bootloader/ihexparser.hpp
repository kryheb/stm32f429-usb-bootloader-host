/*
 * file   ihexparser.hpp
 * author Krystian Heberlein
 * email  krystianheberlein@gmail.com
 *
 * intel hex parser
 */

#include <vector>
#include <fstream>
#include <stdint.h>
#include <string>
#include <list>
#include <optional>
#include "utils/log/log.hpp"


enum class RecordType {
  Data,
  EndOfFile,
  ExtendedSegmentAddress,
  StartSegmentAddress,
  ExtendedLinearAddress,
  StartLinearAddress
};

enum class State {
  NoState,
  StartCode,
  ByteCount,
  Address,
  RecordType,
  Data,
  Checksum,
  Done,
  Error
};

using Byte = uint8_t;
using Bytes = std::vector<Byte>;

struct Record {
  Byte byteCount;
  uint16_t address;
  RecordType recordType;
  Bytes data;
  Byte checksum;

  Bytes rawRecord() const {
    uint8_t addrMSB = (address >> 8) & 0xFF;
    uint8_t addrLSB = (address & 0xFF);
    Bytes rr = { byteCount, addrMSB, addrLSB, static_cast<uint8_t>(recordType), /*data,*/ checksum };
    rr.insert(rr.begin() + 4, data.begin(), data.end());
    return rr;
  }
};
using Records = std::list<Record>;

class IHexParser {

  std::ifstream mFile;
  Records mRecords;

  public:
  IHexParser();

  bool openFile(const std::string& aFileName);
  void parseFile();
  size_t getDataSize() { return mRecords.size(); }
  std::optional<Record> popRecord();

};


class IHexLineParser
{
  State state = State::NoState;
  LoggerChannel& mLoggerChannel;

  public:
  IHexLineParser();
  std::optional<Record> parseLine(const std::string& aLine);

  private:
  std::optional<Byte> getByte(const std::string& aDataStr, size_t& aIndex);
  std::optional<RecordType> getRecordType(const Byte aByte);
  bool verifyChecksum(const Record& aRecord);
};
