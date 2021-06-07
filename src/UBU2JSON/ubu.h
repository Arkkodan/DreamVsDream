#ifndef UBU2JSON_UBU_H
#define UBU2JSON_UBU_H

#include "../DvD/parser.h"

#include <string>

#include <nlohmann/json.hpp>

namespace ubu {
	enum class Type {
		CHARACTER,
		SPRITES,
		SOUNDS,
		VOICES,
		// STATES,
		// COMMANDS,
		
		// SCENE,
		// INTRO,
		// TITLE, // Themes included
		// SELECT,
		// // Versus is the same as Scene
		// OPTIONS, // Themes included
		// FIGHT,
		// NETPLAY,
		// CREDITS,
		// TODO: SCENE's parse should be final but call a parseLine virtual override

		// STAGE,

		UBU_TYPE_MAX
	};

	void init();
	bool load(const std::string& type, const std::string& file, const std::string& output = "");
	void usage();

	class UBU
	{
	protected:
		UBU(const std::string& file);

	public:
		virtual bool parse() = 0;
		virtual void write(const std::string& output) const final;

	protected:
		Parser parser;
		nlohmann::ordered_json j_obj;
	};

}


#endif // UBU2JSON_UBU_H
