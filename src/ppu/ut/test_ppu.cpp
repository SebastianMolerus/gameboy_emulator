#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/ppu_impl.hpp"
#include "../src/pixel_fetcher.hpp"
#include <common.hpp>

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

class mock_rw_device : public rw_device
{
  public:
    MOCK_METHOD((uint8_t), read, (uint16_t, device), (override));
    MOCK_METHOD((void), write, (uint16_t, uint8_t, device), (override));
};

class mock_drawing_device : public drawing_device
{
  public:
    MOCK_METHOD((void), before_frame, (), (override));
    MOCK_METHOD((void), after_frame, (), (override));
    MOCK_METHOD((void), draw_pixel, (int, int, color), (override));
};

static constexpr uint16_t LCD_CTRL{0xFF40};
static constexpr uint16_t LY{0xFF44}; // current Line in dmg memory
constexpr uint8_t LCD_ENABLED{0x80};

TEST(ppu_tests, LCD_is_disabled)
{
    mock_rw_device memory_mock;
    mock_drawing_device drawing_mock;

    constexpr int dot_count{500};
    constexpr uint8_t LDC_DISABLED{0x7F};

    EXPECT_CALL(memory_mock, read(LCD_CTRL, device::PPU)).Times(dot_count).WillRepeatedly(Return(LDC_DISABLED));
    // No write to memory
    EXPECT_CALL(memory_mock, write(_, _, _)).Times(0);
    // No Draw
    EXPECT_CALL(drawing_mock, draw_pixel(_, _, _)).Times(0);
    EXPECT_CALL(drawing_mock, before_frame()).Times(0);
    EXPECT_CALL(drawing_mock, after_frame()).Times(0);

    ppu::ppu_impl p{memory_mock, drawing_mock};

    for (int i = 0; i < dot_count; ++i)
    {
        p.dot();
        ASSERT_EQ(p.m_current_state, ppu::ppu_impl::STATE::OAM_SCAN);
    }
}

TEST(ppu_tests, full_line_draw)
{
    mock_rw_device memory_mock;
    NiceMock<mock_drawing_device> drawing_mock;

    constexpr int LINE_0 = 0;

    EXPECT_CALL(memory_mock, read(LCD_CTRL, device::PPU)).WillRepeatedly(Return(LCD_ENABLED));
    EXPECT_CALL(memory_mock, write(LY, LINE_0, device::PPU)).Times(1);

    ppu::ppu_impl p{memory_mock, drawing_mock};
    ASSERT_EQ(p.m_current_line, -1);

    for (int i = 0; i < 79; ++i)
        p.dot();

    // After 79 dots, Still OAM_SCAN
    ASSERT_EQ(p.m_current_state, ppu::ppu_impl::STATE::OAM_SCAN);

    // After 80 dots: DRAWING_PIXELS
    p.dot();
    ASSERT_EQ(p.m_current_state, ppu::ppu_impl::STATE::DRAWING_PIXELS);

    // Remaining dots = 376
    for (int i = 0; i < 376; ++i)
        p.dot();

    ASSERT_EQ(p.m_current_state, ppu::ppu_impl::STATE::OAM_SCAN);
    ASSERT_EQ(p.m_current_line, 1);
}

TEST(ppu_tests, vblank_reach)
{
    NiceMock<mock_rw_device> memory_mock;
    NiceMock<mock_drawing_device> drawing_mock;

    EXPECT_CALL(memory_mock, read(LCD_CTRL, device::PPU)).WillRepeatedly(Return(LCD_ENABLED));

    constexpr int dot_count{144 * 456}; // 144 lines of LCD screen, each 456 dots
    ppu::ppu_impl p{memory_mock, drawing_mock};

    // Draw almost all 144 lines ( without 1 dot )
    for (int i = 0; i < (dot_count - 1); ++i)
        p.dot();

    // check state
    ASSERT_NE(p.m_current_state, ppu::ppu_impl::STATE::VERTICAL_BLANK);
    ASSERT_EQ(p.m_current_line, 143); // last drawable line ( 0 - 143 )

    // go to Vblank
    p.dot();

    // Vblank Begin
    ASSERT_EQ(p.m_current_state, ppu::ppu_impl::STATE::VERTICAL_BLANK);
    ASSERT_EQ(p.m_current_line, 144);

    // Vblank has always 10 scan lines ( 144 - 153 )
    for (int i = 0; i < (10 * 456) - 1; ++i)
        p.dot();

    // Vblank end
    ASSERT_EQ(p.m_current_state, ppu::ppu_impl::STATE::VERTICAL_BLANK);
    ASSERT_EQ(p.m_current_line, 153);

    // Go to next frame
    p.dot();

    ASSERT_EQ(p.m_current_state, ppu::ppu_impl::STATE::OAM_SCAN);
    ASSERT_EQ(p.m_current_line, -1);
}

TEST(ppu_tests, line_to_ids)
{
    auto res = convert_line_to_ids(0x3C7E);
    ASSERT_EQ(res[0], 0);
    ASSERT_EQ(res[1], 2);
    ASSERT_EQ(res[2], 3);
    ASSERT_EQ(res[3], 3);
    ASSERT_EQ(res[4], 3);
    ASSERT_EQ(res[5], 3);
    ASSERT_EQ(res[6], 2);
    ASSERT_EQ(res[7], 0);

    res = convert_line_to_ids(0x7e5e);
    ASSERT_EQ(res[0], 0);
    ASSERT_EQ(res[1], 3);
    ASSERT_EQ(res[2], 1);
    ASSERT_EQ(res[3], 3);
    ASSERT_EQ(res[4], 3);
    ASSERT_EQ(res[5], 3);
    ASSERT_EQ(res[6], 3);
    ASSERT_EQ(res[7], 0);
}

TEST(ppu_tests, get_tile_offset)
{
    ASSERT_EQ(get_tile_offset(0, 0), 0);
    ASSERT_EQ(get_tile_offset(7, 7), 0);
    ASSERT_EQ(get_tile_offset(8, 7), 1);
    ASSERT_EQ(get_tile_offset(15, 7), 1);
    ASSERT_EQ(get_tile_offset(16, 7), 2);
    ASSERT_EQ(get_tile_offset(23, 7), 2);
    ASSERT_EQ(get_tile_offset(24, 7), 3);

    ASSERT_EQ(get_tile_offset(247, 7), 30);

    // last tile in first row
    ASSERT_EQ(get_tile_offset(248, 7), 31);
    ASSERT_EQ(get_tile_offset(255, 7), 31);

    // first tile in second row
    ASSERT_EQ(get_tile_offset(0, 8), 32);
    ASSERT_EQ(get_tile_offset(7, 8), 32);
    ASSERT_EQ(get_tile_offset(8, 8), 33);

    // first tile in third row
    ASSERT_EQ(get_tile_offset(0, 16), 64);
    ASSERT_EQ(get_tile_offset(7, 16), 64);
    ASSERT_EQ(get_tile_offset(8, 16), 65);

    // Last pixel
    ASSERT_EQ(get_tile_offset(255, 255), 32 * 32 - 1);
}
