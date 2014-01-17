#include "globals.h"
#include "player.h"
#include "input.h"
#include "menu.h"
#include "network.h"

enum
{
    KEY_1_LEFT,
    KEY_1_RIGHT,
    KEY_1_UP,
    KEY_1_DOWN,
    KEY_1_A,
    KEY_1_B,
    KEY_1_C,

    KEY_2_LEFT,
    KEY_2_RIGHT,
    KEY_2_UP,
    KEY_2_DOWN,
    KEY_2_A,
    KEY_2_B,
    KEY_2_C,

    KEY_MAX
};

#define KEY_1_MAX KEY_2_LEFT
#define KEY_2_MAX KEY_MAX

int key_config[KEY_MAX] =
{
    SDLK_LEFT,
    SDLK_RIGHT,
    SDLK_UP,
    SDLK_DOWN,
    SDLK_n,
    SDLK_m,
    SDLK_COMMA,

    SDLK_f,
    SDLK_h,
    SDLK_t,
    SDLK_g,
    SDLK_a,
    SDLK_s,
    SDLK_d,
};

#define KEY_CFG_FILE "keys.cfg"

extern Player madotsuki;
extern Player poniko;

namespace Input
{
    void init()
    {
        //Read controls from file
        int nLines;
        char** szLines = getLinesFromFile(&nLines, szConfigPath + KEY_CFG_FILE);

        if(nLines == KEY_MAX)
        {
            for(int i = 0; i < KEY_MAX; i++)
            {
                if(strlen(szLines[i]) >= 2 && szLines[i][0] == '0' && szLines[i][1] == 'x')
                    key_config[i] = strtol((const char*)szLines[i], NULL, 16);
                else
                    key_config[i] = atoi(szLines[i]);
            }
        }

        freeLines(szLines);
    }

    void refresh()
    {
        madotsuki.applyInput();
        poniko.applyInput();
        if(menu == 0)
        {
            madotsuki.handleInput();
            poniko.handleInput();
        }
    }

    void keyPress(int key, bool press)
    {
        if(press)
        {
            for(int i = 0; i < KEY_MAX; i++)
            {
                if(Network::connected && i >= KEY_1_MAX)
                    break;

                if(key_config[i] == key)
                {
                    if(Network::connected)
                    {
                        Network::getMyPlayer()->frameInput |= (1<<i);
                    }
                    else
                    {
                        if(i < KEY_1_MAX)
                            madotsuki.frameInput |= (1<<i);
                        else
                            poniko.frameInput |= (1<<(i - KEY_1_MAX));
                    }
                    return;
                }
            }

            switch(key)
            {
            case SDLK_F1:
                if(!Network::connected)
                    Menu::setMenu(MENU_TITLE);
                break;

            case SDLK_F2:
                if(!Network::connected)
                    Menu::setMenu(MENU_CREDITS);
                break;
            }
        }
        else
        {
            for(int i = 0; i < KEY_MAX; i++)
            {
                if(Network::connected && i >= KEY_1_MAX)
                    break;

                if(key_config[i] == key)
                {
                    if(Network::connected)
                    {
                        Network::getMyPlayer()->frameInput |= (1<<i<<INPUT_RELSHIFT);
                    }
                    else
                    {
                        if(i < KEY_1_MAX)
                            madotsuki.frameInput |= (1<<i<<INPUT_RELSHIFT);
                        else
                            poniko.frameInput |= (1<<(i - KEY_1_MAX)<<INPUT_RELSHIFT);
                    }
                    return;
                }
            }
        }
    }
}
