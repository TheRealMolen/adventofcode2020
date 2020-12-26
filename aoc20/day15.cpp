#include "pch.h"
#include "harness.h"


int day15(const string& input, int target_turn = 2020)
{
    vector<int> lastplayed;
    int turn = 1;

    // set up starting moves
    for (const string& move : split(input, ","))
    {
        int i = atoi(move.c_str());
        if (size(lastplayed) <= size_t(i))
            lastplayed.resize(i + 1, -1);

        _ASSERT(lastplayed[i] == -1);
        lastplayed[i] = turn;
        ++turn;
    }

    // assume starting sequence doesn't have dupes
    int nextmove = 0;

    for (;;)
    {
        if (turn == target_turn)
            return nextmove;

        int move = nextmove;
        if (size(lastplayed) <= size_t(move))
            lastplayed.resize(move + 1, -1);

        if (lastplayed[move] == -1)
            nextmove = 0;
        else
            nextmove = turn - lastplayed[move];

        lastplayed[move] = turn;
        ++turn;
    }

    return -1;
}

int day15_2(const string& input)
{
    return day15(input, 30000000);
}


void run_day15()
{
    test(436, day15("0,3,6"));
    test(1, day15("1,3,2"));
    test(10, day15("2,1,3"));
    test(27, day15("1,2,3"));
    test(78, day15("2,3,1"));
    test(438, day15("3,2,1"));
    test(1836, day15("3,1,2"));
    gogogo(day15("1,0,16,5,17,4"));

    nest(175594, day15_2("0,3,6"));
    nest(2578, day15_2("1,3,2"));
    nest(3544142, day15_2("2,1,3"));
    nest(261214, day15_2("1,2,3"));
    nest(6895259, day15_2("2,3,1"));
    nest(18, day15_2("3,2,1"));
    nest(362, day15_2("3,1,2"));
    nonono(day15_2("1,0,16,5,17,4"));
}