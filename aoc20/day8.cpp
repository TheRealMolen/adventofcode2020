#include "pch.h"
#include "harness.h"


class VirMax
{
public:
    VirMax(const stringlist& input);

    int run_till_repeat() const;
    int heal_and_run_to_end();

    void uncorrupt(size_t address);

private:
    enum class OpCode : uint8_t { acc, jmp, nop, COUNT};
    struct Instr
    {
        OpCode opcode;
        int operand;
    };
    struct State
    {
        size_t pc = 0;
        int64_t acc = 0;
    };

    void tick(State& state) const;

    vector<Instr> m_program;
};

static const char* const k_opcodes[] = { "acc", "jmp", "nop" };

VirMax::VirMax(const stringlist& input)
{
    static_assert(_countof(k_opcodes) == size_t(OpCode::COUNT));

    m_program.reserve(input.size());
    for (auto& line : input)
    {
        auto chunks = split(line, " ");
        int operand = atoi(chunks[1].c_str());

        if (chunks[0] == "acc")
            m_program.emplace_back(OpCode::acc, operand);
        else if (chunks[0] == "jmp")
            m_program.emplace_back(OpCode::jmp, operand);
        else if (chunks[0] == "nop")
            m_program.emplace_back(OpCode::nop, operand);
        else
            throw string("unknown opcode: ") + chunks[0];
    }
}

void VirMax::tick(VirMax::State& state) const
{
    switch (m_program[state.pc].opcode)
    {
    case OpCode::acc:
        state.acc += m_program[state.pc].operand;
        break;

    case OpCode::jmp:
        state.pc += m_program[state.pc].operand - 1;
        break;

    case OpCode::nop:
        break;
    }

    state.pc++;
}

int VirMax::run_till_repeat() const
{
    State state;
    vector<bool> visited(m_program.size(), 0);

    while (!visited[state.pc])
    {
        visited[state.pc] = true;

        tick(state);
    }

    return int(state.acc);
}

int VirMax::heal_and_run_to_end()
{
    vector<int> visited(m_program.size(), 0);

    {
        State state;
        int watchdog = 10000;

        while (watchdog > 0 && state.pc < m_program.size())
        {
            visited[state.pc]++;
            tick(state);

            --watchdog;
        }

        if (state.pc >= m_program.size())
            return int(state.acc);
    }

    // grab our list of possible corruptions
    vector<size_t> possibles;
    for (size_t i = 0; i < visited.size(); ++i)
    {
        if (visited[i] == 0)
            continue;

        if (m_program[i].opcode == OpCode::jmp || m_program[i].opcode == OpCode::nop)
            possibles.push_back(i);
    }

    for (auto possible_corruption : possibles)
    {
        uncorrupt(possible_corruption);
        State state;
        int watchdog = 10000;

        while (watchdog > 0 && state.pc < m_program.size())
        {
            visited[state.pc]++;
            tick(state);

            --watchdog;
        }

        // return it
        uncorrupt(possible_corruption);

        if (state.pc >= m_program.size())
        {
            cout << " ...auto healed corruption at address " << possible_corruption << endl;
            return int(state.acc);
        }
    }

    /*if (state.pc < m_program.size())
    {
        // dump
        for (size_t i = 0; i < m_program.size(); ++i)
        {
            cout << ((state.pc == i) ? '*' : ' ')
                << setw(4) << i
                << "  " << k_opcodes[int(m_program[i].opcode)]
                << " " << ((m_program[i].operand >= 0) ? '+' : '-') << abs(m_program[i].operand)
                << '\t' << visited[i]
                << '\n';
        }
        cout << endl;

        return -10000;
    }*/

    return -999999999;
}


void VirMax::uncorrupt(size_t address)
{
    if (m_program[address].opcode == OpCode::jmp)
        m_program[address].opcode = OpCode::nop;
    else if (m_program[address].opcode == OpCode::nop)
        m_program[address].opcode = OpCode::jmp;
    else
        throw "can't uncorrupt that";
}


int day8(const stringlist& input)
{
    VirMax vm(input);

    return vm.run_till_repeat();
}

int day8_2(const stringlist& input, size_t corrupt_addr = ~0ull)
{
    VirMax vm(input);
    if (corrupt_addr < input.size())
        vm.uncorrupt(corrupt_addr);
    
    return vm.heal_and_run_to_end();
}


void run_day8()
{
    string sample =
R"(nop +0
acc +1
jmp +4
acc +3
jmp -3
acc -99
acc +1
jmp -4
acc +6)";

    test(5, day8(READ(sample)));
    gogogo(day8(LOAD(8)), 1797);

    test(8, day8_2(READ(sample)));
    gogogo(day8_2(LOAD(8)));
}