#include "Card.hpp"
#include <random>
#include <algorithm>
using namespace std;

int main(int argc, char** argv)
{
    Card deck[52];

    auto c = deck;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 2; j <= 14; ++j)
            *c++ = Card(j, i);
    }

    mt19937 mt;
    shuffle(deck + 0, deck + 52, mt);

    for (int i = 0; i < 52; ++i)
        cout << deck[i] << '\n';

    cout << endl;

    sort(deck, deck + 52);

    for (int i = 0; i < 52; ++i)
        cout << deck[i] << '\n';

    cout << endl;

    cout << sizeof(deck) << endl;

    return 0;
}
