#include "pch.h"
#include "harness.h"


int day10(const stringlist& input)
{
    vector<int64_t> ints;
    ranges::transform(input, back_inserter(ints), [](const string& line) { return strtoll(line.c_str(), nullptr, 10); });
    ranges::sort(ints);

    int64_t curr = 0;
    int numOnes = 0;
    int numThrees = 1; // for the extra end one
    for (auto next : ints)
    {
        auto diff = next - curr;
        if (diff == 1)
            ++numOnes;
        else if (diff == 3)
            ++numThrees;

        curr = next;
    }

    return numOnes * numThrees;
}


using d10Tree = vector<vector<uint32_t>>;

d10Tree parse_tree(const vector<int64_t>& ints)
{
    d10Tree tree;
    tree.resize(ints.size());
    for (int i = 0; i < ints.size(); ++i)
    {
        vector<uint32_t> possible_next;
        for (int j = i+1; j < ints.size() && ints[j] <= ints[i] + 3; ++j)
        {
            possible_next.push_back(j);
        }
        tree[i] = possible_next;
    }
    return tree;
}


int64_t day10_2(const stringlist& input)
{
    vector<int64_t> ints;
    ranges::transform(input, back_inserter(ints), [](const string& line) { return strtoll(line.c_str(), nullptr, 10); });
    ranges::sort(ints);

    int64_t goal = *max_element(begin(ints), end(ints)) + 3;
    ints.insert(begin(ints), 0);
    ints.push_back(goal);

    auto tree = parse_tree(ints);
    vector<int64_t> routes(ints.size(), 0);
    routes[0] = 1;
    for (size_t i = 0; i < ints.size(); ++i)
    {
        for (auto j : tree[i])
            routes[j] += routes[i];
    }

    return routes.back();
}


void run_day10()
{
    string sample =
R"(16
10
15
5
1
11
7
19
6
12
4)";

    string sample2 =
R"(28
33
18
42
31
14
46
20
48
47
24
23
49
45
19
38
39
11
1
32
25
35
8
17
7
9
4
2
34
10
3)";

    test(35, day10(READ(sample)));
    test(220, day10(READ(sample2)));
    gogogo(day10(LOAD(10)));

    test(8, day10_2(READ(sample)));
    test(19208, day10_2(READ(sample2)));
    gogogo(day10_2(LOAD(10)));
}