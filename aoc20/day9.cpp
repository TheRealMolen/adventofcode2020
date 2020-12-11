#include "pch.h"
#include "harness.h"


int64_t day9(const stringlist& input, int windowsize)
{
    deque<int64_t> window;
    for (auto& line : input)
    {
        int64_t num = strtoll(line.c_str(), nullptr, 10);
        if (window.size() < windowsize)
        {
            window.push_back(num);
            continue;
        }

        // find the pair that sum to this
        for (auto ita = window.begin(); ita != window.end() - 1; ++ita)
        {
            for (auto itb = ita + 1; itb != window.end(); ++itb)
            {
                if (*ita + *itb == num)
                {
                    goto found;
                }
            }
        }

        return num;

    found:
        window.pop_front();
        window.push_back(num);
    }

    return -1;
}

int64_t day9_2(const stringlist& input, int64_t target)
{
    vector<int64_t> ints;
    ranges::transform(input, back_inserter(ints), [](const string& line) { return strtoll(line.c_str(), nullptr, 10); });

    for (auto itstart = ints.begin(); *itstart < target; ++itstart)
    {
        int64_t sum = *itstart;
        auto itend = itstart;
        while (sum < target)
        {
            ++itend;
            sum += *itend;
        }
        if (sum == target)
        {
            return *min_element(itstart, itend + 1) + *max_element(itstart, itend + 1);
        }
    }

    return -1;
}


void run_day9()
{
    string sample =
R"(35
20
15
25
47
40
62
55
65
95
102
117
150
182
127
219
299
277
309
576)";

    test(127, day9(READ(sample), 5));
    auto p1 = day9(LOAD(9), 25);
    gogogo(p1);

    test(62, day9_2(READ(sample), day9(READ(sample), 5)));
    gogogo(day9_2(LOAD(9), p1));
}