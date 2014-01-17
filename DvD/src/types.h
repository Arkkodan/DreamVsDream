#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

typedef char byte_t;
typedef unsigned char ubyte_t;

#ifdef _WIN32
#ifndef WINVER
#define WINVER 0x0500
#endif
#include <windows.h>
#endif

#endif
