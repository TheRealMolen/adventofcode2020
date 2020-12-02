#include "pch.h"

#include "harness.h"


struct Policy {
    int64_t lo, hi;
    char letter;

    Policy() = default;
    Policy(istream& is)
    {
        char dash, colon;
        is >> lo >> dash >> hi >> letter >> colon;
    }

    bool isSatisfiedBy(const string& pwd) const
    {
        auto count = std::count(begin(pwd), end(pwd), letter);
        return count >= lo && count <= hi;
    }

    bool isSatisfiedBy_rule2(const string& pwd) const
    {
        bool a = pwd[lo - 1] == letter;
        bool b = pwd[hi - 1] == letter;
        return a ^ b;
    }
};
istream& operator>>(istream& is, Policy& policy)
{
    policy = Policy(is);
    return is;
}


template<typename RuleFunc>
int day2_core(const stringlist& input, RuleFunc rule)
{
    int valid = 0;

    for (const auto& line : input)
    {
        string pwd;
        Policy pol;
        istringstream is(line);
        is >> pol >> pwd;

        if (rule(pol, pwd))
            ++valid;
    }

    return valid;
}

int day2(const stringlist& input)
{
    return day2_core(input, [](const Policy& pol, const string& pwd) { return pol.isSatisfiedBy(pwd); });
}

int day2_2(const stringlist& input)
{
    return day2_core(input, [](const Policy& pol, const string& pwd) { return pol.isSatisfiedBy_rule2(pwd); });
}


void day2_test()
{
    test(2, day2(READ("1 - 3 a: abcde\n1 - 3 b : cdefg\n2 - 9 c : ccccccccc")));
    gogogo(day2(LOAD(2)));

    test(1, day2_2(READ("1 - 3 a: abcde\n1 - 3 b : cdefg\n2 - 9 c : ccccccccc")));
    gogogo(day2_2(LOAD(2)));
}
