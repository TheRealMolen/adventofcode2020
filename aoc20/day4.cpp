#include "pch.h"
#include "harness.h"

#include <regex>
#include <variant>


using ValidateFunc = bool(*)(const string& input);

struct FieldDef
{
    const char* name;
    variant<regex, ValidateFunc> validate;
};
struct ValidateVisitor
{
    string input;

    bool operator()(const regex& re)
    {
        return regex_match(input, re);
    }
    bool operator()(const ValidateFunc& fn)
    {
        return fn(input);
    }
};


bool check_height(const string& input)
{
    int h = atoi(input.c_str());
    if (input.rfind("cm") != string::npos)
    {
        return h >= 150 && h <= 193;
    }
    else if (input.rfind("in") != string::npos)
    {
        return h >= 59 && h <= 76;
    }
    return false;
}

const FieldDef FIELDS[]{
    {"byr:", {regex("19[2-9][0-9]|200[0-2]")}},
    {"iyr:", {regex("201[0-9]|2020")}},
    {"eyr:", {regex("202[0-9]|2030")}},
    {"hgt:", {check_height}},
    {"hcl:", {regex("#[0-9a-f]{6}")}},
    {"ecl:", {regex("amb|blu|brn|gry|grn|hzl|oth")} },
    {"pid:", {regex("^[0-9]{9}$")}},
};

int day4(const stringlist& input, bool validate=false)
{
    int nvalid = 0;

    uint32_t fields = 0;
    auto check_passport = [&]()
    {
        if ((fields & 0x7f) == 0x7f)
            ++nvalid;
    };

    for (const string& line : input)
    {
        if (line.empty())
        {
            check_passport();
            fields = 0;
            continue;
        }

        uint32_t mask = 1;
        for (auto field : FIELDS)
        {
            if (auto pos = line.find(field.name); pos != string::npos)
            {
                bool valid = true;
                if (validate)
                {
                    pos += 4;
                    auto fieldend = line.find(' ', pos);
                    auto len = fieldend != string::npos ? fieldend - pos : string::npos;
                    ValidateVisitor visitor{ line.substr(pos, len) };
                    valid = visit(visitor, field.validate);
                }
                if (valid)
                    fields |= mask;
            }

            mask <<= 1;
        }
    }
    check_passport();

    return nvalid;
}


void run_day4()
{
    test(2, day4(LOAD(4t)));
    gogogo(day4(LOAD(4)));

    test(0, day4(READ(
        R"(eyr:1972 cid:100
hcl:#18171d ecl:amb hgt:170 pid:186cm iyr:2018 byr:1926

iyr:2019
hcl:#602927 eyr:1967 hgt:170cm
ecl:grn pid:012533040 byr:1946

hcl:dab227 iyr:2012
ecl:brn hgt:182cm pid:021572410 eyr:2020 byr:1992 cid:277

hgt:59cm ecl:zzz
eyr:2038 hcl:74454a iyr:2023
pid:3556412378 byr:2007)"), true));

    test(4, day4(READ(
R"(pid:087499704 hgt:74in ecl:grn iyr:2012 eyr:2030 byr:1980
hcl:#623a2f

eyr:2029 ecl:blu cid:129 byr:1989
iyr:2014 pid:896056539 hcl:#a97842 hgt:165cm

hcl:#888785
hgt:164cm byr:2001 iyr:2015 cid:88
pid:545766238 ecl:hzl
eyr:2022

iyr:2010 hgt:158cm hcl:#b6652a ecl:blu byr:1944 eyr:2021 pid:093154719)"), true));

    gogogo(day4(LOAD(4), true));
}
