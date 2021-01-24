#include "pch.h"
#include "harness.h"

#include <ranges>


struct Ingredient
{
    string name;
    vector<string> possible_allergens;
    int mentions = 0;
};

map<string, Ingredient> isolate_ingredients(const stringlist& input)
{
    vector<pair<vector<string>, vector<string>>> parsed;
    parsed.reserve(input.size());
    for (auto& line : input)
    {
        auto chunks = split(rtrim_copy(line, ")"), " (contains ");
        auto ingredients = split(chunks[0], " ");
        ranges::sort(ingredients);
        auto allergens = split(chunks[1], ", ");
        ranges::sort(allergens);
        parsed.emplace_back(ingredients, allergens);
    }

    vector<string> all_allergens;
    for (const auto& [ingredients, allergens] : parsed)
    {
        for (const auto& allrg : allergens)
        {
            auto itfound = ranges::lower_bound(all_allergens, allrg);
            if (itfound == end(all_allergens) || *itfound != allrg)
                all_allergens.insert(itfound, allrg);
        }
    }

    map<string, Ingredient> all_ingredients;
    for (const auto& [ingredients, allergens] : parsed)
    {
        for (const auto& ingr : ingredients)
        {
            auto itfound = all_ingredients.find(ingr);
            if (itfound == end(all_ingredients))
                all_ingredients.try_emplace(ingr, Ingredient{ ingr, all_allergens, 1 });
            else
                ++itfound->second.mentions;
        }
    }

    vector<string> all_ingredient_names;
    ranges::transform(all_ingredients, back_inserter(all_ingredient_names), [](const auto& i) { return i.second.name; });

    for (const auto& [ingredients, allergens] : parsed)
    {
        vector<string> missing_ingredients;
        ranges::set_difference(all_ingredient_names, ingredients, back_inserter(missing_ingredients));
        for (const string& ingredient_name : missing_ingredients)
        {
            Ingredient& ingr = all_ingredients[ingredient_name];
            for (const auto& allergen : allergens | ranges::views::reverse)
            {
                auto itfound = ranges::lower_bound(ingr.possible_allergens, allergen);
                if (itfound != end(ingr.possible_allergens) && *itfound == allergen)
                    ingr.possible_allergens.erase(itfound);
            }
        }
    }

    return all_ingredients;
}

int day21(const stringlist& input)
{
    auto all_ingredients = isolate_ingredients(input);

    return accumulate(begin(all_ingredients), end(all_ingredients), 0, [](int acc, const auto& kv) -> int
        {
            const Ingredient& ingr = kv.second;
            if (ingr.possible_allergens.empty())
                return acc + ingr.mentions;
            return acc;
        });
}

string day21_2(const stringlist& input)
{
    auto all_ingredients = isolate_ingredients(input);

    // strip out safe ingredients
    for (auto it = begin(all_ingredients); it != end(all_ingredients); )
    {
        if (it->second.possible_allergens.empty())
            it = all_ingredients.erase(it);
        else
            ++it;
    }

    vector<Ingredient> identified;
    identified.reserve(all_ingredients.size());
    while (!all_ingredients.empty())
    {
        auto it = ranges::find_if(all_ingredients, [](const auto& pair)
            {
                return pair.second.possible_allergens.size() == 1;
            });

        _ASSERT(it != end(all_ingredients));

        const string& allergen = it->second.possible_allergens.front();

        for (auto itother = begin(all_ingredients); itother != end(all_ingredients); ++itother)
        {
            if (itother == it)
                continue;

            auto& other = itother->second;
            auto itfound = ranges::lower_bound(other.possible_allergens, allergen);
            if (itfound != end(other.possible_allergens) && *itfound == allergen)
                other.possible_allergens.erase(itfound);
        }

        identified.emplace_back(move(it->second));
        all_ingredients.erase(it);
    }

    ranges::sort(identified, {}, [](const auto& ingr) { return ingr.possible_allergens.front(); });

    string res;
    bool first = true;
    for (const string& name : identified | views::transform(&Ingredient::name))
    {
        if (!first)
            res += ",";
        res += name;
        first = false;
    }

    return res;
}


void run_day21()
{
    string sample =
R"(mxmxvkd kfcds sqjhc nhms (contains dairy, fish)
trh fvjkl sbzzf mxmxvkd (contains dairy)
sqjhc fvjkl (contains soy)
sqjhc mxmxvkd sbzzf (contains fish))";

    test(5, day21(READ(sample)));
    gogogo(day21(LOAD(21)));

    test("mxmxvkd,sqjhc,fvjkl", day21_2(READ(sample)));
    gogogo(day21_2(LOAD(21)));
}