#include "pch.h"
#include "harness.h"

enum class HexDir : uint8_t { E, SE, SW, W, NW, NE };
struct HexPt
{
    int col, row;

    bool operator==(const HexPt& o) const
    {
        return row == o.row && col == o.col;
    }
};
bool operator<(const HexPt& a, const HexPt& b)
{
    if (a.row < b.row) return true;
    if (a.row > b.row) return false;
    return (a.col < b.col);
}
ostream& operator<<(ostream& os, const HexPt& pt)
{
    return os << "<" << pt.col << ", " << pt.row << ">";
}
HexDir parsedir(const char*& dir)
{
    switch (*(dir++))
    {
    case 'e': return HexDir::E;
    case 'w': return HexDir::W;
    case 'n':
        switch (*(dir++))
        {
        case 'e': return HexDir::NE;
        case 'w': return HexDir::NW;
        }
        break;
    case 's':
        switch (*(dir++))
        {
        case 'e': return HexDir::SE;
        case 'w': return HexDir::SW;
        }
        break;
    }
    _ASSERT(!"bad hex dir");
    return HexDir::E;
}
HexPt move(const HexPt& pt, const char*& dir)
{
    int adjust = (pt.row & 1) ? 0 : -1;
    HexDir hdir = parsedir(dir);
    using enum HexDir;
    switch (hdir)
    {
    case E: return { pt.col + 1, pt.row };
    case W: return { pt.col - 1, pt.row };
    case SW: return { pt.col + adjust, pt.row + 1 };
    case SE: return { pt.col + 1 + adjust, pt.row + 1 };
    case NW: return { pt.col + adjust, pt.row - 1 };
    case NE: return { pt.col + 1 + adjust, pt.row - 1 };
    }
    return { 10000, 10000 };
}
HexPt follow_path(const char* path)
{
    const char* curr = path;
    HexPt pt{ 0,0 };
    while (*curr)
    {
        pt = move(pt, curr);
    }
    return pt;
}


int day24(const stringlist& input)
{
    map<HexPt, int> numflips;
    for (auto& line : input)
    {
        auto tile = follow_path(line.c_str());
        numflips[tile]++;
    }

    int blacktiles = accumulate(begin(numflips), end(numflips), 0, [](int acc, const auto& ittile)
        {
            return acc + (ittile.second & 1);
        });

    return blacktiles;
}


enum class TileColour : uint8_t { WHITE, BLACK };
struct HexMap
{
    const int boundary = 120;

    vector<TileColour> tiles;
    vector<TileColour> tiles_working;
    int nrows, ncols;
    HexPt minactive;
    HexPt maxactive;

    explicit HexMap(const map<HexPt, int>& flips);
    TileColour& tile(int col, int row)
    {
        return tiles[col + row * ncols];
    }

    void tick();

    int numblack() const
    {
        return int(count(begin(tiles), end(tiles), TileColour::BLACK));
    }

    int count_neighbours(vector<TileColour>::const_iterator it, bool oddrow) const;
};

ostream& operator<<(ostream& os, const HexMap& map)
{
    auto it = begin(map.tiles);
    for (int row = 0; row < map.nrows; ++row)
    {
        os << setw(3) << setfill(' ') << row << ": ";

        if (row & 1)
            os << ' ';

        for (int col = 0; col < map.ncols; ++col, ++it)
        {
            os << (*it == TileColour::WHITE ? ". " : "# ");
        }

        os << "\n";
    }
    return os;
}


HexMap::HexMap(const map<HexPt, int>& flips)
{
    HexPt maxpt{ -1000000,-1000000 };
    HexPt minpt{ 1000000, 1000000 };
    for (const auto& [tile, nflips] : flips)
    {
        maxpt.row = max(tile.row, maxpt.row);
        maxpt.col = max(tile.col, maxpt.col);
        minpt.row = min(tile.row, minpt.row);
        minpt.col = min(tile.col, minpt.col);
    }

    ncols = 2 * boundary;
    nrows = 2 * boundary;

    minactive = { minpt.col + boundary, minpt.row + boundary };
    maxactive = { maxpt.col + boundary, maxpt.row + boundary };

    tiles.resize(ncols * nrows, TileColour::WHITE);
    tiles_working.resize(ncols * nrows, TileColour::WHITE);

    for (const auto& [t, nflips] : flips)
    {
        if (nflips & 1)
            tile(t.col + boundary, t.row + boundary) = TileColour::BLACK;
    }
}

