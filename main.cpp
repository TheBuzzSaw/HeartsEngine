#include "Game.hpp"
using namespace std;

int main(int argc, char** argv)
{
    GameParameters gp;

    int scriptCount = 0;
    for (int i = 1; i < argc; ++i)
    {
        if (scriptCount < 4)
            gp.scripts[scriptCount++] = argv[i];
    }

    PlayGame(gp);
    return 0;
}
