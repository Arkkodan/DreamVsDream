#ifndef DVD_FILE_H
#define DVD_FILE_H

#include <cstdint>
#include <string>

/// @brief High-level file read/write
/// @details Designed for binary files, has its own RW protocol
class File {
public:
  static constexpr auto FILE_IO_READ = 0;
  static constexpr auto FILE_IO_WRITE = 1 << 0;
  static constexpr auto FILE_COMPRESS_NORMAL = 0;
  static constexpr auto FILE_COMPRESS_GZ = 1 << 1;

  static constexpr auto FILE_READ_NORMAL = FILE_IO_READ | FILE_COMPRESS_NORMAL;
  static constexpr auto FILE_READ_GZ = FILE_IO_READ | FILE_COMPRESS_GZ;
  static constexpr auto FILE_WRITE_NORMAL =
      FILE_IO_WRITE | FILE_COMPRESS_NORMAL;
  static constexpr auto FILE_WRITE_GZ = FILE_IO_WRITE | FILE_COMPRESS_GZ;

public:
  File();
  ~File();

  bool open(int flags, std::string szFilename);

  bool read(void *data, size_t size) const;
  int8_t readByte() const;
  int16_t readWord() const;
  int32_t readDword() const;
  float readFloat() const;
  std::string readStr() const;

  bool write(const void *data, size_t size) const;
  bool writeByte(int8_t) const;
  bool writeWord(int16_t) const;
  bool writeDword(int32_t) const;
  bool writeFloat(float) const;
  bool writeStr(const std::string &) const;

  void seek(long index) const;
  long tell() const;
  size_t size() const;

private:
  void close();

  void *fp;
  int flags;
};

#endif // DVD_FILE_H
