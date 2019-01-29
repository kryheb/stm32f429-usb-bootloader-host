
#include <vector>
#include <fstream>
#include <stdint.h>
#include <string>
#include <list>
#include <boost/optional.hpp>


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

  bool openFile(const std::string& pFileName);
  void parseFile();
  size_t getDataSize() { return mRecords.size(); }
  boost::optional<Record> popRecord();

};


class IHexLineParser
{
  State state = State::NoState;

  public:
  IHexLineParser();
  boost::optional<Record> parseLine(const std::string& pLine);

  private:
  boost::optional<Byte> getByte(const std::string& pDataStr, size_t& pIndex);
  boost::optional<RecordType> getRecordType(const Byte aByte);
  bool verifyChecksum(const Record& pRecord);
};