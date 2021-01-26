#include "pch.h"
#include "harness.h"

const int prime = 20201227;

int find_loop_size(int pubkey)
{
    const int subject_number = 7;
    int64_t val = 1;
    int loop = 1;
    while (1)
    {
        val *= subject_number;
        val = val % prime;

        if (val == pubkey)
            return loop;

        ++loop;
    }
}

int calc_encr_key(int pubkey, int loopsize)
{
    int64_t val = 1;
    for (int loop = 0; loop < loopsize; ++loop)
    {
        val *= pubkey;
        val = val % prime;
    }

    return int(val);
}

int day25(const stringlist& input)
{
    int card_pub = atoi(input[0].c_str());
    int door_pub = atoi(input[1].c_str());

    int card_loop = find_loop_size(card_pub);
    
    return calc_encr_key(door_pub, card_loop);
}

int day25_2(const stringlist& input)
{
    for (auto& line : input)
    {
        (void)line;
    }

    return -1;
}


void run_day25()
{
    test(8, find_loop_size(5764801));
    test(11, find_loop_size(17807724));
    test(14897079, calc_encr_key(17807724, 8));
    test(14897079, calc_encr_key(5764801, 11));

    string sample =
R"(5764801
17807724)";

    test(14897079, day25(READ(sample)));
    gogogo(day25(LOAD(25)));

    //test(-100, day25_2(READ(sample)));
    //gogogo(day25_2(LOAD(25)));
}