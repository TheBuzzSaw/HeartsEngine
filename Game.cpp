#include "Game.hpp"
#include "Pile.hpp"
using namespace std;

struct Player
{
    int localScore = 0;
    int overallScore = 0;
    Pile<13> hand;
};

void PlayGame()
{
    bool heartsBroken = false;
    Player players[4];

    Pile<52> deck;

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 2; j <= 14; ++j)
            deck.Push(Card(j, i));
    }

    mt19937 mt;
    deck.Shuffle(mt);

    for (int i = 0; i < 52; ++i)
        players[i & 3].hand.Push(deck.Pop()); // Deal like a human!

    for (int i = 0; i < 4; ++i) players[i].hand.Sort();

    for (int i = 0; i < 4; ++i)
        cout << "Player " << (i + 1) << ": " << players[i].hand << endl;
}
