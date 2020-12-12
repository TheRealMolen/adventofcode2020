#include "pch.h"
#include "harness.h"
#include "pt2.h"

vector<char> make_space(const stringlist& input, int& width, int& height)
{
    stringlist building(input);

    // pad with extra floor
    for (auto& row : building)
    {
        row.insert(begin(row), '.');
        row.push_back('.');
    }
    building.insert(begin(building), string(building[0].size(), '.'));
    building.push_back(building.front());

    width = (int)building.front().size();
    height = (int)building.size();

    // copy to contiguous
    vector<char> space;
    space.reserve(width * height);
    for (const auto& row : building)
        ranges::copy(row, back_inserter(space));

    return space;
}

void dump_space(const vector<char> space, int width, int height)
{
    for (int y = 0; y < height; ++y)
    {
        cout.write(&*space.begin() + (y * width), width);
        cout << '\n';
    }
    cout << endl;
}

int day11(const stringlist& input)
{
    int width, height;
    vector<char> space = make_space(input, width, height);
    vector<char> next(space);

    const auto upleft = -(width + 1);
    const auto dnleft = (width - 1);
    const auto dnright = width + 1;
    bool changed;
    do
    {
        changed = false;

        auto itnext = begin(next) + dnright;
        auto itlast = begin(space) + dnright;

        auto itnextend = end(next) + upleft;

        for (; itnext != itnextend; ++itnext, ++itlast)
        {
            if (*itlast == '.')
            {
                _ASSERT(*itnext == '.');
                continue;
            }

            auto neighbours =
                count(itlast + upleft, itlast + upleft + 3, '#') +
                count(itlast - 1, itlast, '#') + count(itlast + 1, itlast + 2, '#') +
                count(itlast + dnleft, itlast + dnleft + 3, '#');

            if (*itlast == 'L' && neighbours == 0)
            {
                *itnext = '#';
                changed = true;
            }
            else if (*itlast == '#' && neighbours >= 4)
            {
                *itnext = 'L';
                changed = true;
            }
            else
            {
                *itnext = *itlast;
            }
        }

        next.swap(space);
    } while (changed);

    return (int)ranges::count(space, '#');
}


Pt2i directions[] = { {-1,-1}, {0, -1}, {1,-1}, {-1,0}, {1,0}, {-1,1}, {0,1}, {1,1} };
int ndirections = sizeof(directions) / sizeof(directions[0]);


char nextvis(const Pt2i& p, const Pt2i& d, const vector<char>& space, int width, int height)
{
    if (p.x < 0 || p.y < 0 || p.x >= width || p.y >= height)
        return '.';
    auto cell = space[p.x + p.y * width];
    if (cell == 'L' || cell == '#')
        return cell;
    return nextvis(p + d, d, space, width, height);
};


int day11_2(const stringlist& input)
{
    int width = (int)input.front().size();
    int height = (int)input.size();
    vector<char> space;
    space.reserve(width * height);
    for (const auto& row : input)
        ranges::copy(row, back_inserter(space));
    vector<char> next(space);

    bool changed;
    do
    {
        changed = false;

        auto itnext = begin(next);
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x, ++itnext)
            {
                auto last = space[x + y*width];
                if (last == '.')
                {
                    _ASSERT(*itnext == '.');
                    continue;
                }

                int neighbours = 0;
                for (const auto& dir : directions)
                {
                    if (nextvis(Pt2i{ x,y } + dir, dir, space, width, height) == '#')
                        ++neighbours;
                }

                if (last == 'L' && neighbours == 0)
                {
                    *itnext = '#';
                    changed = true;
                }
                else if (last == '#' && neighbours >= 5)
                {
                    *itnext = 'L';
                    changed = true;
                }
                else
                {
                    *itnext = last;
                }
            }
        }

        next.swap(space);
    } while (changed);

    return (int)ranges::count(space, '#');
}


void run_day11()
{
    string sample =
R"(L.LL.LL.LL
LLLLLLL.LL
L.L.L..L..
LLLL.LL.LL
L.LL.LL.LL
L.LLLLL.LL
..L.L.....
LLLLLLLLLL
L.LLLLLL.L
L.LLLLL.LL)";

    test(37, day11(READ(sample)));
    nononoD(day11(LOAD(11)));

    test(26, day11_2(READ(sample)));
    nononoD(day11_2(LOAD(11)));
}