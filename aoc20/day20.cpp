#include "pch.h"
#include "harness.h"
#include "pt2.h"

#pragma warning(disable : 26812) // unscoped enum

struct Tile
{
    static constexpr size_t Size = 10;

    enum Boundary : uint8_t
    {
        N, E, S, W, Nf, Ef, Sf, Wf,
        NumBoundaries,
    };

    int m_id;
    vector<string> m_raw;
    array<uint16_t,NumBoundaries> m_bounds;   // NESW then flipped NESW

    explicit Tile(stringlist::const_iterator& itinput);
    Tile(const Tile& other, Boundary top);
    void update_bounds();

    static uint16_t parse_boundary(const string& str, bool flipped);
    string get_column(size_t col) const;
};

ostream& operator<<(ostream& os, const Tile& t)
{
    os << "Tile " << t.m_id << ":\n";
    for (const auto& row : t.m_raw)
        os << row << '\n';
    return os;
}


static constexpr Pt2i16 kBoundaryDirections[] =
{
    { 0, -1 },
    { 1, 0 },
    { 0, 1 },
    { -1, 0 },
    { 0, -1 },
    { 1, 0 },
    { 0, 1 },
    { -1, 0 },
};
inline constexpr Pt2i16 boundary_dir(int bound)
{
    return kBoundaryDirections[(int)bound];
}

inline constexpr Tile::Boundary neg_boundary(int a)
{
    return Tile::Boundary((a & 4) | (-a & 3));
}
inline constexpr Tile::Boundary add_boundaries(int a, int b)
{
    int flipped = ((a & 4) ^ (b & 4));
    return Tile::Boundary(flipped | ((a + b) & 0x3));
}
inline constexpr Tile::Boundary add_boundaries(int a, int b, int c)
{
    return add_boundaries(add_boundaries(a, b), c);
}
inline constexpr Tile::Boundary add_boundaries(int a, int b, int c, int d)
{
    return add_boundaries(add_boundaries(a, b, c), d);
}


string Tile::get_column(size_t ix) const
{
    _ASSERT(ix < Size);

    string column(Size, '.');
    ranges::transform(m_raw, column.begin(), [ix](const string& row) { return row[ix]; });
    return column;
}

uint16_t Tile::parse_boundary(const string& str, bool flipped)
{
    uint16_t boundary = 0;

    if (!flipped)
    {
        uint16_t bit = (1 << (Size - 1));
        for (uint32_t i = 0; i < Size; ++i, bit >>= 1)
        {
            if (str[i] == '#')
                boundary |= bit;
        }
    }
    else
    {
        uint16_t bit = 1;
        for (uint32_t i = 0; i < Size; ++i, bit <<= 1)
        {
            if (str[i] == '#')
                boundary |= bit;
        }
    }

    return boundary;
}


Tile::Tile(stringlist::const_iterator& itinput)
{
    sscanf_s(itinput->c_str(), "Tile %d:", &m_id);
    ++itinput;

    copy(itinput, itinput + Size, back_inserter(m_raw));
    ranges::reverse(m_raw);
    itinput += Size;

    update_bounds();
}

Tile::Tile(const Tile& other, Boundary top)
    : m_id(other.m_id)
{
    switch(top)
    {
    case N:
        copy(other.m_raw.begin(), other.m_raw.end(), back_inserter(m_raw));
        break;

    case S:
        for (auto itrow = other.m_raw.rbegin(); itrow != other.m_raw.rend(); ++itrow)
            m_raw.emplace_back(itrow->rbegin(), itrow->rend());
        break;

    case E:
        for (size_t col = Size - 1; col < Size; --col)
            m_raw.emplace_back(other.get_column(col));
        break;

    case W:
        for (size_t col = 0; col < Size; ++col)
        {
            string c = other.get_column(col);
            m_raw.emplace_back(c.rbegin(), c.rend());
        }
        break;

    case Nf:
        for (const string& row : other.m_raw)
            m_raw.emplace_back(row.rbegin(), row.rend());
        break;

    case Sf:
        copy(other.m_raw.rbegin(), other.m_raw.rend(), back_inserter(m_raw));
        break;

    case Ef:
        for (size_t col = Size - 1; col < Size; --col)
        {
            string c = other.get_column(col);
            m_raw.emplace_back(c.rbegin(), c.rend());
        }
        break;

    case Wf:
        for (size_t col = 0; col < Size; ++col)
            m_raw.emplace_back(other.get_column(col));
        break;

    default:
        throw "wtaf";
    }


    update_bounds();
}

void Tile::update_bounds()
{
    m_bounds[N] = parse_boundary(m_raw.front(), false);
    m_bounds[S] = parse_boundary(m_raw.back(), false);
    m_bounds[Nf] = parse_boundary(m_raw.front(), true);
    m_bounds[Sf] = parse_boundary(m_raw.back(), true);

    m_bounds[W] = parse_boundary(get_column(0), false);
    m_bounds[E] = parse_boundary(get_column(Size - 1), false);
    m_bounds[Wf] = parse_boundary(get_column(0), true);
    m_bounds[Ef] = parse_boundary(get_column(Size - 1), true);
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

    // place the first tile at 0,0
    placed.emplace_back(make_unique<Tile>(*open.front(), Tile::N), Pt2i16{ 0, 0 });
    place_in_grid(placed.back());
    open.pop_front();

    while (!open.empty())
    {
        // look for one of our boundaries that matches an open tile
        for (auto itplaced_tile = placed.rbegin(); itplaced_tile != placed.rend(); ++itplaced_tile)
        {
            const auto& placed_tile = *itplaced_tile;

            for (uint16_t ourside = 0; ourside < Tile::NumBoundaries; ++ourside)
            {
                uint16_t ourbound = placed_tile.tile->m_bounds[ourside];
                for (auto ittile = open.begin(); ittile != open.end(); ++ittile)
                {
                    const Tile* ptile = *ittile;

                    for (uint16_t theirside = 0; theirside < Tile::NumBoundaries; ++theirside)
                    {
                        if (ptile->m_bounds[theirside] == ourbound)
                        {
                            cout << "placing tile " << ptile->m_id << " beside " << placed_tile.tile->m_id << endl;
                            Pt2i16 pos = placed_tile.pos + boundary_dir(ourside);
                            Tile::Boundary newtop = add_boundaries(ourside, 2, neg_boundary(theirside));
                            cout << "    old tile " << placed_tile.tile->m_id << " is at " << placed_tile.pos << endl;
                            cout << "    matched old side " << ourside << " to new side " << theirside << endl;
                            cout << "    new tile placed at " << pos << ", and top is " << (int)newtop << endl;
                            cout << "    old tile: " << *placed_tile.tile << endl;
                            cout << "    raw new tile: " << *ptile << endl;

                            placed.emplace_back(make_unique<Tile>(*ptile, (Tile::Boundary)newtop), pos);
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
    test(Tile::E, add_boundaries(Tile::N, Tile::E));
    test(Tile::S, add_boundaries(Tile::E, Tile::E));
    test(Tile::Sf, add_boundaries(Tile::E, Tile::Ef));
    test(Tile::S, add_boundaries(Tile::W, Tile::W));
    test(20899048083289, day20(LOAD(20t)));
    gogogo(day20(LOAD(20)));

    //test(-100, day20_2(READ(sample)));
    //gogogo(day20_2(LOAD(20)));
}