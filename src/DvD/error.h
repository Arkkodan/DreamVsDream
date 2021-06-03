#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include <errno.h>
#include <string>

namespace error {
	/// @brief Report a non-fatal warning
	void error(const std::string& sz);

	/// @brief Report a fatal error and terminate
	void die(const std::string& sz);

#ifdef DEBUG
	void debug(const std::string& sz);
#endif
}


#endif // ERROR_H_INCLUDED
