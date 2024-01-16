#include <cstdint>
#include <common.hpp>

bool LCD_PPU_ENABLE;
uint16_t WINDOW_TILE_MAP_AREA{};
bool WINDOW_ENABLE;
uint16_t BG_WINDOW_TILE_DATA_AREA{};
uint16_t BG_TILE_MAP{};
uint8_t OBJ_SIZE;
bool OBJ_ENABLE;
bool BG_WINDOW_ENABLE;

void lcd_control_settings(const uint8_t lcd_ctrl_value)
{
    if (checkbit(lcd_ctrl_value, 7))
        LCD_PPU_ENABLE = true;
    else
        LCD_PPU_ENABLE = false;

    if (checkbit(lcd_ctrl_value, 6))
        WINDOW_TILE_MAP_AREA = 0x9C00;
    else
        WINDOW_TILE_MAP_AREA = 0x9800;

    if (checkbit(lcd_ctrl_value, 5))
        WINDOW_ENABLE = true;
    else
        WINDOW_ENABLE = false;

    if (checkbit(lcd_ctrl_value, 4))
        BG_WINDOW_TILE_DATA_AREA = 0x8000;
    else
        BG_WINDOW_TILE_DATA_AREA = 0x8800;

    if (checkbit(lcd_ctrl_value, 3))
        BG_TILE_MAP = 0x9C00;
    else
        BG_TILE_MAP = 0x9800;

    if (checkbit(lcd_ctrl_value, 2))
        OBJ_SIZE = 16;
    else
        OBJ_SIZE = 8;

    if (checkbit(lcd_ctrl_value, 1))
        OBJ_ENABLE = true;
    else
        OBJ_ENABLE = false;

    if (checkbit(lcd_ctrl_value, 0))
        BG_WINDOW_ENABLE = true;
    else
        BG_WINDOW_ENABLE = false;
}