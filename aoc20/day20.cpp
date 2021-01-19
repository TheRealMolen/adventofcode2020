#include "pch.h"
#include "harness.h"
#include "pt2.h"

#pragma warning(disable : 26812) // unscoped enum

struct Tile
{
    static constexpr size_t Size = 10;

    enum Boundary : uint8_t
    {
        N, E, S, W, 
        NumBoundaries,
    };

    int m_id;
    vector<string> m_raw;
    array<uint16_t,NumBoundaries> m_bounds;   // NESW

    explicit Tile(stringlist::const_iterator& itinput);
    Tile(const Tile& other, size_t rotations, bool hflip, bool vflip);
    void update_bounds();

    string get_column(size_t col) const;
    string get_column(size_t ix, const vector<string>& raw) const;
};

ostream& operator<<(ostream& os, const Tile& t)
{
    os << "Tile " << t.m_id << ":\n";
    for (const auto& row : t.m_raw)
        os << row << '\n';
    return os;
}



uint16_t parse_boundary(const string& str)
{
    uint16_t boundary = 0;
    uint16_t bit = (1 << (Tile::Size - 1));
    for (uint32_t i = 0; i < Tile::Size; ++i, bit >>= 1)
    {
        if (str[i] == '#')
            boundary |= bit;
    }

    return boundary;
}

// reverse the bits in a 10bit int
uint16_t reverse_boundary(uint16_t a)
{
    uint16_t b = ((a & 0x3e0) >> 5) | ((a & 0x1f) << 5);
    uint16_t c = ((b & 0x318) >> 3) | /*(b & 0x84) |*/ ((b & 0x63) << 3);
    uint16_t d = ((c & 0x252) >> 1) | (b & 0x84) | ((c & 0x129) << 1);
    return d;
}

static constexpr Pt2i16 kBoundaryDirections[] =
{
    { 0, -1 },
    { 1, 0 },
    { 0, 1 },
    { -1, 0 },
};
inline constexpr Pt2i16 boundary_dir(int bound)
{
    return kBoundaryDirections[(int)bound];
}

inline constexpr uint16_t add_boundaries(uint16_t a, uint16_t b)
{
    return ((b - a + 2 + 4) & 0x3);
}


string Tile::get_column(size_t ix) const
{
    _ASSERT(ix < Size);

    string column(Size, '.');
    ranges::transform(m_raw, column.begin(), [ix](const string& row) { return row[ix]; });
    return column;
}
string Tile::get_column(size_t ix, const vector<string>& raw) const
{
    _ASSERT(ix < Size);

    string column(Size, '.');
    ranges::transform(raw, column.begin(), [ix](const string& row) { return row[ix]; });
    return column;
}


Tile::Tile(stringlist::const_iterator& itinput)
{
    sscanf_s(itinput->c_str(), "Tile %d:", &m_id);
    ++itinput;

    copy(itinput, itinput + Size, back_inserter(m_raw));
    ranges::reverse(m_raw); // seems like sample input was vflipped before doing anything else
    itinput += Size;

    update_bounds();
}

Tile::Tile(const Tile& other, size_t rotations, bool hflip, bool vflip)
    : m_id(other.m_id)
{
    vector<string> otherraw(other.m_raw);

    if (vflip)
        ranges::reverse(otherraw);

    if (hflip)
    {
        for (string& row : otherraw)
            ranges::reverse(row);
    }

    switch (rotations)
    {
    case 0:
        copy(otherraw.begin(), otherraw.end(), back_inserter(m_raw));
        break;

    case 1:
        for (size_t col = Size - 1; col < Size; --col)
            m_raw.emplace_back(get_column(col, otherraw));
        break;

    case 2:
        for (auto itrow = otherraw.rbegin(); itrow != otherraw.rend(); ++itrow)
            m_raw.emplace_back(itrow->rbegin(), itrow->rend());
        break;

    case 3:
        for (size_t col = 0; col < Size; ++col)
        {
            string c = get_column(col, otherraw);
            m_raw.emplace_back(c.rbegin(), c.rend());
        }
        break;

    default:
        _ASSERT(!"is impossible");
        break;
    }

    update_bounds();
}

void Tile::update_bounds()
{
    // assume we're rotating
    m_bounds[N] = parse_boundary(m_raw.front());
    m_bounds[E] = parse_boundary(get_column(Size - 1));
    m_bounds[S] = reverse_boundary(parse_boundary(m_raw.back()));
    m_bounds[W] = reverse_boundary(parse_boundary(get_column(0)));
}


