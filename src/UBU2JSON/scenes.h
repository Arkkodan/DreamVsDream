#ifndef UBU2JSON_SCENES_H
#define UBU2JSON_SCENES_H

#include "ubu.h"

namespace ubu {
	class Scene : public UBU {
	public:
		Scene(const std::string& file);

		// SCENE's parse should be final but call a parseLine virtual override
		virtual bool parse() override final;
		virtual void parseLine();

	private:
		int imageIndex;
	};

	class Intro : public Scene {
	public:
		Intro(const std::string& file);
		virtual void parseLine() override final;
	};

	class Title : public Scene {
	public:
		Title(const std::string& file);
		virtual void parseLine() override final;

	private:
		int themeIndex;
	};

	class Select : public Scene {
	public:
		Select(const std::string& file);
		virtual void parseLine() override final;

	private:
		int gridC;
		int selectIndex;
	};

	class Options : public Scene {
	public:
		Options(const std::string& file);
		virtual void parseLine() override final;

	private:
		int themeIndex;
	};

	class Fight : public Scene {
	public:
		Fight(const std::string& file);
		virtual void parseLine() override final;
	};

	class Netplay : public Scene {
	public:
		Netplay(const std::string& file);
		virtual void parseLine() override final;
	};

	class Credits : public Scene {
	public:
		Credits(const std::string& file);
		virtual void parseLine() override final;
	};
}

#endif // UBU2JSON_SCENES_H
