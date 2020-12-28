#include "pch.h"
#include "harness.h"


int64_t evaluate_expr(istream& is);

int64_t evaluate_paren(istream& is)
{
    [[maybe_unused]] int openparen = is.get();
    _ASSERT(openparen == '(');

    int64_t acc = evaluate_expr(is);

    [[maybe_unused]] int closeparen = is.get();
    _ASSERT(closeparen == ')');

    return acc;
}

int64_t evaluate_expr(istream& is)
{
    int64_t acc;
    if (is.peek() == '(')
        acc = evaluate_paren(is);
    else
        is >> acc;

    while (is.peek() != EOF && is.peek() != ')')
    {
        char op;
        int64_t operand;
        is >> op >> ws;

        if (is.peek() == '(')
            operand = evaluate_paren(is);
        else
            is >> operand;

        if (op == '+')
            acc += operand;
        else if (op == '*')
            acc *= operand;
        else throw "wtf";
    }

    return acc;
}

int64_t evaluate_expr(const string& input)
{
    istringstream is(input);
    is.setf(ios::skipws);

    return evaluate_expr(is);
}

int64_t day18(const stringlist& input)
{
    int64_t sum = 0;
    for (auto& line : input)
    {
        sum += evaluate_expr(line);
    }
    return sum;
}


regex re_paren("\\(([^()]+)\\)");
regex re_plainparen("\\(([0-9]+)\\)");
regex re_plus("([0-9]+)\\+([0-9]+)");
regex re_times("([0-9]+)\\*([0-9]+)");
regex re_num("[0-9]+");

int64_t evaluate_expr_advanced(const string& input)
{
    // do it with regex!
    string curr(input);
    curr.erase(remove(begin(curr), end(curr), ' '), end(curr)); // lose the spaces

    while (!regex_match(curr, re_num))
    {
        smatch m;
        while (regex_search(curr, m, re_paren))
        {
            string res = to_string(evaluate_expr_advanced(m[1].str()));
            curr.replace(m[0].first, m[0].second, res);
        }

        if (regex_search(curr, m, re_plainparen))
        {
            curr.replace(m[0].first, m[0].second, m[1].str());
            continue;
        }

        if (regex_search(curr, m, re_plus))
        {
            int64_t a = strtoll(m[1].str().c_str(), nullptr, 10);
            int64_t b = strtoll(m[2].str().c_str(), nullptr, 10);
            string res = to_string(a + b);
            curr.replace(m[0].first, m[0].second, res);
            continue;
        }

        if (regex_search(curr, m, re_times))
        {
            int64_t a = strtoll(m[1].str().c_str(), nullptr, 10);
            int64_t b = strtoll(m[2].str().c_str(), nullptr, 10);
            string res = to_string(a * b);
            curr.replace(m[0].first, m[0].second, res);
            continue;
        }
    }

    return strtoll(curr.c_str(), nullptr, 10);;
}

int64_t day18_2(const stringlist& input)
{
    int64_t sum = 0;
    for (auto& line : input)
    {
        sum += evaluate_expr_advanced(line);
    }
    return sum;
}


void run_day18()
{
    test(71, evaluate_expr("1 + 2 * 3 + 4 * 5 + 6"));
    test(51, evaluate_expr("1 + (2 * 3) + (4 * (5 + 6))"));
    test(26, evaluate_expr("2 * 3 + (4 * 5)"));
    test(437, evaluate_expr("5 + (8 * 3 + 9 + 3 * 4 * 3)"));
    test(12240, evaluate_expr("5 * 9 * (7 * 3 * 3 + 9 * 3 + (8 + 6 * 4))"));
    test(13632, evaluate_expr("((2 + 4 * 9) * (6 + 9 * 8 + 6) + 6) + 2 + 4 * 2"));
    gogogo(day18(LOAD(18)));

    test(231, evaluate_expr_advanced("1 + 2 * 3 + 4 * 5 + 6"));
    test(51, evaluate_expr_advanced("1 + (2 * 3) + (4 * (5 + 6))"));
    test(46, evaluate_expr_advanced("2 * 3 + (4 * 5)"));
    test(1445, evaluate_expr_advanced("5 + (8 * 3 + 9 + 3 * 4 * 3)"));
    test(669060, evaluate_expr_advanced("5 * 9 * (7 * 3 * 3 + 9 * 3 + (8 + 6 * 4))"));
    test(23340, evaluate_expr_advanced("((2 + 4 * 9) * (6 + 9 * 8 + 6) + 6) + 2 + 4 * 2"));
    nononoD(day18_2(LOAD(18)));
}