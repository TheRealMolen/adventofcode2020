#include "pch.h"

#include "harness.h"

// -------------------------------------------------------------------

int day1(const stringlist& input);
int64_t day1_2(const stringlist& input);

void day2_test();


// -------------------------------------------------------------------

int main()
{
    initcolours();
    srand((unsigned int)time(0));

    cout << GARLAND(2) << "  advent of code 2020  ~~ TheRealMolen " << GARLAND(2) << endl;

    test(514579, day1(READ("1721\n979\n366\n299\n675\n1456")));
    gogogo(day1(LOAD(1)));

    test(241861950ll, day1_2(READ("1721\n979\n366\n299\n675\n1456")));
    gogogo(day1_2(LOAD(1)));


    day2_test();


    // animate snow falling behind the characters in the console until someone presses a key
    return twinkleforever();
}

