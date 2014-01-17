#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include <errno.h>
#include <string>

#include "types.h"

void error(std::string sz);
void die(std::string sz);

#endif // ERROR_H_INCLUDED
