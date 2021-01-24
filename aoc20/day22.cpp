#include "pch.h"
#include "harness.h"


using Deck = deque<uint8_t>;    // nice

int checksum(const Deck& deck)
{
    int chk = 0;
    int pos = 1;
    for (auto it = deck.rbegin(); it != deck.rend(); ++it, ++pos)
        chk += pos * *it;
    return chk;
}

tuple<Deck, Deck> read_decks(const stringlist& input)
{
    Deck p1, p2;

    bool reading_p1 = true;
    for (auto& line : input)
    {
        if (line.empty())
        {
            reading_p1 = false;
            continue;
        }
        if (line.back() == ':')
            continue;

        Deck& deck = reading_p1 ? p1 : p2;
        deck.push_back(uint8_t(atoi(line.c_str())));
    }

    return make_tuple(p1, p2);
}

pair<bool,Deck> did_p1_win(Deck p1, Deck p2)
{
    vector<tuple<int, int>> prior_states;

    while (!p1.empty() && !p2.empty())
    {
        auto state = make_tuple(checksum(p1), checksum(p2));
        if (ranges::find(prior_states, state) != end(prior_states))
            return { true, p1 };
        prior_states.push_back(state);

        bool p1_wins_round;
        size_t c1 = p1.front(); p1.pop_front();
        size_t c2 = p2.front(); p2.pop_front();
        if (c1 <= p1.size() && c2 <= p2.size())
        {
            p1_wins_round = did_p1_win(Deck(begin(p1), begin(p1) + c1), Deck(begin(p2), begin(p2) + c2)).first;
        }
        else
        {
            p1_wins_round = c1 > c2;
        }

        if (p1_wins_round)
        {
            p1.push_back(uint8_t(c1));
            p1.push_back(uint8_t(c2));
        }
        else
        {
            p2.push_back(uint8_t(c2));
            p2.push_back(uint8_t(c1));
        }
    }

    return p1.empty() ? make_pair(false, p2) : make_pair(true, p1);
}

int day22(const stringlist& input)
{
    auto [p1, p2] = read_decks(input);

    int nturns = 0;
    while (!p1.empty() && !p2.empty())
    {
        ++nturns;

        int c1 = p1.front(); p1.pop_front();
        int c2 = p2.front(); p2.pop_front();

        if (c1 > c2)
        {
            p1.push_back(uint8_t(c1));
            p1.push_back(uint8_t(c2));
        }
        else
        {
            p2.push_back(uint8_t(c2));
            p2.push_back(uint8_t(c1));
        }
    }

    return checksum(p1.empty() ? p2 : p1);
}

int day22_2(const stringlist& input)
{
    auto [p1, p2] = read_decks(input);
    auto [winner, deck] = did_p1_win(p1, p2);
    return checksum(deck);
}


void run_day22()
{
    string sample =
R"(Player 1:
9
2
6
3
1

Player 2:
5
8
4
7
10)";

    test(306, day22(READ(sample)));
    gogogo(day22(LOAD(22)));

    test(291, day22_2(READ(sample)));
    nononoD(day22_2(LOAD(22)));
}