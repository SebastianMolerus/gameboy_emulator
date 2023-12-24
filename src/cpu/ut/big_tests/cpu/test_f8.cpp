#include "../big_tests_utils.h"
#include <gtest/gtest.h>

namespace
{
const std::filesystem::path e8_tests_path{test_data_dir / "cpu_tests" / "v1" / "f8.json"};
}

TEST(test_cpu_BIG, f8)
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

        ASSERT_EQ(r.A(), data.final.cpu.a);
        ASSERT_EQ(r.B(), data.final.cpu.b);
        ASSERT_EQ(r.C(), data.final.cpu.c);
        ASSERT_EQ(r.D(), data.final.cpu.d);
        ASSERT_EQ(r.F(), data.final.cpu.f);
        ASSERT_EQ(r.E(), data.final.cpu.e);
        ASSERT_EQ(r.H(), data.final.cpu.h);
        ASSERT_EQ(r.L(), data.final.cpu.l);
        ASSERT_EQ(r.PC(), data.final.cpu.pc);
        ASSERT_EQ(r.SP(), data.final.cpu.sp);
    }
}