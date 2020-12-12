#include "pch.h"
#include "harness.h"
#include "pt2.h"


constexpr int get_dir_ix(char dir)
{
    switch (dir)
    {
    case 'E': return 0;
    case 'S': return 1;
    case 'W': return 2;
    case 'N': return 3;
    default: return -1;
    }
}
Pt2i moves[] = { {1, 0}, {0, -1}, {-1, 0}, {0, 1} };

int day12(const stringlist& input)
{
    Pt2i pos{ 0, 0 };
    int dir = get_dir_ix('E');
    for (auto& line : input)
    {
        char action = line[0];
        int dist = atoi(line.c_str() + 1);

        switch (action)
        {
        case 'E': case 'S': case 'W': case 'N':
            pos += moves[get_dir_ix(action)] * dist;
            break;

        case 'F':
            pos += moves[dir] * dist;
            break;

        case 'R':
            dir = (dir + (dist / 90)) % 4;
            break;
        case 'L':
            dir = (dir + ((3 * dist) / 90)) % 4;
            break;
        }
    }

    return abs(pos.x) + abs(pos.y);
}

int day12_2(const stringlist& input)
{
    Pt2i pos{ 0, 0 };
    Pt2i wpos{ 10, 1 };
    for (auto& line : input)
    {
        char action = line[0];
        int dist = atoi(line.c_str() + 1);

        switch (action)
        {
        case 'E': case 'S': case 'W': case 'N':
            wpos += moves[get_dir_ix(action)] * dist;
            break;

        case 'F':
            pos += wpos * dist;
            break;

        case 'R':
            wpos.rotRight(dist / 90);
            break;
        case 'L':
            wpos.rotRight(-dist / 90);
            break;
        }
    }

    return abs(pos.x) + abs(pos.y);
}


void run_day12()
{
    string sample =
R"(F10
N3
F7
R90
F11)";

    test(25, day12(READ(sample)));
    gogogo(day12(LOAD(12)));

    test(286, day12_2(READ(sample)));
    gogogo(day12_2(LOAD(12)));
}