#ifndef UBU2JSON_STAGES_H
#define UBU2JSON_STAGES_H

#include "ubu.h"

namespace ubu {
	class Stage : public UBU
	{
	public:
		Stage(const std::string& file);

		virtual bool parse() override final;

	private:
		void parseImage(bool above);
		void parseBGM(int index);

	private:
		int imageAbove, imageBelow;
	};
}

#endif // UBU2JSON_STAGES_H
