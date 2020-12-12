#include "pch.h"
#include "harness.h"


struct Rule;
struct ChildRule
{
    int count;
    Rule* rule;

    bool operator<(const ChildRule& other) const
    {
        return (rule < other.rule);
    }
};
struct Rule
{
    string outer;

    set<ChildRule> contents;
    set<string> content_strings;

    bool operator<(const Rule& other) const
    {
        return (outer < other.outer);
    }
};

bool can_contain(const Rule& outer, const string& inner)
{
    if (ranges::any_of(outer.contents, [&](const ChildRule& child)
        {
            return child.rule->outer == inner;
        }))
    {
        return true;
    }

    for (auto& child : outer.contents)
    {
        if (can_contain(*child.rule, inner))
            return true;
    }

    return false;
}

int count_containers(const string& contents, const map<string, Rule>& rules)
{
    int count = 0;
    for (auto outerRulePair : rules)
    {
        const Rule& rule = outerRulePair.second;

        if (can_contain(rule, contents))
            ++count; 
    }

    return count;
}

map<string, Rule> read_rules(const stringlist& input)
{
    map<string, Rule> rules;

    for (auto& line : input)
    {
        auto inout = split(line, "contain");
        string outer = inout[0].substr(0, inout[0].find("bags")-1);
        trim(outer);

        Rule rule{ outer };

        auto contents = split(inout[1], ",");
        for (auto& content : contents)
        {
            trim(content, " ,.");
            auto lastspace = content.find_last_of(" ");
            content.erase(content.begin() + lastspace, content.end());
            rule.content_strings.insert(content);
        }

        rules.try_emplace(outer, rule);
    }

    for (auto& outerRulePair : rules)
    {
        Rule* prule = &outerRulePair.second;
        for (const string& content : prule->content_strings)
        {
            auto colstart = content.find(' ');
            string colour = content.substr(colstart + 1);
            int count = atoi(content.c_str());

            Rule& childrule = rules[colour];
            ChildRule child{ count, &childrule };
            prule->contents.insert(child);
        }
    }

    return rules;
}

int day7(const stringlist& input)
{
    map<string, Rule> rules = read_rules(input);
    return count_containers("shiny gold", rules);
}

int count_contents(const Rule& outer, const map<string, Rule>& rules)
{
    int count = 0;
    for (auto& child : outer.contents)
    {
        count += child.count * (1 + count_contents(*child.rule, rules));
    }
    return count;
}

int day7_2(const stringlist& input)
{
    map<string, Rule> rules = read_rules(input);
    Rule& shiny_gold = rules["shiny gold"];

    return count_contents(shiny_gold, rules);
}


void run_day7()
{
    string sample =
        R"(light red bags contain 1 bright white bag, 2 muted yellow bags.
dark orange bags contain 3 bright white bags, 4 muted yellow bags.
bright white bags contain 1 shiny gold bag.
muted yellow bags contain 2 shiny gold bags, 9 faded blue bags.
shiny gold bags contain 1 dark olive bag, 2 vibrant plum bags.
dark olive bags contain 3 faded blue bags, 4 dotted black bags.
vibrant plum bags contain 5 faded blue bags, 6 dotted black bags.
faded blue bags contain no other bags.
dotted black bags contain no other bags.)";

    string sample2 =
        R"(shiny gold bags contain 2 dark red bags.
dark red bags contain 2 dark orange bags.
dark orange bags contain 2 dark yellow bags.
dark yellow bags contain 2 dark green bags.
dark green bags contain 2 dark blue bags.
dark blue bags contain 2 dark violet bags.
dark violet bags contain no other bags.)";

    test(4, day7(READ(sample)));
    nononoD(day7(LOAD(7)));

    test(32, day7_2(READ(sample)));
    test(126, day7_2(READ(sample2)));
    nononoD(day7_2(LOAD(7)));
}