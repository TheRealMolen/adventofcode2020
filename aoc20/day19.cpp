#include "pch.h"
#include "harness.h"

using RuleSet = map<int, string>;

string compile_rule(int ruleix, const RuleSet& rules, bool part2);

string compile_expr(const string& expr, const RuleSet& rules, bool part2)
{
    string compiled = "";
    for (const auto& sub : split(expr, " "))
    {
        int subix = atoi(sub.c_str());
        compiled += compile_rule(subix, rules, part2);
    }
    return compiled;
}

string compile_rule(int ruleix, const RuleSet& rules, bool part2)
{
    const string& rule = rules.at(ruleix);

    if (part2)
    {
        if (ruleix == 8)
        {
            // 8: 42 | 42 8
            return "(?:" + compile_expr("42", rules, true) + ")+";
        }
        else if (ruleix == 11)
        { // 11: 42 31 | 42 11 31
            // if 42 is X and 31 Y, this matches XY, XXYY, XXXYYY, ...
            // the max length input is ~90, we just add the variants that could possibly exist
            string r42 = compile_expr("42", rules, true);
            string r31 = compile_expr("31", rules, true);
            size_t max_dupes = 45;
            string compiled = "(?:";
            for (size_t i = 0; i <= max_dupes; ++i)
            {
                for (size_t j = 0; j <= i; ++j)
                    compiled += r42;
                for (size_t j = 0; j <= i; ++j)
                    compiled += r31;
                compiled += "|";
            }
            compiled.back() = ')';
            return compiled;
        }
    }

    if (rule[0] == '"')
        return rule.substr(1, 1);

    if (rule.find("|") == string::npos)
        return compile_expr(rule, rules, part2);

    string compiled{ "(?:" };
    for (const auto& alt : split(rule, " | "))
    {
        compiled += compile_expr(alt, rules, part2) + "|";
    }
    compiled.back() = ')';
    return compiled;
}

regex compile_rules(const map<int, string>& rules, bool part2)
{
    string compiled = compile_rule(0, rules, part2);
    return regex(compiled);
}

int count_valid_inputs(const stringlist& input, bool part2)
{
    RuleSet rules;
    regex compiled_rules;
    bool rules_finished = false;
    int matching_lines = 0;
    for (auto& line : input)
    {
        if (line.empty())
        {
            rules_finished = true;
            compiled_rules = compile_rules(rules, part2);
            continue;
        }

        if (!rules_finished)
        {
            auto chunks = split(line, ": ");
            int ruleix = atoi(chunks[0].c_str());
            rules.try_emplace(ruleix, chunks[1]);
        }
        else
        {
            if (regex_match(line, compiled_rules))
                ++matching_lines;
        }
    }

    return matching_lines;
}

int day19(const stringlist& input)
{
    return count_valid_inputs(input, false);
}

int day19_2(const stringlist& input)
{
    return count_valid_inputs(input, true);
}


void run_day19()
{
    string sample =
R"(0: 1 2
1: "a"
2: 1 3 | 3 1
3: "b"

aab
aba
a
aa
aaaa
aaaaab
b)";

    string sample2 =
R"(0: 4 1 5
1: 2 3 | 3 2
2: 4 4 | 5 5
3: 4 5 | 5 4
4: "a"
5: "b"

ababbb
bababa
abbbab
aaabbb
aaaabbb)";

    test(2, day19(READ(sample)));
    test(2, day19(READ(sample2)));
    gogogo(day19(LOAD(19)), 111);

    string sample3 =
R"(42: 9 14 | 10 1
9: 14 27 | 1 26
10: 23 14 | 28 1
1: "a"
11: 42 31
5: 1 14 | 15 1
19: 14 1 | 14 14
12: 24 14 | 19 1
16: 15 1 | 14 14
31: 14 17 | 1 13
6: 14 14 | 1 14
2: 1 24 | 14 4
0: 8 11
13: 14 3 | 1 12
15: 1 | 14
17: 14 2 | 1 7
23: 25 1 | 22 14
28: 16 1
4: 1 1
20: 14 14 | 1 15
3: 5 14 | 16 1
27: 1 6 | 14 18
14: "b"
21: 14 1 | 1 14
25: 1 1 | 1 14
22: 14 14
8: 42
26: 14 22 | 1 20
18: 15 15
7: 14 5 | 1 21
24: 14 1

abbbbbabbbaaaababbaabbbbabababbbabbbbbbabaaaa
bbabbbbaabaabba
babbbbaabbbbbabbbbbbaabaaabaaa
aaabbbbbbaaaabaababaabababbabaaabbababababaaa
bbbbbbbaaaabbbbaaabbabaaa
bbbababbbbaaaaaaaabbababaaababaabab
ababaaaaaabaaab
ababaaaaabbbaba
baabbaaaabbaaaababbaababb
abbbbabbbbaaaababbbbbbaaaababb
aaaaabbaabaaaaababaa
aaaabbaaaabbaaa
aaaabbaabbaaaaaaabbbabbbaaabbaabaaa
babaaabbbaaabaababbaabababaaab
aabbbbbaabbbaaaaaabbbbbababaaaaabbaaabba)";

    nestD(3, day19(READ(sample3)));
    nestD(12, day19_2(READ(sample3)));
    nonono(day19_2(LOAD(19)));
}