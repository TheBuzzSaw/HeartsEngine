#include "Card.hpp"
using namespace std;

int main(int argc, char** argv)
{
    Card deck[52];

    int index = 0;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 2; j <= 14; ++j)
            deck[index++] = Card(j, i);
    }

    for (int i = 0; i < 52; ++i)
        cout << ' ' << deck[i];

    cout << endl;

    cout << sizeof(deck) << endl;

    return 0;
}
