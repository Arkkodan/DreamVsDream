#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <sstream>
#include <algorithm>

#ifndef _WIN32
#include <dirent.h>
#include <sys/stat.h>
#else
#ifndef WINVER
#define WINVER 0x0500
#endif
#include <windows.h>
#include <shlwapi.h>
#endif

#if(_MSC_VER) // MSVC uses _stricmp instead of strcasecmp
#define strcasecmp(str1, str2) _stricmp(str1, str2)
#endif

namespace util {
	Vector::Vector() :
	    x(0), y(0)
	{}

    Vector::Vector(const Vector& other) :
        x(other.x), y(other.y)
    {}

    Vector::Vector(Vector&& other) :
        x(other.x), y(other.y)
    {}

    Vector& Vector::operator=(const Vector& other) {
        x = other.x;
        y = other.y;
        return *this;
    }

    Vector& Vector::operator=(Vector&& other) {
        x = other.x;
        y = other.y;
        return *this;
    }

	Vector::Vector(int x, int y) :
	    x(x), y(y)
	{}

	Vector& Vector::operator+=(const Vector& other) {
		x += other.x;
		y += other.y;
		return *this;
	}

	Vector& Vector::operator-=(const Vector& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Vector& Vector::operator*=(const int scalar) {
		x *= scalar;
		y *= scalar;
		return *this;
	}

	Vector Vector::operator*(const int scalar) {
		Vector result;
		return result *= scalar;
	}

	Vectorf::Vectorf() :
	    x(0.0), y(0.0)
    {}

    Vectorf::Vectorf(const Vectorf& other) :
        x(other.x), y(other.y)
    {}

    Vectorf::Vectorf(Vectorf&& other) :
        x(other.x), y(other.y)
    {}

    Vectorf& Vectorf::operator=(const Vectorf& other) {
        x = other.x;
        y = other.y;
        return *this;
    }

    Vectorf& Vectorf::operator=(Vectorf&& other) {
        x = other.x;
        y = other.y;
        return *this;
    }

	Vectorf::Vectorf(float x, float y) :
	    x(x), y(y)
    {}

	Vectorf& Vectorf::operator+=(const Vectorf& other) {
		x += other.x;
		y += other.y;
		return *this;
	}

	Vectorf& Vectorf::operator-=(const Vectorf& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Vectorf& Vectorf::operator*=(const float scalar) {
		x *= scalar;
		y *= scalar;
		return *this;
	}

	Vectorf Vectorf::operator*(const float scalar) {
		Vectorf result;
		return result *= scalar;
	}

	FILE* ufopen(const std::string& szFileName, const char* flags) {
#ifdef _WIN32
		// Windows requires an inner getPath call
		// Use a temporary hack
		wchar_t* filename16 = getPathUtf16(getPath(szFileName));
		wchar_t* flags16 = utf8to16(flags);
		FILE* file = _wfopen(filename16, flags16);
		free(flags16);
		free(filename16);
#else
		FILE* file = fopen(szFileName.c_str(), flags);
#endif
		return file;
	}

#ifdef _WIN32
	//Character set conversion
	char* utf16to8(const wchar_t* string) {

		int size = WideCharToMultiByte(CP_UTF8, 0, (const wchar_t*)string, -1, nullptr, 0, nullptr, nullptr);
		if(size) {
			char* str8 = (char*)malloc(sizeof(char) * size);
			if(WideCharToMultiByte(CP_UTF8, 0, (const wchar_t*)string, -1, (char*)str8, size, nullptr, nullptr)) {
				return str8;
			}
			free(str8);
		}
		return nullptr;
	}

	wchar_t* utf8to16(const char* string) {
		int size = MultiByteToWideChar(CP_UTF8, 0, (const char*)string, -1, nullptr, 0);
		if(size) {
			wchar_t* str16 = (wchar_t*)malloc(sizeof(wchar_t) * size);
			if(MultiByteToWideChar(CP_UTF8, 0, (const char*)string, -1, str16, size)) {
				return str16;
			}
			free(str16);
		}
		return nullptr;
	}
	
	wchar_t* getPathUtf16(const std::string& filename) {
		return utf8to16(("\\\\?\\" + filename).c_str());
	}
#endif

	std::string getPath(const std::string& filename) {
#ifdef _WIN32
		static std::string wd;
		if(wd.empty()) {
			//Allocate the module filename from the brain-damaged win32 api
			int size = 1;
			wchar_t* wd16 = nullptr;
			do {
				//Allocate buffer
				free(wd16);
				size *= 2;
				wd16 = (wchar_t*)malloc(size * 2);
			} while(GetModuleFileNameW(nullptr, wd16, size) == size);
			PathRemoveFileSpecW(wd16);
			PathAddBackslashW(wd16);
			
			//Convert to utf-8
			char* wd8 = utf16to8(wd16);
			free(wd16);
			
			//Allocate the c++ string; skip over \\?\ if it exists
			if(memcmp(wd8, "\\\\?\\", 4)) {
				wd = std::string(wd8);
			} else {
				wd = std::string(wd8 + 4);
			}
#ifdef GAME
			wd += "data\\";
#endif
			
			free(wd8);
		}
		
		//Concatonate long path specifier + working directory + filename
		//Then turn forward slashes into backslashes
		std::string buffer;
		if(PathIsRelativeA(filename.c_str())) {
			buffer = wd + filename;
		} else {
			buffer = filename;
		}
		std::replace(buffer.begin(), buffer.end(), '/', '\\');
		return buffer;
#else
#ifdef GAME
		return "data/" + filename;
#else
		return filename;
#endif
#endif
	}
	
	bool fileExists(const std::string& filename) {
#ifdef _WIN32
		wchar_t* sz_file16 = util::getPathUtf16(filename.c_str());
		DWORD dwAttrib = GetFileAttributesW(sz_file16);
		free(sz_file16);
		return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
		struct stat sts;
		return !(stat(filename.c_str(), &sts) == -1 && errno == ENOENT);
#endif
	}

	int isNewline(char* cs, int size) {
		unsigned char* c = (unsigned char*)cs;
		if(size >= 3) {
			//LS
			if(c[0] == 0xE2 && c[1] == 0x80 && c[2] == 0xA8) {
				return 3;
			}
			//PS
			if(c[0] == 0xE2 && c[1] == 0x80 && c[2] == 0xA9) {
				return 3;
			}
		}
		if(size >= 2) {
			//CR+LF
			if(c[0] == 0x0D && c[1] == 0x0A) {
				return 2;
			}
			//NEL
			if(c[0] == 0xC2 && c[1] == 0x85) {
				return 2;
			}
		}
		if(size >= 1) {
			//LF
			if(c[0] == 0x0A) {
				return 1;
			}
			//VT
			if(c[0] == 0x0B) {
				return 1;
			}
			//FF
			if(c[0] == 0x0C) {
				return 1;
			}
			//CR
			if(c[0] == 0x0D) {
				return 1;
			}
		}
		return 0;
	}

	int isWhitespace(char* cs, int size) {
		unsigned char* c = (unsigned char*)cs;
		if(size >= 2) {
			//NBS
			if(c[0] == 0xC2 && c[1] == 0xA0) {
				return 2;
			}
		}
		if(size >= 1) {
			//TAB
			if(c[0] == 0x09) {
				return 1;
			}
			//SPACE
			if(c[0] == 0x20) {
				return 1;
			}
			//ASCII NBS
			if(c[0] == 0xFF) {
				return 1;
			}
		}
		return 0;
	}

	void nextWord(char* cs, int index, int size, int* start, int* end) {
		unsigned char* c = (unsigned char*)cs;
		int wSpace;

		while(index < size) {
			if(c[index] == ',') {
				index++;
			} else if((wSpace = isWhitespace(cs + index, size - index))) {
				index += wSpace;
			} else {
				break;
			}
		}
		*start = index;
		for(; index < size && c[index] != ',' && c[index] != ']' && !isWhitespace(cs + index, size - index); index++);
		*end = index;
	}

	bool strtobool(const char* str, bool def) {
		if(!strcasecmp(str, "true")) {
			return true;
		}
		if(!strcasecmp(str, "false")) {
			return false;
		}
		return def;
	}

	char** getLinesFromFile(int* n, const std::string& szFile) {
		*n = 0;

		//Open the file
		FILE* f = ufopen(szFile, "rb");
		if(!f) {
			return 0;
		}

		//Read the entire file into memory
		fseek(f, 0, SEEK_END);
		long int sizeRaw = ftell(f);
		if(sizeRaw <= 0) {
			fclose(f);
			return nullptr;
		}
		fseek(f, 0, SEEK_SET);
		char* buffRaw = (char*)malloc(sizeRaw + 1);
		if(fread(buffRaw, sizeRaw, 1, f) != 1) {
			fclose(f);
			return nullptr;
		}
		fclose(f);

		//Add a \n on the end, just in case one doesn't exist already
		buffRaw[sizeRaw++] = '\n';

		//Determine if the BOM exists and remove it
		//We compare via > 3 because we incremented sizeRaw
		int i = 0;
		if(sizeRaw > 3) {
			if((unsigned char)buffRaw[0] == 0xEF && (unsigned char)buffRaw[1] == 0xBB && (unsigned char)buffRaw[2] == 0xBF) {
				buffRaw[0] = 0;
				buffRaw[1] = 0;
				buffRaw[2] = 0;
				i = 3;
			}
		}

		//Count lines and remove comments, beginning/trailing whitespace, and blank lines
		bool inLine = false;				//True if we're parsing the text in a line
		bool inComment = false;				//True if we're passing over a comment
		bool lastCharNonNewline = false;	//True if the last character parsed wasn't a newline or newline + whitespace
		int iFirstWhitespace = 0;			//Assumed first whitespace character (after last parsed non-whitespace character)

		long int size = 0;
		int nLines = 0;
		while(i < sizeRaw) {
			int wChar = 0;
			if((wChar = isNewline(buffRaw + i, sizeRaw - i))) {
				//Increment line counter and allocate a byte for the null terminator
				//if the last character wasn't a newline
				if(lastCharNonNewline) {
					nLines++;
					size++;
				}

				//Remove any trailing whitespace/comments + newline
				memset(buffRaw + iFirstWhitespace, 0, i - iFirstWhitespace + wChar);

				//Reset line flags
				inLine = false;
				inComment = false;
				lastCharNonNewline = false;

				//wChar bytes wide
				i += wChar;
				iFirstWhitespace = i;
			} else if((wChar = isWhitespace(buffRaw + i, sizeRaw - 1))) {
				//If we've already parsed text on this line, increase the buffer
				//size as normal. Otherwise, eliminate all beginning whitespace.
				if(inLine) {
					size += wChar;
				} else {
					memset(buffRaw + i, 0, wChar);
				}

				//wChar bytes wide
				i += wChar;
			} else {
				if(!inComment) {
					//Is this the start of a comment?
					if(buffRaw[i] == '/' && i < sizeRaw - 1 && buffRaw[i+1] == '/') {
						inComment = true;
					} else {
						//Indicate that this is the last non-whitespace character parsed in the line
						iFirstWhitespace = i + 1;
						lastCharNonNewline = true;
						size++;
					}

					//Indicate that we're in a line
					inLine = true;
				}
				//One byte wide
				i++;
			}
		}

		//Allocate the new text buffer and line buffer
		char* buff = (char*)malloc(size);
		char** lines = (char**)malloc(nLines * sizeof(char*));

		//Copy the data into the new buffer
		for(int line = 0, iBuff = 0, iRaw = 0; line < nLines; line++) {
			//Fast forward past groups of nulls
			for(; !buffRaw[iRaw]; iRaw++);

			//Save the pointer
			lines[line] = buff + iBuff;

			//Copy the data, stopping at the first null
			for(;;) {
				buff[iBuff++] = buffRaw[iRaw];
				if(buffRaw[iRaw]) {
					iRaw++;
				} else {
					break;
				}
			}
		}

		//Free the raw buffer and return, we're done!
		free(buffRaw);
		*n = nLines;
		return lines;
	}

	void freeLines(char** lines) {
		if(lines) {
			free(lines[0]);
			free(lines);
		}
	}

	//RANDOM NUMBER GENERATOR
	//Later on, we'll phase out using rand() because it sucks
	int roll(int size_) {
		return (int)(rollf() * size_); //This is more accurate than using modulus
	}

	int roll(int min_, int max_) {
		return roll(max_ - min_ + 1) + min_;
	}

	//Random number where 0 <= x < 1
	float rollf() {
		//ugly sunovabich
		return (float)((double)rand()/(double)((long long)RAND_MAX+1L));
	}

	//Number->string conversion
	std::string toString(int value) {
	    std::ostringstream ss;
	    ss << value;
	    return ss.str();
	}

	//Directory listings
	std::vector<std::string> listDirectory(const std::string& directory, bool listFiles) {
	    std::vector<std::string> result;

#ifdef _WIN32
        wchar_t* directory16 = getPathUtf16(directory + "\\*");

        WIN32_FIND_DATAW findData;
        HANDLE hFind = nullptr;

        if(!(hFind = FindFirstFileW(directory16, &findData))) {
            return result;
        }

        do {
            if(wcscmp(findData.cFileName, L".") && wcscmp(findData.cFileName, L"..")) {
                if(((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) == listFiles) {
                    char* file8 = utf16to8(findData.cFileName);
                    result.push_back(file8);
                    free(file8);
                }
            }
        } while(FindNextFileW(hFind, &findData));

        free(directory16);
#else
        DIR* dp = opendir(directory.c_str());
        if(dp == nullptr) {
            return result;
        }

        dirent* de;
        while((de = readdir(dp))) {
            //Don't do . and .. relative paths
            if(strcmp(de->d_name, ".") && strcmp(de->d_name, "..")) {
                struct stat st;
                lstat((directory + "/" + de->d_name).c_str(), &st);

                //Add to the vector if it's the right type
                if((S_ISDIR(st.st_mode) != 0) == !listFiles) {
                    result.push_back(std::string(de->d_name));
                }
            }
        }
        closedir(dp);
#endif

        return result;
	}
}
