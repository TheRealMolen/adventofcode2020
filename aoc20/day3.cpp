#include "pch.h"
#include "harness.h"


struct TreeMap
{
    int width, height;
    uint32_t firstRow;
    vector<uint32_t> rows;

    TreeMap(const stringlist& input)
    {
        width = int(input.front().length());
        _ASSERT(width <= 32);
        height = int(input.size());

        rows.reserve(height);
        for (const string& line : input)
        {
            uint32_t trees = 0;
            uint32_t mask = 1u << 31;
            for (char c : line)
            {
                if (c == '#')
                    trees |= mask;
                mask >>= 1;
            }
            rows.push_back(trees);
        }
        
        // take the top row off as we use it differently from the rest
        firstRow = rows.front();
        rows.erase(rows.begin());
    }
};
ostream& operator<<(ostream& os, const TreeMap& trees)
{
    for (uint32_t row : trees.rows)
    {
        uint32_t mask = 1u << 31;
        for (int i = 0; i < trees.width; ++i)
        {
            os << ((row & mask) ? '#' : '.');
            mask >>= 1;
        }
        os << '\n';
    }
    return os;
}


int countTrees(const TreeMap& trees, uint32_t gradient, uint32_t skiprows=0)
{
    int numTrees = 0;
    uint64_t mask = 1u << 31;
    uint32_t skip = skiprows;
    for (uint32_t row : trees.rows)
    {
        if (skip)
        {
            --skip;
            continue;
        }

        mask |= (mask << (trees.width));
        mask >>= gradient;

        if (row & mask)
            ++numTrees;

        skip = skiprows;
    }
    return numTrees;
}

int day3(const stringlist& input, uint32_t gradient=3, uint32_t skiprows=0)
{
    TreeMap trees(input);
    return countTrees(trees, gradient, skiprows);
}

int64_t day3_2(const stringlist& input)
{
    TreeMap trees(input);

    int64_t product = 1;
    product *= countTrees(trees, 1);
    product *= countTrees(trees, 3);
    product *= countTrees(trees, 5);
    product *= countTrees(trees, 7);
    product *= countTrees(trees, 1, 1);
    return product;
}


void day3_test()
{
    test(2, day3(LOAD(3t), 1));
    test(7, day3(LOAD(3t), 3));
    test(3, day3(LOAD(3t), 5));
    test(4, day3(LOAD(3t), 7));
    test(2, day3(LOAD(3t), 1, 1));
    gogogo(day3(LOAD(3)));

    test(336ll, day3_2(LOAD(3t)));
    gogogo(day3_2(LOAD(3)));
}