vector<Tile> read_tiles(const stringlist& input)
{
    vector<Tile> tiles;
    tiles.reserve(100);
    for (auto itinput = input.begin(); itinput != input.end(); /**/)
    {
        tiles.emplace_back(itinput);

        if (itinput != input.end())
        {
            _ASSERT(itinput->empty());
            ++itinput;
        }
    }
    return tiles;
}


struct PlacedTile
{
    unique_ptr<Tile> tile;
    Pt2i16 pos = {-1000, -1000};

    operator bool() const
    {
        return tile != nullptr;
    }
};

struct Image
{
    vector<Tile> m_tiles;
    uint16_t m_size;

    Image(const stringlist& input);

    [[nodiscard]] vector<PlacedTile> place() const;
};

Image::Image(const stringlist& input)
    : m_tiles(read_tiles(input))
{
    m_size = (uint16_t)sqrt(m_tiles.size());
}


vector<PlacedTile> Image::place() const
{
    deque<const Tile*> open;
    ranges::transform(m_tiles, back_inserter(open), [](const Tile& t) { return &t; });

    vector<PlacedTile> placed;
    auto size = m_size;
    vector<int> grid(size * size * 4, 0);
    auto place_in_grid = [&grid, size](const PlacedTile& tile)
    {
        _ASSERT(abs(tile.pos.x) < size);
        _ASSERT(abs(tile.pos.y) < size);
        int x = tile.pos.x + size;
        int y = tile.pos.y + size;
        _ASSERT(grid[x + (2 * size * y)] == 0);
        grid[x + (2 * size * y)] = tile.tile->m_id;

        cout << "--------\n";
        for (y = 0; y < 2 * size; ++y)
        {
            for (x = 0; x < 2 * size; ++x)
            {
                cout << setw(4) << setfill(' ') << grid[x + (2 * size * y)] << ' ';
            }
            cout << '\n';
        }
        cout << endl;
    };

    for (auto i = m_tiles.begin(); i+1 != m_tiles.end(); ++i)
    {
        for (int rot = 0; rot < 4; ++rot)
        {
            auto edge = i->m_bounds[rot];
            auto matching = count_if(i + 1, m_tiles.end(), [edge](const auto& tile)
                {
                    return ranges::find_if(tile.m_bounds, [edge](const auto& bound)
                        {
                            return (edge == bound) || (edge == reverse_boundary(bound));
                        }) != tile.m_bounds.end();
                });
            _ASSERT(matching <= 1);
        }
    }

    // place the first tile at 0,0
    placed.emplace_back(make_unique<Tile>(*open.front(), 0, false, false), Pt2i16{ 0, 0 });
    place_in_grid(placed.back());
    open.pop_front();

    while (!open.empty())
    {
        // look for one of our boundaries that matches an open tile
        for (auto itplaced_tile = placed.rbegin(); itplaced_tile != placed.rend(); ++itplaced_tile)
        {
            const auto& placed_tile = *itplaced_tile;

            for (uint16_t ourrots = 0; ourrots < 4; ++ourrots)
            {
                uint16_t ourbound = placed_tile.tile->m_bounds[ourrots];
                for (auto ittile = open.begin(); ittile != open.end(); ++ittile)
                {
                    const Tile* ptile = *ittile;

                    for (uint16_t theirrots = 0; theirrots < 4; ++theirrots)
                    {
                        if (ptile->m_bounds[theirrots] == reverse_boundary(ourbound))
                        {
                            cout << "placing tile " << ptile->m_id << " beside " << placed_tile.tile->m_id << endl;
                            Pt2i16 pos = placed_tile.pos + boundary_dir(ourrots);
                            uint16_t newrots = add_boundaries(ourrots, theirrots);
                            cout << "    old tile " << placed_tile.tile->m_id << " is at " << placed_tile.pos << endl;
                            cout << "    matched old side " << ourrots << " to new side " << theirrots << endl;
                            cout << "    new tile placed at " << pos << ", and top is " << (int)newrots << endl;
                            cout << "    old tile: " << *placed_tile.tile << endl;
                            cout << "    raw new tile: " << *ptile << endl;

                            placed.emplace_back(make_unique<Tile>(*ptile, newrots, false, false), pos);
                            cout << "    placed tile: " << *placed.back().tile << endl;
                            place_in_grid(placed.back());

                            open.erase(ittile);

                            goto found_tile;
                        }
                        else if (ptile->m_bounds[theirrots] == ourbound)
                        {
                            cout << "placing tile " << ptile->m_id << " FLIPPED beside " << placed_tile.tile->m_id << endl;
                            Pt2i16 pos = placed_tile.pos + boundary_dir(ourrots);
                            uint16_t newrots = add_boundaries(ourrots, theirrots);
                            cout << "    old tile " << placed_tile.tile->m_id << " is at " << placed_tile.pos << endl;
                            cout << "    matched old side " << ourrots << " to FLIPPED new side " << theirrots << endl;
                            cout << "    new tile placed at " << pos << ", and top is " << (int)newrots << endl;
                            cout << "    old tile: " << *placed_tile.tile << endl;
                            cout << "    raw new tile: " << *ptile << endl;

                            bool vflip = (theirrots & 1) != 0;
                            bool hflip = !vflip;

                            placed.emplace_back(make_unique<Tile>(*ptile, newrots, hflip, vflip), pos);
                            cout << "    placed tile: " << *placed.back().tile << endl;
                            place_in_grid(placed.back());

                            open.erase(ittile);

                            goto found_tile;
                        }
                    }
                }
            }
        }

    found_tile:
        continue;
    }

    
    //const auto [itminx, itmaxx] = ranges::minmax_element(placed, {}, [](const auto& tile) { return tile.pos.x; });
    //const auto [itminy, itmaxy] = ranges::minmax_element(placed, {}, [](const auto& tile) { return tile.pos.y; });
    //auto minx = itminx->pos.x;
    //auto maxx = itmaxx->pos.x;
    //auto miny = itminy->pos.y;
    //auto maxy = itmaxy->pos.y;
    //for (int y = miny; y <= maxy; ++y)
    //{
    //    for (int x = minx; x <= maxx; ++x)
    //    {
    //        int64_t bl = ranges::find_if(placed, [=](const auto& tile) { return tile.pos.x == minx && tile.pos.y == miny; });
    //    }
    //}
    cout << "\n\n\n\n\n=======================\n\n";
    for (const auto& ptile : placed)
    {
        cout << "Placed Tile " << ptile.tile->m_id << "  top line: " << ptile.tile->m_raw.front() << "\n";
    }
    cout << "\n\n=======================\n" << endl;


    return placed;
}