int HexMap::count_neighbours(vector<TileColour>::const_iterator it, bool oddrow) const
{
    int neighbours = 0;
    int adjust = oddrow ? 0 : -1;

    if (*(it - 1) == TileColour::BLACK)
        ++neighbours;
    if (*(it + 1) == TileColour::BLACK)
        ++neighbours;
    if (*(it - ncols + adjust) == TileColour::BLACK)
        ++neighbours;
    if (*(it + ncols + adjust) == TileColour::BLACK)
        ++neighbours;
    if (*(it - ncols + 1 + adjust) == TileColour::BLACK)
        ++neighbours;
    if (*(it + ncols + 1 + adjust) == TileColour::BLACK)
        ++neighbours;

    return neighbours;
}

void HexMap::tick()
{
    auto& otiles = tiles;
    auto& ntiles = tiles_working;

    ranges::fill(ntiles, TileColour::WHITE);    // ugh

    minactive.row -= 1;
    minactive.col -= 1;
    maxactive.row += 1;
    maxactive.col += 1;

    HexPt maxpt{ -1000000,-1000000 };
    HexPt minpt{ 1000000, 1000000 };

    using enum TileColour;

    for (int row = minactive.row; row <= maxactive.row; ++row)
    {
        bool odd = row & 1;
        auto itold = begin(otiles) + ((row * ncols) + minactive.col);
        auto itnew = begin(ntiles) + ((row * ncols) + minactive.col);

        for (int col = minactive.col; col <= maxactive.col; ++col, ++itold, ++itnew)
        {
            int neighbours = count_neighbours(itold, odd);

            if (*itold == BLACK)
            {
                if (neighbours == 0 || neighbours > 2)
                    *itnew = WHITE;
                else
                {
                    *itnew = BLACK;

                    maxpt.col = max(maxpt.col, col);
                    maxpt.row = max(maxpt.row, row);
                    minpt.col = min(minpt.col, col);
                    minpt.row = min(minpt.row, row);
                }
            }
            else
            {
                if (neighbours == 2)
                {
                    *itnew = BLACK;

                    maxpt.col = max(maxpt.col, col);
                    maxpt.row = max(maxpt.row, row);
                    minpt.col = min(minpt.col, col);
                    minpt.row = min(minpt.row, row);
                }
                else
                    *itnew = WHITE;
            }
        }
    }

    tiles.swap(tiles_working);
    minactive = minpt;
    maxactive = maxpt;
}


int day24_2(const stringlist& input)
{
    map<HexPt, int> numflips;
    for (auto& line : input)
    {
        auto tile = follow_path(line.c_str());
        numflips[tile]++;
    }

    HexMap state(numflips);

    const int nticks = 100;
    for (int tick = 1; tick <= nticks; ++tick)
    {
        state.tick();
    }

    return state.numblack();
}


void run_day24()
{
    test(HexPt{ 0,0 }, follow_path("nwwswee"));
    string sample =
R"(sesenwnenenewseeswwswswwnenewsewsw
neeenesenwnwwswnenewnwwsewnenwseswesw
seswneswswsenwwnwse
nwnwneseeswswnenewneswwnewseswneseene
swweswneswnenwsewnwneneseenw
eesenwseswswnenwswnwnwsewwnwsene
sewnenenenesenwsewnenwwwse
wenwwweseeeweswwwnwwe
wsweesenenewnwwnwsenewsenwwsesesenwne
neeswseenwwswnwswswnw
nenwswwsewswnenenewsenwsenwnesesenew
enewnwewneswsewnwswenweswnenwsenwsw
sweneswneswneneenwnewenewwneswswnese
swwesenesewenwneswnwwneseswwne
enesenwswwswneneswsenwnewswseenwsese
wnwnesenesenenwwnenwsewesewsesesew
nenewswnwewswnenesenwnesewesw
eneswnwswnwsenenwnwnwwseeswneewsenese
neswnwewnwnwseenwseesewsenwsweewe
wseweeenwnesenwwwswnew)";

    test(10, day24(READ(sample)));
    gogogo(day24(LOAD(24)));

    nestD(2208, day24_2(READ(sample)));
    nononoD(day24_2(LOAD(24)));
}