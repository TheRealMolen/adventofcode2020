#include "pch.h"
#include "harness.h"

class Universe
{
    vector<uint8_t> m_cells;
    uint16_t m_size;
    uint16_t m_max_influence;

    friend ostream& operator<<(ostream& os, const Universe& u);

    size_t count_active_neighbours(vector<uint8_t>::const_iterator itold) const;

public:
    explicit Universe(const stringlist& input, uint16_t max_cycles = 6)
    {
        _ASSERT(input.size() == input.front().size());

        m_max_influence = max_cycles + 1;

        size_t size = input.size() + 2 * m_max_influence;
        m_size = uint16_t(size);
        m_cells.resize(size * size * size, 0);

        // copy input into cells starting at (max_cycles, max_cycles, max_cycles)
        uint16_t y = m_max_influence;
        uint16_t z = m_max_influence;
        auto itline = input.begin();
        for (; y < m_max_influence + input.size(); ++y, ++itline)
        {
            uint16_t x = m_max_influence;
            auto it_cell = m_cells.begin() + (size * ((size * z) + y)) + x;
            transform(itline->begin(), itline->end(), it_cell, [](char c) -> uint8_t { return c == '#' ? 1 : 0; });
        }
    }

    void tick();

    int count_active() const
    {
        return accumulate(begin(m_cells), end(m_cells), 0);
    }
};

ostream& operator<<(ostream& os, const Universe& u)
{
    // establish extents
    size_t size = u.m_size;
    uint16_t minx = 0xffff, miny = 0xffff, minz = 0xffff;
    uint16_t maxx = 0, maxy = 0, maxz = 0;
    auto itcell = u.m_cells.begin();
    for (size_t z = 0; z < size; ++z)
    {
        bool foundz = false;
        for (size_t y = 0; y < size; ++y)
        {
            bool foundy = false;
            for (size_t x = 0; x < size; ++x, ++itcell)
            {
                if (!*itcell)
                    continue;
                minx = min((uint16_t)x, minx);
                maxx = max((uint16_t)x, maxx);
                foundy = true;
                foundz = true;
            }
            if (foundy)
            {
                miny = min((uint16_t)y, miny);
                maxy = max((uint16_t)y, maxy);
            }
        }
        if (foundz)
        {
            minz = min((uint16_t)z, minz);
            maxz = max((uint16_t)z, maxz);
        }
    }

    for (size_t z = minz; z <= maxz; ++z)
    {
        cout << "z=" << (int(z) - int(u.m_max_influence)) << '\n';
        for (size_t y = miny; y <= maxy; ++y)
        {
            itcell = u.m_cells.begin() + (size * ((size * z) + y)) + minx;
            for (size_t x = minx; x <= maxx; ++x, ++itcell)
            {
                os << (*itcell ? '#' : '.');
            }
            os << '\n';
        }
        os << '\n';
    }
    return os;
}

size_t Universe::count_active_neighbours(vector<uint8_t>::const_iterator itcell) const
{
    size_t neighbours = 0;
    ptrdiff_t upleft = -(m_size + 1);
    ptrdiff_t dnleft = (m_size - 1);

    auto it = itcell - m_size * m_size;
    for (int z = -1; z <= 1; ++z)
    {
        neighbours +=
            accumulate(it + upleft, it + upleft + 3, 0) +
            accumulate(it - 1, it + 2, 0) +
            accumulate(it + dnleft, it + dnleft + 3, 0);

        if (z < 1)
            it += m_size * m_size;
    }

    neighbours -= *itcell;
    return neighbours;
}

void Universe::tick()
{
    vector<uint8_t> newcells(m_cells.size(), 0);

    size_t size = m_size;
    for (size_t z = 1; z < size - 1; ++z)
    {
        for (size_t y = 1; y < size - 1; ++y)
        {
            auto itold = m_cells.cbegin() + (size * ((size * z) + y)) + 1;
            auto itnew = newcells.begin() + (size * ((size * z) + y)) + 1;
            for (size_t x = 1; x < size - 1; ++x, ++itold, ++itnew)
            {
                size_t active_neighbours = count_active_neighbours(itold);

                if (*itold && (active_neighbours < 2 || active_neighbours > 3))
                {
                    //cout << "(" << int64_t(x - m_max_influence) << ", " << int64_t(y - m_max_influence) << ", " << int64_t(z - m_max_influence) << ") died - " << active_neighbours << " neighbours\n";
                    *itnew = 0;
                }
                else if (!*itold && active_neighbours == 3)
                {
                    //cout << "(" << int64_t(x - m_max_influence) << ", " << int64_t(y - m_max_influence) << ", " << int64_t(z - m_max_influence) << ") was born - " << active_neighbours << " neighbours\n";
                    *itnew = 1;
                }
                else
                    *itnew = *itold;
            }
        }
    }

    m_cells.swap(newcells);
}



class Universe4
{
    vector<uint8_t> m_cells;
    uint16_t m_size;
    uint16_t m_max_influence;

    friend ostream& operator<<(ostream& os, const Universe4& u);

    size_t count_active_neighbours(vector<uint8_t>::const_iterator itold) const;

public:
    explicit Universe4(const stringlist& input, uint16_t max_cycles = 6)
    {
        _ASSERT(input.size() == input.front().size());

        m_max_influence = max_cycles + 1;

        size_t size = input.size() + 2 * m_max_influence;
        m_size = uint16_t(size);
        m_cells.resize(size * size * size * size, 0);

        // copy input into cells starting at (max_cycles, max_cycles, max_cycles, max_cycles)
        uint16_t y = m_max_influence;
        uint16_t z = m_max_influence;
        uint16_t w = m_max_influence;
        auto itline = input.begin();
        for (; y < m_max_influence + input.size(); ++y, ++itline)
        {
            uint16_t x = m_max_influence;
            auto it_cell = m_cells.begin() + (size * (size * ((size * w) + z) + y)) + x;
            transform(itline->begin(), itline->end(), it_cell, [](char c) -> uint8_t { return c == '#' ? 1 : 0; });
        }
    }