int64_t day20(const stringlist& input)
{
#if 0
    stringlist samp{ READ(
 R"(Tile 2473:
#....####.
#..#.##...
#.##..#...
######.#.#
.#...#.#.#
.#########
.###.#..#.
########.#
##...##.#.
..###.#.#.)") };
    auto itsamp = samp.cbegin();
    Tile t(itsamp);
    for (int i = 0; i < 8; ++i)
    {
        Tile t2(t, (Tile::Boundary)i);
        cout << "------ top = " << i << ":\n" << t2 << endl;
    }
    cout << "\n\n\n\n" << endl;
#endif //0

    Image img(input);
    auto placed = img.place();

    const auto [itminx, itmaxx] = ranges::minmax_element(placed, {}, [](const auto& tile) { return tile.pos.x; });
    const auto [itminy, itmaxy] = ranges::minmax_element(placed, {}, [](const auto& tile) { return tile.pos.y; });
    auto minx = itminx->pos.x;
    auto maxx = itmaxx->pos.x;
    auto miny = itminy->pos.y;
    auto maxy = itmaxy->pos.y;

    int64_t bl = ranges::find_if(placed, [=](const auto& tile) { return tile.pos.x == minx && tile.pos.y == miny; })->tile->m_id;
    int64_t br = ranges::find_if(placed, [=](const auto& tile) { return tile.pos.x == maxx && tile.pos.y == miny; })->tile->m_id;
    int64_t tl = ranges::find_if(placed, [=](const auto& tile) { return tile.pos.x == minx && tile.pos.y == maxy; })->tile->m_id;
    int64_t tr = ranges::find_if(placed, [=](const auto& tile) { return tile.pos.x == maxx && tile.pos.y == maxy; })->tile->m_id;

    return bl * br * tl * tr;
}

int day20_2(const stringlist& input)
{
    for (auto& line : input)
    {
        (void)line;
    }

    return -1;
}


void run_day20()
{
    test(20899048083289, day20(LOAD(20t)));
    gogogo(day20(LOAD(20)));

    //test(-100, day20_2(READ(sample)));
    //gogogo(day20_2(LOAD(20)));
}