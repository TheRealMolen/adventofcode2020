#include "pch.h"
#include "chargrid.h"
#include "harness.h"
#include "pt2.h"

struct Tile
{
    static constexpr size_t Size = 10;

    int m_id;
    CharGrid m_raw;
    array<uint16_t,4> m_bounds;   // NESW

    explicit Tile(stringlist::const_iterator& itinput);
    Tile(const Tile& other, uint32_t rotations, bool hflip, bool vflip);
    void update_bounds();

    string get_column(size_t col) const { return ::get_column(col, m_raw); }
};

ostream& operator<<(ostream& os, const Tile& t)
{
    os << "Tile " << t.m_id << ":\n";
    os << t.m_raw;
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


Tile::Tile(stringlist::const_iterator& itinput)
{
    sscanf_s(itinput->c_str(), "Tile %d:", &m_id);
    ++itinput;

    copy(itinput, itinput + Size, back_inserter(m_raw));
    ranges::reverse(m_raw); // seems like sample input was vflipped before doing anything else
    itinput += Size;

    update_bounds();
}

Tile::Tile(const Tile& other, uint32_t rotations, bool hflip, bool vflip)
    : m_id(other.m_id)
{
    CharGrid grid(other.m_raw);

    if (vflip)
        ::vflip(grid);
    if (hflip)
        ::hflip(grid);

    m_raw = ::rotate(grid, rotations);

    update_bounds();
}

void Tile::update_bounds()
{
    // assume we're rotating
    m_bounds[0] = parse_boundary(m_raw.front());
    m_bounds[1] = parse_boundary(get_column(Size - 1));
    m_bounds[2] = reverse_boundary(parse_boundary(m_raw.back()));
    m_bounds[3] = reverse_boundary(parse_boundary(get_column(0)));
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

    explicit Image(const stringlist& input);

    [[nodiscard]] map<Pt2i16, PlacedTile> place() const;
};

Image::Image(const stringlist& input)
    : m_tiles(read_tiles(input))
{
    m_size = (uint16_t)sqrt(m_tiles.size());
}


map<Pt2i16, PlacedTile> Image::place() const
{
    deque<const Tile*> open;
    ranges::transform(m_tiles, back_inserter(open), [](const Tile& t) { return &t; });

    map<Pt2i16, PlacedTile> placed;

    for (auto i = m_tiles.begin(); i+1 != m_tiles.end(); ++i)
    {
        for (int rot = 0; rot < 4; ++rot)
        {
            auto edge = i->m_bounds[rot];
            [[maybe_unused]] auto matching = count_if(i + 1, m_tiles.end(), [edge](const auto& tile)
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
    placed.try_emplace(Pt2i16{ 0, 0 }, make_unique<Tile>(*open.front(), 0, false, false), Pt2i16{ 0, 0 });
    open.pop_front();

    while (!open.empty())
    {
        // look for one of our boundaries that matches an open tile
        for (auto itplaced_tile = placed.begin(); itplaced_tile != placed.end(); ++itplaced_tile)
        {
            const auto& placed_tile = itplaced_tile->second;

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
                            Pt2i16 pos = placed_tile.pos + boundary_dir(ourrots);
                            uint16_t newrots = add_boundaries(ourrots, theirrots);

                            [[maybe_unused]] auto itplaced = placed.try_emplace(pos, make_unique<Tile>(*ptile, newrots, false, false), pos);
                            //cout << "placed " << itplaced.first->second.tile->m_id << " at " << pos << "\n";
                            //cout << itplaced.first->second.tile->m_raw << "\n\n";
                            open.erase(ittile);

                            goto found_tile;
                        }
                        else if (ptile->m_bounds[theirrots] == ourbound)
                        {
                            Pt2i16 pos = placed_tile.pos + boundary_dir(ourrots);
                            uint16_t newrots = add_boundaries(ourrots, theirrots);

                            bool vflip = (theirrots & 1) != 0;
                            bool hflip = !vflip;

                            [[maybe_unused]] auto itplaced = placed.try_emplace(pos, make_unique<Tile>(*ptile, newrots, hflip, vflip), pos);
                            //cout << "placed " << itplaced.first->second.tile->m_id << " at " << pos << "\n";
                            //cout << itplaced.first->second.tile->m_raw << "\n\n";
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

    return placed;
}


CharGrid render(const map<Pt2i16, PlacedTile>& placed)
{
    auto minx = placed.begin()->second.pos.x;
    auto miny = placed.begin()->second.pos.y;
    auto maxx = placed.rbegin()->second.pos.x;
    auto maxy = placed.rbegin()->second.pos.y;

    const uint32_t size_in_tiles = maxx - minx + 1;
    _ASSERT(uint32_t(maxy - miny + 1) == size_in_tiles);

    const size_t tilesize = placed.begin()->second.tile->m_raw.size() - 2;
    _ASSERT(placed.begin()->second.tile->m_raw.front().size() - 2 == tilesize);

    const size_t size = tilesize * size_in_tiles;
    CharGrid full(size, string(size, '!'));

    for (auto& [pos, tile] : placed)
    {
        auto itrow = full.begin() + (tilesize * (pos.y - miny));
        size_t offset = tilesize * (pos.x - minx);
        for (auto it = tile.tile->m_raw.begin() + 1; it != tile.tile->m_raw.end() - 1; ++it)
        {
            copy(it->begin()+1, it->end()-1, itrow->begin() + offset);
            ++itrow;
        }
    }

    return full;
}


int64_t day20(const stringlist& input)
{
    Image img(input);
    auto placed = img.place();

    auto minx = placed.begin()->second.pos.x;
    auto miny = placed.begin()->second.pos.y;
    auto maxx = placed.rbegin()->second.pos.x;
    auto maxy = placed.rbegin()->second.pos.y;

    int64_t bl = placed.at(Pt2i16{ minx,miny }).tile->m_id;
    int64_t br = placed.at(Pt2i16{ maxx,miny }).tile->m_id;
    int64_t tl = placed.at(Pt2i16{ minx,maxy }).tile->m_id;
    int64_t tr = placed.at(Pt2i16{ maxx,maxy }).tile->m_id;

    return bl * br * tl * tr;
}

int day20_2(const stringlist& input)
{
    Image img(input);
    auto placed = img.place();

    auto map = render(placed);
    cout << map << endl;

    return -1;
}


void run_day20()
{
    test(20899048083289, day20(LOAD(20t)));
    nononoD(day20(LOAD(20)));

    test(-100, day20_2(LOAD(20t)));
    //gogogo(day20_2(LOAD(20)));
}