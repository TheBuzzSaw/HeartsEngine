#ifndef CARD_HPP
#define CARD_HPP

#include <iostream>
#include <cstdint>

const int Clubs = 0;
const int Diamonds = 1;
const int Spades = 2;
const int Hearts = 3;

class Card
{
    public:
        Card(int value, int suit) : _data((suit << 6) | value) {}
        Card(const Card&) = default;
        ~Card() = default;

        Card& operator=(const Card&) = default;

        int Value() const { return _data & 0x3f; }
        int Suit() const { return _data >> 6; }

    protected:
    private:
        uint8_t _data;
};

#endif
