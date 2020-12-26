#include "pch.h"
#include "harness.h"


using LoHi = pair<int16_t, int16_t>;
using LoHiList = vector<LoHi>;
using Ticket = vector<int16_t>;
using TicketList = vector<Ticket>;
using FieldRulesMap = map<string, LoHiList>;
using FieldIndexMap = map<string, size_t>;
using FieldPossiblesMap = map<string, vector<size_t>>;
using DeducedTicket = map<string, int16_t>;

ostream& operator<<(ostream& os, const DeducedTicket& ticket)
{
    os << "( ";
    for (auto& nameix : ticket)
    {
        if (nameix != *ticket.begin())
            os << ", ";
        os << nameix.first << "=" << nameix.second;
    }
    os << " )";
    return os;
}

ostream& operator<<(ostream& os, const FieldPossiblesMap& possibles)
{
    for (auto& possible : possibles)
    {
        os << possible.first << ": ";
        for (auto ix : possible.second)
        {
            if (ix != possible.second.front())
                os << ", ";
            os << ix;
        }
        os << endl;
    }
    return os;
}


void addrange(const string& pair, LoHiList& valid)
{
    auto lohi = split(pair, "-");
    int16_t lo = int16_t(atoi(lohi[0].c_str()));
    int16_t hi = int16_t(atoi(lohi[1].c_str()));

    auto it_subset = ranges::find_if(valid, [lo, hi](const auto& pair)
        { return (lo >= pair.second && hi <= pair.first); });
    if (it_subset != valid.end())
        return;

    auto it_superset = ranges::find_if(valid, [lo, hi](const auto& pair)
        { return (lo <= pair.first && hi >= pair.second); });
    if (it_superset != valid.end())
    {
        it_superset->first = lo;
        it_superset->second = hi;
        return;
    }

    auto it_loverlap = ranges::find_if(valid, [lo, hi](const auto& pair)
        { return (lo < pair.first&& hi >= pair.first && hi <= pair.second); });
    if (it_loverlap != valid.end())
    {
        it_loverlap->first = lo;
        return;
    }

    auto it_roverlap = ranges::find_if(valid, [lo, hi](const auto& pair)
        { return (lo >= pair.first && lo <= pair.second && hi > pair.second); });
    if (it_roverlap != valid.end())
    {
        it_roverlap->second = hi;
        return;
    }

    valid.emplace_back(lo, hi);
}

Ticket parse_ticket(const string& ticket)
{
    Ticket parsed;
    parsed.reserve(ticket.size());
    ranges::transform(split(ticket, ","), back_inserter(parsed), [](const string& val) { return int16_t(atoi(val.c_str())); });
    return parsed;
}

int get_ticket_error(const Ticket& ticket, const LoHiList& valid)
{
    int error = 0;
    for (int val : ticket)
    {
        auto found = ranges::find_if(valid, [val](const auto& pair)
            {
                return (pair.first <= val) && (pair.second >= val);
            });

        if (found == valid.end())
            error += val;
    }
    return error;
}

int day16(const stringlist& input)
{
    LoHiList valid;

    int section = 0;
    int esr = 0;
    for (auto& line : input)
    {
        if (line.length() == 0)
        {
            ++section;
            continue;
        }

        if (section == 0)
        {
            string ranges = trim_copy(split(line, ":")[1]);
            for (const auto& pair : split(ranges, " or "))
                addrange(pair, valid);
        }
        else if (section == 2)
        {
            if (line.back() == ':')
                continue;

            esr += get_ticket_error(parse_ticket(line), valid);
        }
    }

    return esr;
}


void add_deduced_index(const string& field, size_t deducedix, FieldIndexMap& deduced, FieldPossiblesMap& possibles)
{
    deque<pair<string, size_t>> deductions{ make_pair(field, deducedix) };
    while (!deductions.empty())
    {
        const string& name = deductions.front().first;
        const size_t ix = deductions.front().second;
        _ASSERT(ranges::find_if(deduced, [ix](auto& nameix) { return nameix.second == ix; }) == deduced.end());
        deduced.try_emplace(name, ix);

        for (auto& other_possible : possibles)
        {
            if (other_possible.first == name)
                continue;

            auto& other_possibles = other_possible.second;
            if (other_possibles.size() <= 1)
                continue;

            other_possibles.erase(remove(begin(other_possibles), end(other_possibles), ix), end(other_possibles));
            if (other_possibles.size() == 1)
            {
                //cout << "\n**inferred " << other_possible.first << " is ix " << other_possibles.front() << endl;
                //cout << possibles << endl;

                auto dix = other_possibles.front();
                _ASSERT(ranges::find_if(deduced, [dix](auto& nameix) { return nameix.second == dix; }) == deduced.end());
                _ASSERT(ranges::find_if(deductions, [dix](auto& nameix) { return nameix.second == dix; }) == deductions.end());
                deductions.emplace_back(make_pair(other_possible.first, other_possibles.front()));
            }
        }

        deductions.pop_front();
    }

    //cout << "\npost cleanup:\n" << possibles << endl;
}

