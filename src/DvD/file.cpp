#include "file.h"

#include "../util/fileIO.h"
#include "sys.h"

#include <cstdio>
#include <vector>

#ifndef NO_ZLIB
#include <zlib.h>
#endif

File::File() {
  fp = nullptr;
  flags = 0;
}

File::~File() { close(); }

bool File::open(int flags, std::string szFileName) {
  close();

  // Prepare the correct flag string
  const char *szFlags = "rb";
  if (flags & FILE_IO_WRITE) {
    szFlags = "wb";
  }

  this->flags = flags;

  // Open either a FILE* or a gzFile
#ifndef NO_ZLIB
  if (flags & FILE_COMPRESS_GZ) {
    FILE *f = util::ufopen(szFileName + ".gz", szFlags);
    fp = (void *)gzdopen(fileno(f), szFlags);
    if (f && !fp) {
      fclose(f);
    }
  }
  else
#endif
    fp = (void *)util::ufopen(szFileName, szFlags);
  if (!fp) {
    return false;
  }
  return true;
}

// READ OPERATIONS
bool File::read(void *data, size_t size) const {
#ifndef NO_ZLIB
  if (flags & FILE_COMPRESS_GZ) {
    return gzread((gzFile)fp, data, size) == (int)size;
  }
#endif
  return fread(data, 1, size, (FILE *)fp) == size;
}

int8_t File::readByte() const {
  int8_t _value = 0;
  read(&_value, 1);
  return _value;
}

int16_t File::readWord() const {
  int16_t _value = 0;
  read(&_value, 2);
  return _value;
}

int32_t File::readDword() const {
  int32_t _value = 0;
  read(&_value, 4);
  return _value;
}

float File::readFloat() const {
  int32_t _value_int = 0;
  read(&_value_int, 4);
  return _value_int / (float)sys::FLOAT_FIXED_ACCURACY;
}

std::string File::readStr() const {
  uint8_t size = readByte();
  if (!size) {
    return "";
  }
  std::vector<char> sz(size);
  read(sz.data(), size);
  std::string str(sz.begin(), sz.end());
  return str;
}

// WRITE OPERATIONS
bool File::write(const void *data, size_t size) const {
#ifndef NO_ZLIB
  if (flags & FILE_COMPRESS_GZ) {
    return gzwrite((gzFile)fp, data, size) == (int)size;
  }
#endif
  return fwrite(data, 1, size, (FILE *)fp) == size;
}

bool File::writeByte(int8_t value) const { return write(&value, 1); }

bool File::writeWord(int16_t value) const { return write(&value, 2); }

bool File::writeDword(int32_t value) const { return write(&value, 4); }

bool File::writeFloat(float value) const {
  int32_t valueInt = static_cast<int32_t>(value * sys::FLOAT_FIXED_ACCURACY);
  return write(&valueInt, 4);
}

bool File::writeStr(const std::string &value) const {
  uint8_t size = static_cast<uint8_t>(value.length());
  if (!write(&size, 1)) {
    return false;
  }
  if (!size) {
    return true;
  }
  return write(value.c_str(), size);
}

void File::seek(long index) const {
#ifndef NO_ZLIB
  if (flags & FILE_COMPRESS_GZ) {
    gzseek((gzFile)fp, index, SEEK_SET);
  }
  else
#endif
    fseek((FILE *)fp, index, SEEK_SET);
}

long File::tell() const {
#ifndef NO_ZLIB
  if (flags & FILE_COMPRESS_GZ) {
    return gztell((gzFile)fp);
  }
#endif
  return ftell((FILE *)fp);
}

size_t File::size() const {
  if (flags & FILE_COMPRESS_GZ) {
    return -1;
  }
  long pos = ftell((FILE *)fp);
  fseek((FILE *)fp, 0, SEEK_END);
  size_t size = ftell((FILE *)fp);
  fseek((FILE *)fp, pos, SEEK_SET);
  return size;
}

// Private
void File::close() {
  if (fp) {
#ifndef NO_ZLIB
    if (flags & FILE_COMPRESS_GZ) {
      gzclose((gzFile)fp);
    }
    else
#endif
      fclose((FILE *)fp);
    fp = nullptr;
  }
}
