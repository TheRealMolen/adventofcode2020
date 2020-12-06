#include "pch.h"
#include "harness.h"


int day6(const stringlist& input)
{
    string group;
    int total = 0;
    auto check_group = [&group]()
    {
        ranges::sort(group);
        auto last_uniq = unique(begin(group), end(group));
        int questions = (int)distance(begin(group), last_uniq);
        group.clear();
        return questions;
    };

    for (auto& line : input)
    {
        if (!line.length())
            total += check_group();
        else
            group += line;
    }
    total += check_group();
    return total;
}

int day6_2(stringlist input)
{
    string group;
    int total = 0;
    bool newgroup = true;
    for (auto& line : input)
    {
        ranges::sort(line);

        if (newgroup)
        {
            group = line;
            newgroup = false;
            continue;
        }
        if (!line.length())
        {
            total += (int)group.length();
            group.clear();
            newgroup = true;
            continue;
        }

        string isxn;
        set_intersection(begin(group), end(group), begin(line), end(line), back_inserter(isxn));
        group = move(isxn);
    }
    total += (int)group.length();
    return total;
}

void run_day6()
{
    auto sample =
R"(abc

a
b
c

ab
ac

a
a
a
a

b)";

    test(11, day6(READ(sample)));
    gogogo(day6(LOAD(6)));

    test(6, day6_2(move(READ(sample))));
    gogogo(day6_2(move(LOAD(6))));
}