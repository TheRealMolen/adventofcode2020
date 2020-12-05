#include "pch.h"

#include "harness.h"

// -------------------------------------------------------------------

void day1_test();
void day2_test();
void day3_test();
void run_day4();
void run_day5();

// -------------------------------------------------------------------

int main()
{
    initcolours();
    srand((unsigned int)time(0));

    cout << GARLAND(2) << "  advent of code 2020  ~~ TheRealMolen " << GARLAND(2) << endl;


    day1_test();
    day2_test();
    day3_test();
    run_day4();
    run_day5();


    // animate snow falling behind the characters in the console until someone presses a key
    return twinkleforever();
}

