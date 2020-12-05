#include "pch.h"
#include "harness.h"


uint16_t calc_seatid(const string& seatname)
{
    uint16_t id = 0;
    uint16_t mask = 1 << 9;
    for (int i = 0; i < 7; ++i, mask >>= 1)
        if (seatname[i] == 'B')
            id |= mask;
    for (int i = 7; i < 10; ++i, mask >>= 1)
        if (seatname[i] == 'R')
            id |= mask;
    return id;
}

int day5(const stringlist& input)
{
    int highest = -1;
    for (auto& line : input)
        highest = max(highest, calc_seatid(line));
    return highest;
}

int day5_2(const stringlist& input)
{
    vector<uint16_t> ids;
    ids.reserve(input.size());
    transform(begin(input), end(input), back_inserter(ids), calc_seatid);
    sort(begin(ids), end(ids));

    auto id = ids.front();
    for (auto testId : ids)
    {
        if (testId != id)
            return id;
        ++id;
    }
    return -1;
}


void run_day5()
{
    test(357, calc_seatid("FBFBBFFRLR"s));
    test(567u, calc_seatid("BFFFBBFRRR"s));
    test(119u, calc_seatid("FFFBBBFRRR"s));
    test(820u, calc_seatid("BBFFBBFRLL"s));
    gogogo(day5(LOAD(5)));

    gogogo(day5_2(LOAD(5)));
}