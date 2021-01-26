#include "pch.h"
#include "harness.h"


string day23(const string& input, int nmoves = 10)
{
    const size_t npickedup = 3;
    const size_t ncups = input.length();
    
    vector<uint8_t> state;
    ranges::transform(input, back_inserter(state), [](const char c) -> uint8_t { return c - '0'; });

    const int lo = *ranges::min_element(state);
    const int hi = *ranges::max_element(state);

    string pickup;

    for (int move = 1; move <= nmoves; ++move)
    {    
        auto itpickup = begin(state) + 1;
        pickup.assign(itpickup, itpickup + npickedup);
        state.erase(itpickup, itpickup + npickedup);

        int curr = state.front();
        int dest = curr - 1;
        for (;;)
        {
            if (dest < lo) dest = hi;
            auto itfound = ranges::find(state, uint8_t(dest));
            if (itfound == end(state))
            {
                --dest;
                continue;
            }

            state.insert(itfound + 1, begin(pickup), end(pickup));
            break;
        }

        ranges::rotate(state, begin(state) + 1);
    }

    auto itone = ranges::find(state, 1);
    ranges::rotate(state, itone);

    string result;
    transform(begin(state) + 1, end(state), back_inserter(result), [](uint8_t i) -> char { return '0' + i; });

    return result;
}

int64_t day23_2(const string& input, int nmoves = 10'000'000)
{
    const size_t npickedup = 3;
    const size_t ncups = 1'000'000;

    vector<int> state;
    ranges::transform(input, back_inserter(state), [](const char c) -> int { return c - '0'; });
    const int lo = *ranges::min_element(state);
    int hi = *ranges::max_element(state);
    state.resize(ncups, -1);
    iota(begin(state) + input.size(), end(state), hi+1);
    hi = state.back();

    size_t curr = 0;
    int pickup[3] = { -1, -1, -1 };

    for (int move = 1; move <= nmoves; ++move)
    {
        if (move % 10000 == 0)
            cout << "move " << move << "\t/ " << nmoves << endl;

        pickup[0] = (curr + 1 < state.size()) ? state[curr + 1] : state[curr + 1 - state.size()];
        state.erase(begin(state) + curr + 1);
        pickup[1] = (curr + 1 < state.size()) ? state[curr + 1] : state[curr + 1 - state.size()];
        state.erase(begin(state) + curr + 1);
        pickup[2] = (curr + 1 < state.size()) ? state[curr + 1] : state[curr + 1 - state.size()];
        state.erase(begin(state) + curr + 1);

        int cup = state[curr];
        int dest = cup - 1;
        for (;;)
        {
            if (dest < lo) dest = hi;
            auto itfound = ranges::find(state, uint8_t(dest));
            if (itfound == end(state))
            {
                --dest;
                continue;
            }

            state.insert(itfound + 1, begin(pickup), end(pickup));
            break;
        }

        ++curr;
        if (curr == ncups) curr = 0;
    }

    auto itone = ranges::find(state, 1);
    int64_t result = *(++itone);
    result *= *(++itone);
    return result;
}


void run_day23()
{
    string sample = "389125467";
    string input = "326519478";

    test("92658374", day23(sample));
    test("67384529", day23(sample, 100));
    gogogo(day23(input, 100));

    test(149245887792ll, day23_2(sample));
    //gogogo(day23_2(LOAD(23)));
}