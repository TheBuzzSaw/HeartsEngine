#include "Card.hpp"
using namespace std;

int main(int argc, char** argv)
{
    Card c(2, Hearts);

    cout << c.Value() << " of " << c.Suit() << endl;

    return 0;
}
