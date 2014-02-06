#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include "globals.h"

#include <errno.h>

void error(const std::string& sz);
void die(const std::string& sz);

#ifdef DEBUG
void debug(const std::string& sz);
#endif

#endif // ERROR_H_INCLUDED
