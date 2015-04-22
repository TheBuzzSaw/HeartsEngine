#include "Pile.hpp"
using namespace std;

int main(int argc, char** argv)
{
    Pile<52> deck;

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 2; j <= 14; ++j)
            deck.Push(Card(j, i));
    }

    mt19937 mt;
    deck.Shuffle(mt);

    Pile<13> hand;
    for (int i = 0; i < 13; ++i)
        hand.Push(deck.Pop());

    hand.Sort();

    cout << "My Hand: " << hand << endl;

    cout << sizeof(Card) << endl;

    return 0;
}
