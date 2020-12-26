#include "pch.h"
#include "harness.h"


tuple<int64_t, vector<int64_t>> read_input(const stringlist& input)
{
    int64_t earliest = strtoll(input[0].c_str(), nullptr, 10);
    vector<int64_t> available;
    for (const string& busname : split(input[1], ","))
    {
        if (busname == "x")
            continue;
        available.push_back(strtoll(busname.c_str(), nullptr, 10));
    }
    return make_tuple(earliest, available);
}

int64_t day13(const stringlist& input)
{
    int64_t earliest;
    vector<int64_t> available;
    tie(earliest, available) = read_input(input);

    int64_t best = -1;
    int64_t bestwait = 10000000000ll;
    for (int64_t bus : available)
    {
        auto mod = earliest / bus;
        auto firstdeparture = bus * (mod + 1);
        auto wait = firstdeparture - earliest;
        if (wait < bestwait)
        {
            bestwait = wait;
            best = bus;
        }
    }

    return best * bestwait;
}


volatile bool quit = false;
BOOL WINAPI ctrlc_handler(DWORD)
{
    quit = true;
    return TRUE;
}

int64_t day13_2(const string& input, int64_t start = -1)
{
    vector<pair<int64_t, int64_t>> bus_offsets;
    {
        int64_t offset = -1;
        for (const string& busname : split(input, ","))
        {
            ++offset;
            if (busname == "x")
                continue;
            bus_offsets.push_back(make_pair(strtoll(busname.c_str(), nullptr, 10), offset));
        }
    }

    auto biggest_bus = *max_element(begin(bus_offsets), end(bus_offsets), [](const auto& a, const auto& b)
        {
            return a.first < b.first;
        });

    SetConsoleCtrlHandler(ctrlc_handler, TRUE);

    int64_t i = start;
    if (i < 0)
        i = biggest_bus.first - biggest_bus.second;
    for (;;)
    {
        if (quit)
        {
            cout << i << endl;
            return -1;
        }
        //if (i > 1068760ll)
        //    cout << "oop\n";

        for (const auto& bus_offset : bus_offsets)
        {
            if ((i + bus_offset.second) % bus_offset.first != 0)
                goto next;
        }
        return i;

    next: i += biggest_bus.first;
    }

    return -1ll;
}


void run_day13()
{
    string sample =
R"(939
7,13,x,x,59,x,31,19)";

    string input =
R"(1015292
19,x,x,x,x,x,x,x,x,41,x,x,x,x,x,x,x,x,x,743,x,x,x,x,x,x,x,x,x,x,x,x,13,17,x,x,x,x,x,x,x,x,x,x,x,x,x,x,29,x,643,x,x,x,x,x,37,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,23)";

    test(295, day13(READ(sample)));
    gogogo(day13(READ(input)));

    test(1068781, day13_2(READ(sample)[1]));
    test(3417, day13_2("17,x,13,19"));
    test(754018, day13_2("67,7,59,61"));
    test(779210, day13_2("67,x,7,59,61"));
    test(1261476, day13_2("67,7,x,59,61"));
    test(1202161486ll, day13_2("1789,37,47,1889"));
    nonono(day13_2(READ(input)[1]), 1001569619313439ll);
}


