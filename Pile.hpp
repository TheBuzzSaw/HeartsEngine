#ifndef PILE_HPP
#define PILE_HPP

#include "Card.hpp"
#include <algorithm>
#include <random>

template<uint8_t N>
class Pile
{
    public:
        Pile() : _count(0) {}
        Pile(const Pile&) = default;
        ~Pile() = default;

        Pile& operator=(const Pile&) = default;

        void Push(Card card) { _cards[_count++] = card; }
        Card Pop() { return _cards[--_count]; }
        void Clear() { _count = 0; }
        void Sort() { std::sort(_cards, _cards + _count); }
        void Shuffle(std::mt19937& mt) { shuffle(_cards, _cards + _count, mt); }

        Card Remove(size_t index)
        {
            Card result = _cards[index];

            for (size_t i = index + 1; i < _count; ++i)
                _cards[i - 1] = _cards[i];

            --_count;
            return result;
        }

        size_t Count() const { return _count; }
        bool IsEmpty() const { return _count == 0; }
        Card Get(size_t index) const { return _cards[index]; }

    protected:
    private:
        uint8_t _count;
        Card _cards[N];
};

template<uint8_t N>
std::ostream& operator<<(std::ostream& stream, const Pile<N>& pile)
{
    if (pile.Count() > 0)
    {
        stream << pile.Get(0);

        for (size_t i = 1; i < pile.Count(); ++i)
            stream << ", " << pile.Get(i);
    }

    return stream;
}

#endif
