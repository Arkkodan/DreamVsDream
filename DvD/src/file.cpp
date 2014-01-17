#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#ifndef NO_ZLIB
#include <zlib.h>
#endif

#include "globals.h"
#include "file.h"
#include "util.h"

File::File()
{
    fp = NULL;
    flags = 0;
}

File::~File()
{
    close();
}

bool File::open(int flags_, std::string szFileName_)
{
    //Prepare the correct flag string
    const char* szFlags = "rb";
    if(flags_ & FILE_IO_WRITE)
        szFlags = "wb";

    szFileName = szFileName_;

    //Open either a FILE* or a gzFile
#ifndef NO_ZLIB
    if(flags_ & FILE_COMPRESS_GZ)
    {
        FILE* _fp = fopen8(szFileName_ + ".gz", szFlags);
        fp = (void*)gzdopen(fileno(_fp), szFlags);
        if(_fp && !fp)
            fclose(_fp);
    }
    else
#endif
        fp = (void*)fopen8(szFileName, szFlags);
    if(!fp) return false;
    flags = flags_;
    return true;
}

void File::close()
{
    szFileName = "";
	
    if(fp)
    {
#ifndef NO_ZLIB
        if(flags & FILE_COMPRESS_GZ)
            gzclose((gzFile)fp);
        else
#endif
            fclose((FILE*)fp);
        fp = NULL;
    }
}

//READ OPERATIONS
bool File::read(void* data_, size_t size_)
{
#ifndef NO_ZLIB
    if(flags & FILE_COMPRESS_GZ)
        return gzread((gzFile)fp, data_, size_) == (int)size_;
#endif
    return fread(data_, 1, size_, (FILE*)fp) == size_;
}

int8_t File::readByte()
{
    int8_t _value = 0;
    read(&_value, 1);
    return _value;
}

int16_t File::readWord()
{
    int16_t _value = 0;
    read(&_value, 2);
    return _value;
}

int32_t File::readDword()
{
    int32_t _value = 0;
    read(&_value, 4);
    return _value;
}

float File::readFloat()
{
    int32_t _value_int = 0;
    read(&_value_int, 4);
    return _value_int / (float)FLOAT_ACCURACY;
}

std::string File::readStr()
{
    uint8_t size = readByte();
	if(!size)
		return "";
	char* sz = (char*)malloc(size + 1);
    sz[size] = 0;
    read(sz, size);
	std::string str(sz);
	free(sz);
	return str;
}

//WRITE OPERATIONS
bool File::write(const void* data_, size_t size_)
{
#ifndef NO_ZLIB
    if(flags & FILE_COMPRESS_GZ)
        return gzwrite((gzFile)fp, data_, size_) == (int)size_;
#endif
    return fwrite(data_, 1, size_, (FILE*)fp) == size_;
}

bool File::writeByte(int8_t value_)
{
    return write(&value_, 1);
}

bool File::writeWord(int16_t value_)
{
    return write(&value_, 2);
}

bool File::writeDword(int32_t value_)
{
    return write(&value_, 4);
}

bool File::writeFloat(float value_)
{
    int32_t valueInt = value_ * FLOAT_ACCURACY;
    return write(&valueInt, 4);
}

bool File::writeStr(std::string value_)
{
    uint8_t size = value_.length();
    if(!write(&size, 1))
		return false;
	if(!size)
		return true;
    return write(value_.c_str(), size);
}

void File::seek(long index_)
{
#ifndef NO_ZLIB
    if(flags & FILE_COMPRESS_GZ)
        gzseek((gzFile)fp, index_, SEEK_SET);
    else
#endif
        fseek((FILE*)fp, index_, SEEK_SET);

}

long File::tell()
{
#ifndef NO_ZLIB
    if(flags & FILE_COMPRESS_GZ)
        return gztell((gzFile)fp);
#endif
    return ftell((FILE*)fp);
}

size_t File::size()
{
    if(flags & FILE_COMPRESS_GZ) return -1;
    long _pos = ftell((FILE*)fp);
    fseek((FILE*)fp, 0, SEEK_END);
    size_t _size = ftell((FILE*)fp);
    fseek((FILE*)fp, _pos, SEEK_SET);
    return _size;
}

std::string File::getFilename()
{
    return szFileName;
}
