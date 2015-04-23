#ifndef GAME_HPP
#define GAME_HPP

struct GameParameters
{
    int pointLimit = 100;
    const char* scripts[4] = {nullptr, nullptr, nullptr, nullptr};
};

void PlayGame(const GameParameters& gp = GameParameters());

#endif
