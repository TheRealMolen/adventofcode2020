#include "pch.h"

#include "harness.h"

// -------------------------------------------------------------------

void day1_test();
void day2_test();
void day3_test();

// -------------------------------------------------------------------

int main()
{
    initcolours();
    srand((unsigned int)time(0));

    cout << GARLAND(2) << "  advent of code 2020  ~~ TheRealMolen " << GARLAND(2) << endl;


    day1_test();
    day2_test();
    day3_test();


    // animate snow falling behind the characters in the console until someone presses a key
    return twinkleforever();
}

