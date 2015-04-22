#ifndef CARD_HPP
#define CARD_HPP

#include <iostream>
#include <cstdint>

const int Clubs = 0;
const int Diamonds = 1;
const int Spades = 2;
const int Hearts = 3;

const int Jack = 11;
const int Queen = 12;
const int King = 13;
const int Ace = 14;

class Card
{
    public:
        Card() {}
        Card(int value, int suit) : _data((suit << 6) | value) {}
        Card(const Card&) = default;
        ~Card() = default;

        Card& operator=(const Card&) = default;

        int Value() const { return _data & 0x3f; }
        int Suit() const { return _data >> 6; }

        bool operator==(const Card& other) { return _data == other._data; }
        bool operator<(const Card& other) { return _data < other._data; }

    protected:
    private:
        uint8_t _data;
};

std::ostream& operator<<(std::ostream& stream, Card card)
{
    switch (card.Value())
    {
        case Jack: stream << "Jack"; break;
        case Queen: stream << "Queen"; break;
        case King: stream << "King"; break;
        case Ace: stream << "Ace"; break;
        default: stream << card.Value(); break;
    }

    stream << " of ";

    switch (card.Suit())
    {
        case Clubs: stream << "clubs"; break;
        case Diamonds: stream << "diamonds"; break;
        case Spades: stream << "spades"; break;
        case Hearts: stream << "hearts"; break;
        default: stream << "BLAM"; break;
    }

    return stream;
}

#endif
