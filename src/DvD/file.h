#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

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
	static constexpr auto FILE_WRITE_NORMAL = FILE_IO_WRITE | FILE_COMPRESS_NORMAL;
	static constexpr auto FILE_WRITE_GZ = FILE_IO_WRITE | FILE_COMPRESS_GZ;

public:
	File();
	~File();

	bool open(int flags, std::string szFilename);

	bool read(void* data, size_t size);
	int8_t readByte();
	int16_t readWord();
	int32_t readDword();
	float readFloat();
	std::string readStr();

	bool write(const void* data, size_t size);
	bool writeByte(int8_t);
	bool writeWord(int16_t);
	bool writeDword(int32_t);
	bool writeFloat(float);
	bool writeStr(const std::string&);

	void seek(long index);
	long tell();
	size_t size();

private:
    void close();

	void* fp;
	int flags;
};

#endif // FILE_H_INCLUDED
