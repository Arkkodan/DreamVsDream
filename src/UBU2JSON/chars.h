#ifndef UBU2JSON_CHARS_H
#define UBU2JSON_CHARS_H

#include "ubu.h"

namespace ubu {
	class Character : public UBU {
	public:
		Character(const std::string& file);

		virtual bool parse() override final;
	};

	class Sprites : public UBU {
	public:
		Sprites(const std::string& file);

		virtual bool parse() override final;
	};

	class Sounds : public UBU {
	public:
		Sounds(const std::string& file);

		virtual bool parse() override final;
	};

	class Voices : public UBU {
	public:
		Voices(const std::string& file);

		virtual bool parse() override final;
	};

	class States : public UBU {
	public:
		States(const std::string& file);

		virtual bool parse() override final;
	};

	class Commands : public UBU {
	public:
		Commands(const std::string& file);

		virtual bool parse() override final;
	};
}

#endif // UBU2JSON_CHARS_H