FieldIndexMap deduce_field_indices(const FieldRulesMap& rules, const TicketList& tickets)
{
    auto num_entries = tickets.front().size();
    FieldPossiblesMap possible_indices;
    // start with every field being allowed everywhere
    ranges::transform(rules, inserter(possible_indices, possible_indices.end()), [num_entries](const FieldRulesMap::value_type& field)
        {
            vector<size_t> all(num_entries);
            iota(all.begin(), all.end(), 0);
            return make_pair(field.first, all);
        });

    FieldIndexMap deduced;
    while (deduced.size() != rules.size())
    {
        for (auto& rule : rules)
        {
            auto& possibles = possible_indices[rule.first];
            if (possibles.size() == 1)
                continue;

            for (auto ixit = possibles.begin(); ixit != possibles.end(); )
            {
                size_t ix = *ixit;
                for (const Ticket& ticket : tickets)
                {
                    int val = ticket[ix];
                    auto found = ranges::find_if(rule.second, [val](const auto& pair)
                        {
                            return (pair.first <= val) && (pair.second >= val);
                        });
                    if (found == rule.second.end())
                    {
                        //cout << "  - removing " << ix << " from " << rule.first << " because of " << val << endl;
                        ixit = possibles.erase(ixit);
                        if (possibles.size() == 1)
                        {
                            //cout << "\n**deduced " << rule.first << " is ix " << possibles.front() << endl;
                            //cout << possible_indices << endl;

                            add_deduced_index(rule.first, possibles.front(), deduced, possible_indices);
                            if (deduced.size() == rules.size())
                                return deduced;
                        }
                        goto ixerased;
                    }
                }
                ++ixit;

            ixerased:
                continue;
            }
        }
    }

    // shouldn't ever reach here
    return deduced;
}

DeducedTicket deduce_ticket(const stringlist& input)
{
    LoHiList valid;
    FieldRulesMap field_valid;
    TicketList tickets;
    Ticket my_ticket;

    int section = 0;
    for (auto& line : input)
    {
        if (line.length() == 0)
        {
            ++section;
            continue;
        }
        if (line.back() == ':')
            continue;

        if (section == 0)
        {
            auto chunks = split(line, ":");
            string fieldname = trim_copy(chunks[0]);
            string ranges = trim_copy(chunks[1]);
            auto& field = field_valid[fieldname];
            for (const auto& pair : split(ranges, " or "))
            {
                addrange(pair, valid);
                addrange(pair, field);
            }
        }
        else if (section == 1)
        {
            my_ticket = parse_ticket(line);
            tickets.push_back(my_ticket);
        }
        else if (section == 2)
        {
            Ticket ticket = parse_ticket(line);
            if (get_ticket_error(ticket, valid) == 0)
            {
                // ugh, there's a ticket with a 0 which evades our detection :(
                if (ranges::find(ticket, int16_t(0)) == ticket.end())
                    tickets.push_back(ticket);
            }
        }
    }

    auto deduced_indices = deduce_field_indices(field_valid, tickets);

    DeducedTicket deduced;
    transform(begin(deduced_indices), end(deduced_indices), inserter(deduced, end(deduced)), [&my_ticket](const pair<string, size_t>& fieldix)
        {
            return make_pair(fieldix.first, my_ticket[fieldix.second]);
        });
    return deduced;
}


int64_t day16_2(const stringlist& input)
{
    DeducedTicket my_ticket = deduce_ticket(input);
    int64_t product = 1;
    for (const auto& nameval : my_ticket)
    {
        if (nameval.first.starts_with("departure"))
            product *= nameval.second;
    }

    return product;
}


void run_day16()
{
    string sample =
R"(class: 1-3 or 5-7
row: 6-11 or 33-44
seat: 13-40 or 45-50

your ticket:
7,1,14

nearby tickets:
7,3,47
40,4,50
55,2,20
38,6,12)";

    string sample2 =
R"(class: 0-1 or 4-19
row: 0-5 or 8-19
seat: 0-13 or 16-19

your ticket:
11,12,13

nearby tickets:
3,9,18
15,1,5
5,14,9)";

    test(71, day16(READ(sample)));
    gogogo(day16(LOAD(16)), 24021);

    test({ {"class"s, (int16_t)1}, {"row"s, (int16_t)7}, {"seat"s, (int16_t)14} }, deduce_ticket(READ(sample)));
    test({ {"class"s, (int16_t)12}, {"row"s, (int16_t)11}, {"seat"s, (int16_t)13} }, deduce_ticket(READ(sample2)));
    gogogo(day16_2(LOAD(16)));
}