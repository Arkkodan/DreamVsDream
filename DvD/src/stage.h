#ifndef STAGE_H_INCLUDED
#define STAGE_H_INCLUDED

#include "menu.h"

extern int stage;
#define STAGE stages[stage]

class Stage
{
public:
    std::string name;
    MenuImage* imagesAbove;
    MenuImage* imagesBelow;
    Music bgm;
    Music bgm2;
	
	bool initialized;

    int width;
    int height;
    int widthAbs;
    int heightAbs;

    Stage();
    //Stage(const char* _name);
    ~Stage();

    void create(std::string _name);
	
	void init();

    void think();
    void reset();
    void draw(bool _above);

    void bgmPlay();

    void parseFile(std::string szFileName);
	std::string getResource(std::string szFileName, std::string extension);

    static void ginit();
    static void deinit();
};

extern Stage stages[];
extern const char* sz_stages[];

#endif // STAGE_H_INCLUDED
