#include "pch.h"
#include "harness.h"

void kernel(vector<int>& state, const int nmoves)
{
    const size_t npickedup = 3;
    const size_t ncups = state.size();

    const int lo = *ranges::min_element(state);
    const int hi = *ranges::max_element(state);

    size_t curr = 0;
    int pickup[3] = { -1, -1, -1 };

    vector<int> indices(ncups + 1, -1);
    auto reindex_full = [&state, &indices]()
    {
        int ix = 0;
        for (auto cup : state)
        {
            indices[cup] = ix;
            ++ix;
        }
    };
    auto reindex_partial = [&state, &indices](vector<int>::iterator itstart, vector<int>::iterator itend)
    {
        int ix = int(distance(begin(state), itstart));
        while (itstart != itend)
        {
            indices[*itstart] = ix;
            ++ix;
            ++itstart;
        }
    };

    reindex_full();

    /* auto pick_up = [&state, &pickup, curr](size_t offs)
     {
         size_t ix = (curr + 1 + offs);
         if (ix >= ncups)
             ix -= ncups;

         pickup[offs] = state[ix];
         state[ix] = -1;
     };*/

    for (int move = 1; move <= nmoves; ++move)
    {
        if (move % 10000 == 0)
            cout << "move " << move << "\t/ " << nmoves << endl;

        auto itcup = begin(state) + curr;
        auto itpickup = itcup + 1;
        copy(itpickup, itpickup + npickedup, pickup);
        fill(itpickup, itpickup + npickedup, -1);

        int dest = *itcup - 1;
        for (;;)
        {
            if (dest < lo) dest = hi;
            //auto itdest = begin(state) + indices[dest];
            //if (*itdest == -1)
            auto itdest = ranges::find(state, dest);
            if (itdest == end(state))
            {
                --dest;
                continue;
            }

            if (itdest > itcup)
            {
                auto itstart = itpickup + npickedup;
                auto itend = itdest + 1;

                //::move(itstart, itend, itpickup);
                auto itin = itstart;
                auto itout = itpickup;
                int ix = int(distance(begin(state), itout));
                for (; itin != itend; ++itin, ++itout, ++ix)
                {
                    int x = *itin;
                    *itout = x;
                    //indices[x] = ix;
                }

                //ranges::copy(pickup, itend - npickedup);
                auto itin2 = begin(pickup);
                auto itend2 = end(pickup);
                for (; itin2 != itend2; ++itin2, ++itout, ++ix)
                {
                    int x = *itin2;
                    *itout = x;
                   // indices[x] = ix;
                }
                
                //reindex_partial(itpickup, itend);
            }
            else
            {
                auto itstart = itdest + 1;
                auto itend = itpickup;
                ::move(itstart, itend, itstart + npickedup);
                ranges::copy(pickup, itstart);
              // reindex_partial(itstart, itend + npickedup);

                curr += npickedup;
            }
            break;
        }

        ++curr;
        if (curr == ncups) curr = 0;
        else if (curr + npickedup >= ncups)
        {
            ranges::rotate(state, begin(state) + curr);
            curr = 0;
            //reindex_full();
        }
    }
}

string day23(const string& input, int nmoves = 10)
{
    vector<int> state;
    ranges::transform(input, back_inserter(state), [](const char c) -> int { return c - '0'; });

    kernel(state, nmoves);

    auto itone = ranges::find(state, 1);
    ranges::rotate(state, itone);

    string result;
    transform(begin(state) + 1, end(state), back_inserter(result), [](int i) -> char { return '0' + char(i); });

    return result;
}

int64_t day23_2(const string& input, int nmoves = 10'000'000)
{
    const int ncups = 1'000'000;

    vector<int> state;
    ranges::transform(input, back_inserter(state), [](const char c) -> int { return c - '0'; });
    int hi = *ranges::max_element(state);
    state.resize(ncups, -1);
    iota(begin(state) + input.size(), end(state), hi+1);
    hi = state.back();

    kernel(state, nmoves);

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
    gogogo(day23(input, 100), "25368479"s);

    test(149245887792ll, day23_2(sample));
    //gogogo(day23_2(LOAD(23)));
}