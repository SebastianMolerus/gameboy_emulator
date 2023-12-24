#include "../big_tests_utils.h"
#include <gtest/gtest.h>

namespace
{
const std::filesystem::path e8_tests_path{test_data_dir / "cpu_tests" / "v1" / "e8.json"};
}

TEST(test_cpu_BIG, e8)
{
    auto const all_data = read_cpu_data(e8_tests_path);

    for (auto const &data : all_data)
    {
        registers startup;
        startup.A() = data.initial.cpu.a;
        startup.B() = data.initial.cpu.b;
        startup.C() = data.initial.cpu.c;
        startup.D() = data.initial.cpu.d;
        startup.E() = data.initial.cpu.e;
        startup.F() = data.initial.cpu.f;
        startup.H() = data.initial.cpu.h;
        startup.L() = data.initial.cpu.l;
        startup.PC() = data.initial.cpu.pc;
        startup.SP() = data.initial.cpu.sp;

        bus b{startup};

        for (auto const &[addr, value] : data.initial.ram)
            b.m_ram[addr] = value;

        b.go();

        // if (r.A() != data.final.cpu.a)
        // {
        //     int a = 10;
        // }

        ASSERT_EQ(r.A(), data.final.cpu.a) << "Instruction: " << data.name << "\n";
    }
}