    void tick();

    int count_active() const
    {
        return accumulate(begin(m_cells), end(m_cells), 0);
    }
};

ostream& operator<<(ostream& os, const Universe4& u)
{
    // establish extents
    size_t size = u.m_size;
    uint16_t minx = 0xffff, miny = 0xffff, minz = 0xffff, minw = 0xffff;
    uint16_t maxx = 0, maxy = 0, maxz = 0, maxw = 0;
    auto itcell = u.m_cells.begin();
    for (size_t w = 0; w < size; ++w)
    {
        bool foundw = false;
        for (size_t z = 0; z < size; ++z)
        {
            bool foundz = false;
            for (size_t y = 0; y < size; ++y)
            {
                bool foundy = false;
                for (size_t x = 0; x < size; ++x, ++itcell)
                {
                    if (!*itcell)
                        continue;
                    minx = min((uint16_t)x, minx);
                    maxx = max((uint16_t)x, maxx);
                    foundy = true;
                    foundz = true;
                    foundw = true;
                }
                if (foundy)
                {
                    miny = min((uint16_t)y, miny);
                    maxy = max((uint16_t)y, maxy);
                }
            }
            if (foundz)
            {
                minz = min((uint16_t)z, minz);
                maxz = max((uint16_t)z, maxz);
            }
        }
        if (foundw)
        {
            minw = min((uint16_t)w, minw);
            maxw = max((uint16_t)w, maxw);
        }
    }

    for (size_t w = minw; w <= maxw; ++w)
    {
        for (size_t z = minz; z <= maxz; ++z)
        {
            cout << "z=" << (int(z) - int(u.m_max_influence)) << ", w=" << (int(w) - int(u.m_max_influence)) << '\n';
            for (size_t y = miny; y <= maxy; ++y)
            {
                itcell = u.m_cells.begin() + (size * (size * ((size * w) + z) + y)) + minx;
                for (size_t x = minx; x <= maxx; ++x, ++itcell)
                {
                    os << (*itcell ? '#' : '.');
                }
                os << '\n';
            }
            os << '\n';
        }
    }
    return os;
}

size_t Universe4::count_active_neighbours(vector<uint8_t>::const_iterator itcell) const
{
    size_t neighbours = 0;
    ptrdiff_t upleft = -(m_size + 1);
    ptrdiff_t dnleft = (m_size - 1);

    auto itw = itcell - m_size * m_size * m_size; // w-1
    for (int w = -1; w <= 1; ++w)
    {
        auto it = itw - m_size * m_size; // z-1
        for (int z = -1; z <= 1; ++z)
        {
            neighbours +=
                accumulate(it + upleft, it + upleft + 3, 0) +
                accumulate(it - 1, it + 2, 0) +
                accumulate(it + dnleft, it + dnleft + 3, 0);

            if (z < 1)
                it += m_size * m_size;
        }
        if (w < 1)
            itw += m_size * m_size * m_size;
    }

    neighbours -= *itcell;
    return neighbours;
}

void Universe4::tick()
{
    vector<uint8_t> newcells(m_cells.size(), 0);

    size_t size = m_size;
    for (size_t w = 1; w < size - 1; ++w)
    {
        for (size_t z = 1; z < size - 1; ++z)
        {
            for (size_t y = 1; y < size - 1; ++y)
            {
                auto itold = m_cells.cbegin() + (size * (size * ((size * w) + z) + y)) + 1;
                auto itnew = newcells.begin() + (size * (size * ((size * w) + z) + y)) + 1;
                for (size_t x = 1; x < size - 1; ++x, ++itold, ++itnew)
                {
                    size_t active_neighbours = count_active_neighbours(itold);

                    if (*itold && (active_neighbours < 2 || active_neighbours > 3))
                    {
                        //cout << "(" << int64_t(x - m_max_influence) << ", " << int64_t(y - m_max_influence) << ", " << int64_t(z - m_max_influence) << ") died - " << active_neighbours << " neighbours\n";
                        *itnew = 0;
                    }
                    else if (!*itold && active_neighbours == 3)
                    {
                        //cout << "(" << int64_t(x - m_max_influence) << ", " << int64_t(y - m_max_influence) << ", " << int64_t(z - m_max_influence) << ") was born - " << active_neighbours << " neighbours\n";
                        *itnew = 1;
                    }
                    else
                        *itnew = *itold;
                }
            }
        }
    }

    m_cells.swap(newcells);
}


int day17(const stringlist& input)
{
    Universe u(input);

    for (int i = 0; i < 6; ++i)
    {
       // cout << "\n\n------- after " << i << " ticks\n" << u << endl;
        u.tick();
    }

    return u.count_active();
}

int day17_2(const stringlist& input)
{
    Universe4 u(input);

    for (int i = 0; i < 6; ++i)
    {
        //cout << "\n\n------- after " << i << " ticks\n" << u << endl;
        u.tick();
    }

    return u.count_active();
}


void run_day17()
{
    string sample =
R"(.#.
..#
###)";

    nestD(112, day17(READ(sample)));
    nononoD(day17(LOAD(17)), 276);

    nestD(848, day17_2(READ(sample)));
    nononoD(day17_2(LOAD(17)));
}