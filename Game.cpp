#include "Game.hpp"
#include "Pile.hpp"
#include <chrono>
using namespace std;

const int Passes[4] = { -1, +1, +2, 0 };

struct Player
{
    int localScore = 0;
    int overallScore = 0;
    Pile<13> hand;
};

struct Game
{
    mt19937 mt;
    bool heartsBroken = false;
    int pass = 3;
    Player players[4];
};

void PlayGame()
{
    Game game;

    game.mt.seed(chrono::system_clock::now().time_since_epoch().count());

    Pile<52> deck;

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 2; j <= 14; ++j)
            deck.Push(Card(j, i));
    }

    deck.Shuffle(game.mt);

    for (int i = 0; i < 52; ++i)
        game.players[i & 3].hand.Push(deck.Pop()); // Deal like a human!

    for (int i = 0; i < 4; ++i) game.players[i].hand.Sort();

    for (int i = 0; i < 4; ++i)
        cout << "Player " << (i + 1) << ": " << game.players[i].hand << endl;
}
