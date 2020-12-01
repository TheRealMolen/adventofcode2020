#include "pch.h"

#include "harness.h"


int day1(const stringlist& input)
{
    vector<int> expenses;
    expenses.reserve(input.size());
    for (const string& line : input)
        expenses.push_back(atoi(line.c_str()));

    sort(begin(expenses), end(expenses));

    auto lo = begin(expenses);
    auto hi = rbegin(expenses);

    while (*lo + *hi != 2020)
    {
        if (*lo + *hi < 2020)
            ++lo;
        else
            ++hi;
    }

    return *lo * *hi;
}


int64_t day1_2(const stringlist& input)
{
    vector<int> expenses;
    expenses.reserve(input.size());
    for (const string& line : input)
        expenses.push_back(atoi(line.c_str()));

    sort(begin(expenses), end(expenses));

    for (auto lo = begin(expenses); lo != end(expenses); ++lo)
    {
        for (auto med = lo + 1; med != end(expenses) && *lo + *med < 2020; ++med)
        {
            for (auto hi = med + 1; *lo + *med + *hi <= 2020; ++hi)
            {
                if (*lo + *med + *hi == 2020)
                    return int64_t(*lo) * *med * *hi;
            }
        }
    }

    return -1;
}
