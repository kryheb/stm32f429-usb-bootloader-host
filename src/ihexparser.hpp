
#include <vector>
#include <fstream>
#include <stdint.h>
#include <string>
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
  Error
};

using Byte = uint8_t;
using Bytes = std::vector<Byte>;

struct Record {
  Byte byteCount;
  uint16_t adddress;
  RecordType recordCount;
  Bytes data;
  Byte checksum;
};


class IHexParser {

  std::ifstream file;

  public:
  IHexParser();

  bool openFile(const std::string& pFileName);

};


class IHexLineParser
{
  State state = State::NoState;

  public:
  IHexLineParser();
  boost::optional<Record> parseLine(const std::string& pLine);

  private:
  boost::optional<Byte> getByte(const std::string& pDataStr, size_t& pIndex);
};