#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include "globals.h"

#include <errno.h>

void error(std::string sz);
void die(std::string sz);

#endif // ERROR_H_INCLUDED
